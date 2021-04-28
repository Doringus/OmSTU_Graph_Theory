#include "branchandbound.h"

#include <QDebug>
#include <iostream>
#include <QThread>

const double MAX_VALUE = 9999;


static void printMatrix(const GraphMatrix& matrix) {
    QDebug deb = qDebug();
    for(const auto& row: matrix) {
        for(const auto& val: row) {
            deb.nospace() << val << " ";
        }
        deb << "\n";
    }
}

static QList<double> createColumn(const GraphMatrix& matrix, int col) {
    QList<double> result;
    for(int i = 0; i < matrix.count(); ++i) {
        result.append(matrix.at(i).at(col));
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// BranchTask //////////////////////////////////////////////
void BranchTask::run() {
    while(m_CurrentNode->getVisitedVerticesCount() < m_TotalVerticesCount) {
        iterate();
    }
    emit finished(m_CurrentNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// ParallelPenaltyTask ///////////////////////////////////////

ParallelPenaltyTask::ParallelPenaltyTask(Node *rootNode, double topBound, int totalVerticesCount, QPair<int, int> edge, QObject *parent) :
                                        BBTask(rootNode, topBound, totalVerticesCount, parent),
                                        m_Edge(std::move(edge)) {

}

void ParallelPenaltyTask::run() {
    createNextBranch(m_Edge);
    while(m_CurrentNode->getVisitedVerticesCount() < m_TotalVerticesCount) {
        iterate();
    }

    emit finished(m_CurrentNode);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////BranchAndBound/////////////////////////////////////////////

void BBTask::iterate() {
    if(m_CurrentNode->getVisitedVerticesCount() != m_CurrentNode->getMatrix().count() - 1 && m_CurrentNode->getVisitedVerticesCount() > 1) {
        m_CurrentNode->removeLoops();
    }
    auto penalties = bb::getPathWithMaxPenalty(m_CurrentNode->getMatrixRef());
    if(penalties.isEmpty()) {
        m_CurrentNode->setVisitedVerticesCount(m_TotalVerticesCount);
        return;
    }
    if(penalties.count() > 1) {
        for(int i = 1; i < penalties.count(); ++i) {
            Node *brother = Node::createBrother(m_CurrentNode);
            BBTask *task = createBBTask<ParallelPenaltyTask>(brother, m_TopBound, m_TotalVerticesCount, penalties.at(i).vertices);
            emit subtaskCreated(task);
        }
    }
   createNextBranch(penalties.first().vertices);
}

void BBTask::createNextBranch(const QPair<int, int>& edge) {
    Node *leftNode = Node::createChild(m_CurrentNode);
    Node *rightNode = Node::createChild(m_CurrentNode);

    int count = m_CurrentNode->getMatrix().count();

    /// Clear parent node fields to save memory
    m_CurrentNode->getMatrixRef().clear();
    m_CurrentNode->getMatrixRef().squeeze();
    if(Q_LIKELY(m_CurrentNode->hasEdges())) {
        m_CurrentNode->clearUnusedData();
    }
    if(count < 10) {
        m_CurrentNode->setLeftChild(leftNode);
        m_CurrentNode->setRightChild(rightNode);
    } else {
        leftNode->setParent(nullptr);
        rightNode->setParent(nullptr);
    }


    bb::createLeftNode(leftNode, edge);
    bb::createRightNode(rightNode, edge);
    if(rightNode->getWeight() == leftNode->getWeight() && m_TotalVerticesCount - m_CurrentNode->getVisitedVerticesCount() > 2) {
        BBTask *task = createBBTask<BranchTask>(rightNode, m_TopBound, m_TotalVerticesCount);
        emit subtaskCreated(task);
    }

    if(count >= 10) {
        delete m_CurrentNode;
    }
    if(rightNode->getWeight() < leftNode->getWeight()) {
        if(Q_UNLIKELY(rightNode->getWeight() > m_TopBound)) {
            m_CurrentNode = rightNode;
            m_CurrentNode->setVisitedVerticesCount(m_TotalVerticesCount);
            m_CurrentNode->setWeight(m_TopBound);
            m_CurrentNode->clearUnusedData();
            return;
        }
        m_CurrentNode = rightNode;
        if(count >= 10) {
            delete leftNode;
        }
    } else {
        if(Q_UNLIKELY(leftNode->getWeight() > m_TopBound)) {
            m_CurrentNode = leftNode;
            m_CurrentNode->setVisitedVerticesCount(m_TotalVerticesCount);
            m_CurrentNode->setWeight(m_TopBound);
            m_CurrentNode->clearUnusedData();
            return;
        }
        m_CurrentNode = leftNode;
        if(count > 10) {
            delete rightNode;
        }
    }
}

BranchAndBound::BranchAndBound(QObject *parent) : IAlgorithm(parent), m_RootNode(nullptr) {
    StaticThreadPool *pool = new StaticThreadPool(this);
    m_Executor.setThreadPool(pool);
    connect(&m_Executor, &BBExecutor::taskFinished, this, &BranchAndBound::handleBB);
}

void BranchAndBound::start(const GraphMatrix &matrix) {
    if(!matrix.count()) {
        return;
    }
    m_Results.clear();
    deleteOldTree();
    m_Matrix = matrix;
    for(int i = 0; i < m_Matrix.count(); ++i) {
        m_Matrix[i][i] = MAX_VALUE;
    }

    Node* node = new Node();

    for(int i = 0; i < m_Matrix.count(); ++i) {
        m_Matrix[i][i] = MAX_VALUE;
    }
    double topBound = bb::findSimpleWay(m_Matrix);
    node->setWeight(bb::reduceMatrix(m_Matrix));
    node->setMatrix(m_Matrix);
    node->setIsInPath(true);
    m_RootNode = node;
    BBTask *task = createBBTask<BranchTask>(node, topBound, m_Matrix.count());

    m_Executor.putTask(task);
}

void BranchAndBound::handleBB(Node *node) {
    if(node == nullptr) {
        // for profiler (memory)
        emit lastTaskFinished();
        findOptimalPath();
    } else {
        m_Results.append(node);
    }

}

void BranchAndBound::deleteOldTree() {
    delete m_RootNode;
}

void BranchAndBound::findOptimalPath() {
    Node* node = *std::min_element(m_Results.begin(), m_Results.end(), [](Node *lhs, Node *rhs) {
        return lhs->getWeight() < rhs->getWeight();
    });
    Node *endNode = node;
    m_Results.removeOne(endNode);
    /* Mark optimal path */
    if(m_Matrix.count() < 10) {
        while (node != m_RootNode) {
            node->setIsInPath(true);
            if(node->getParent() == nullptr && node->getBrother()) {
                // swap subtrees
                std::swap(node->getBrother()->getLeftChildRef(), node->getLeftChildRef());
                std::swap(node->getBrother()->getRightChildRef(), node->getRightChildRef());
                std::swap(node->getBrother()->getLeftChild()->getParentRef(), node->getLeftChild()->getParentRef());
                std::swap(node->getBrother()->getRightChild()->getParentRef(), node->getRightChild()->getParentRef());
                node->setIsInPath(false);
                Node* brother = node->getBrother();
                node->setBrother(nullptr);
                node = brother;
            } else {
                node = node->getParent();
            }
        }
    }

    /* Clean other paths */
    for(auto n : m_Results) {
        while (n && !n->isInPath()) {
            Node *parent = n->getParent();

            /// parent in path => one child in path tree
            /// => another child shouldnt be deleted to build tree in ui
            if (parent && !parent->isInPath()) {
                /// delete parent`s childrens
                /// if one child has own childs => it`s childrens belongs to another subtree
                /// it will be deleted later
                if(!parent->getLeftChild()->getLeftChild()) {
                    parent->deleteLeftChild();
                    parent->deleleRightChild();
                }
            }
            if(parent == nullptr) {
                delete n;
            }

            n = parent;
        }
    }
    qDebug() << endNode->getWeight() << "\n\n";
    if(m_Matrix.count() >= 10) {
        m_RootNode = endNode;
    }
    emit bbFinished(endNode, m_RootNode);
    // for profiler
    emit finished();
}

Node::Node() : m_Parent(nullptr), m_Brother(nullptr), m_Left(nullptr), m_Right(nullptr), m_IsInPath(false),
                 m_VisitedVerticesCount(0) {

}

Node::Node(const Node &other) : m_Parent(other.m_Parent),
                        m_Left(other.m_Left), m_Right(other.m_Right), m_Brother(other.m_Brother),
                        m_Matrix(other.m_Matrix), m_Edges(other.m_Edges),
                        m_ExcludedEdges(other.m_ExcludedEdges), m_VisitedVerticesCount(other.m_VisitedVerticesCount),
                        m_Weight(other.m_Weight), m_IsInPath(false){

}

Node::~Node() {
    if(m_Left != nullptr) {
        delete m_Left;
        m_Left = nullptr;
    }
    if(m_Right != nullptr) {
        delete m_Right;
        m_Right = nullptr;
    }
}

Node* Node::createChild(Node *parent) {
    Node *node = new Node(*parent);
    node->setParent(parent);
    return node;
}

Node *Node::createBrother(Node *brother) {
    Node *node = new Node(*brother);
    node->setParent(nullptr);
    node->setBrother(brother);
    return node;
}

QList<QPair<int, int>> Node::getIncludedEdges() {
    if(m_Edges.count() == 0) {
        return QList<QPair<int, int>>();
    }
    return m_Edges.first();
}

void Node::removeLoops() {
    for(const auto& path: m_Edges) {
        int pathCount = path.count();
        for(int i = 1; i < pathCount; ++i) {
            m_Matrix[path.at(i).second][path.first().first] = MAX_VALUE;
        }
    }
}

void Node::includeEdge(const QPair<int, int> &edge) {
    m_VisitedVerticesCount++;
    m_LastIncludedEdge = edge;
    int prependIndex = -1;
    int appendIndex = -1;
    int edgesCount = m_Edges.count();
    for(int i = 0; i < edgesCount; i++) {
        if(m_Edges[i].empty()) {
            continue;
        }
        if(m_Edges[i].first().first == edge.second) {
            prependIndex = i;
        }
        if(m_Edges[i].last().second == edge.first) {
            appendIndex = i;
        }
    }

    if(prependIndex == appendIndex) {
        appendIndex = -1;
    }

    if(prependIndex != -1 && appendIndex != -1) {
        /// merge lists
        if(prependIndex < appendIndex) {
            appendIndex--;
        }
        auto lhs = m_Edges.takeAt(prependIndex);
        auto rhs = m_Edges.takeAt(appendIndex);
        auto path = QList<QPair<int, int>>();
        path.append(rhs);
        path.append(edge);
        path.append(lhs);
        m_Edges.append(path);
    } else if(prependIndex != -1) {
        m_Edges[prependIndex].prepend(edge);
    } else if(appendIndex != -1) {
        m_Edges[appendIndex].append(edge);
    } else {
        m_Edges.append({edge});
    }
}

void Node::excludeEdge(const QPair<int, int> &edge) {
    m_LastExcludedEdge = edge;
    m_ExcludedEdges.append(edge);
}

int Node::getVisitedVerticesCount() const {
    return m_VisitedVerticesCount;
}

void Node::setVisitedVerticesCount(int count) {
    m_VisitedVerticesCount = count;
}

void Node::deleteLeftChild() {
    delete m_Left;
    m_Left = nullptr;
}

void Node::deleleRightChild() {
    delete m_Right;
    m_Right = nullptr;
}

bool Node::hasEdges() {
    return !m_Edges.isEmpty() || !m_ExcludedEdges.isEmpty();
}

void Node::clearUnusedData() {
    m_Edges.clear();
    m_Edges.squeeze();
    m_ExcludedEdges.clear();
    m_ExcludedEdges.squeeze();
}

Node* Node::getLeftChild() const {
    return m_Left;
}

Node* Node::getRightChild() const {
    return m_Right;
}

Node* Node::getParent() const {
    return m_Parent;
}

Node* Node::getBrother() const {
    return m_Brother;
}

Node*& Node::getLeftChildRef() {
    return m_Left;
}

Node*& Node::getRightChildRef() {
    return m_Right;
}

Node*& Node::getParentRef() {
    return m_Parent;
}

void Node::setLeftChild(Node* node) {
    m_Left = node;
}

void Node::setRightChild(Node* node) {
    m_Right = node;
}

void Node::setParent(Node* node) {
    m_Parent = node;
}

void Node::setBrother(Node* node) {
    m_Brother = node;
}

GraphMatrix Node::getMatrix() const {
    return m_Matrix;
}

GraphMatrix& Node::getMatrixRef() {
    return m_Matrix;
}

void Node::setMatrix(const GraphMatrix &matrix) {
    m_Matrix = matrix;
}

double Node::getWeight() const {
    return m_Weight;
}

void Node::setWeight(double weight) {
    m_Weight = weight;
}

bool Node::isInPath() const {
    return m_IsInPath;
}

void Node::setIsInPath(bool path) {
    m_IsInPath = path;
}

QPair<int, int> Node::getLastIncludedEdge() const {
    return m_LastIncludedEdge;
}

QPair<int, int> Node::getLastExcludedEdge() const {
    return m_LastExcludedEdge;
}

double bb::findSimpleWay(const GraphMatrix &matrix) {
    double weight = 0;
    int matrixCount = matrix.count() - 1;
    for(int i = 0; i < matrixCount; ++i) {
        weight += matrix[i][i + 1];
    }
    weight += matrix[matrix.count() - 1][0];
    return weight;
}

double bb::reduceMatrix(GraphMatrix &matrix) {
    QList<double> rowReduce = getMinByRows(matrix);
    int matrixCount = matrix.count();
    for(int i = 0; i < matrixCount; ++i) {
        for(int j = 0; j < matrixCount; ++j) {
            if(matrix[i][j] != MAX_VALUE) {
                matrix[i][j] -= rowReduce.at(i);
            }
        }
    }
    QList<double> columnReduce = getMinByColumns(matrix);
    for(int j = 0; j < matrixCount; ++j) {
        for(int i = 0; i < matrixCount; ++i) {
            if(matrix[i][j] != MAX_VALUE) {
                matrix[i][j] -= columnReduce.at(j);
            }
        }
    }
    rowReduce.removeAll(MAX_VALUE);
    columnReduce.removeAll(MAX_VALUE);
    double minBound = 0;
    for(const auto& v : rowReduce) {
        minBound = std::move(minBound) + v;
    }
    for(const auto& v: columnReduce) {
        minBound = std::move(minBound) + v;
    }
    return minBound;
}

QList<double> bb::getMinByRows(const GraphMatrix &matrix) {
    QList<double> minByRows;
    for(auto& row : matrix) {
        double min = *std::min_element(row.begin(), row.end());
        if(min == MAX_VALUE) {
            min = 0;
        }
        minByRows.append(min);
    }
    return minByRows;
}

QList<double> bb::getMinByColumns(const GraphMatrix &matrix) {
    QList<double> minByColumns;
    int matrixCount = matrix.count();
    for(int j = 0; j < matrixCount; ++j) {
        double min = MAX_VALUE;
        for(int i = 0; i < matrixCount; ++i) {
            if(matrix[i][j] < min) {
                min = matrix[i][j];
            }
        }
        if(min == MAX_VALUE) {
            min = 0;
        }
        minByColumns.append(min);
    }
    return minByColumns;
}

QList<penalty_t> bb::getPathWithMaxPenalty(GraphMatrix &matrix) {
    QList<penalty_t> penalties;
    int matrixCount = matrix.count();
    for(int i = 0; i < matrixCount; ++i) {
        for(int j = 0; j < matrixCount; ++j) {
            if(matrix[i][j] != MAX_VALUE) {
                if(matrix[i][j] == 0) {
                    double weight = getMinByRowExcept(matrix.at(i), j) + getMinByRowExcept(createColumn(matrix, j), i);
                   // double weight = getMinByRowExcept(matrix, i, j) + getMinByColumnExcept(matrix, j, i);
                    penalties.append({{i, j}, weight});
                }
            }
        }
    }

    double maxWeight = -1;
    for(const auto& penalty : penalties) {
        if(penalty.weight > maxWeight) {
            maxWeight = penalty.weight;
        }
    }
    QList<penalty_t> result;
    for(const auto& penalty : penalties) {
        if(penalty.weight == maxWeight) {
            result.append(penalty);
        }
    }
    return result;
}

double bb::getMinByRowExcept(GraphMatrix &matrix, int row, int exceptionIndex) {
    double value = matrix[row][exceptionIndex];
    matrix[row][exceptionIndex] = MAX_VALUE;
    double min = *std::min_element(matrix.at(row).begin(), matrix.at(row).end());
    matrix[row][exceptionIndex] = value;
    return min;
}

double bb::getMinByColumnExcept(GraphMatrix &matrix, int col, int exceptionIndex) {
    double min = MAX_VALUE;
    double value = matrix[exceptionIndex][col];
    matrix[exceptionIndex][col] = MAX_VALUE;
    int matrixCount = matrix.count();
    for(int i = 0; i < matrixCount; ++i) {
        if(i == exceptionIndex) {
            continue;
        }
        if(matrix[i][col] < min) {
            min = matrix[i][col];
        }
    }
    matrix[exceptionIndex][col] = value;
    return min;
}

double bb::getMinByRowExcept(QList<double> row, int exceptionIndex) {
    row[exceptionIndex] = MAX_VALUE;
    double min = *std::min_element(row.begin(), row.end());
    return min;
}

void bb::createLeftNode(Node *leftNode, const QPair<int, int> &edge) {
    GraphMatrix& matrix = leftNode->getMatrixRef();
    int matrixCount = matrix.count();
    for(int i = 0; i < matrixCount; ++i) {
        matrix[i][edge.second] = MAX_VALUE;
        matrix[edge.first][i] = MAX_VALUE;
    }
    matrix[edge.second][edge.first] = MAX_VALUE;
    double weight = reduceMatrix(matrix);
    leftNode->setWeight(leftNode->getWeight() + weight);
    leftNode->includeEdge(edge);
}

void bb::createRightNode(Node *rightNode, const QPair<int, int> &edge) {
    rightNode->getMatrixRef()[edge.first][edge.second] = MAX_VALUE;
    double weight = reduceMatrix(rightNode->getMatrixRef());
    rightNode->setWeight(rightNode->getWeight() + weight);
    rightNode->excludeEdge(edge);
}

