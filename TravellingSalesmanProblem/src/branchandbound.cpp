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

static node_t* createNode(node_t *parent) {
    node_t *node = new node_t;
    node->weight = parent->weight;
    node->matrix = parent->matrix;
    node->parent = parent;
    node->excludedEdges = parent->excludedEdges;
    node->includedEdges = parent->includedEdges;
    node->visitedVertices = parent->visitedVertices;
    return node;
}

static node_t* createBroter(node_t *brother) {
    node_t *node = new node_t;
    node->weight = brother->weight;
    node->matrix = brother->matrix;
    node->brother = brother;
    node->excludedEdges = brother->excludedEdges;
    node->includedEdges = brother->includedEdges;
    node->visitedVertices = brother->visitedVertices;
    return node;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// BranchTask //////////////////////////////////////////////
void BranchTask::run() {
    while(m_CurrentNode->visitedVertices.count() < m_Matrix.count()) {
        iterate();
    }
    emit finished(m_CurrentNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// ParallelPenaltyTask ///////////////////////////////////////

ParallelPenaltyTask::ParallelPenaltyTask(node_t *rootNode, double topBound, const GraphMatrix &matrix, QPair<int, int> edge, QObject *parent) :
                                        BBTask(rootNode, topBound, matrix, parent),
                                        m_Edge(std::move(edge)) {

}

void ParallelPenaltyTask::run() {
    createNextBranch(m_Edge);
    while(m_CurrentNode->visitedVertices.count() < m_Matrix.count()) {
        iterate();
    }
    emit finished(m_CurrentNode);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////BranchAndBound/////////////////////////////////////////////

void BBTask::iterate() {
    if(m_CurrentNode->visitedVertices.count() != m_CurrentNode->matrix.count() - 1) {
        removeLoop(m_CurrentNode);
    }
    auto penalties = getPathWithMaxPenalty(m_CurrentNode->matrix);
    if(penalties.isEmpty()) {
        m_CurrentNode->visitedVertices.append(QList<int>(m_Matrix.count(), 1));
        m_CurrentNode->weight = MAX_VALUE;
        return;
    }
    if(penalties.count() > 1) {
        for(int i = 1; i < penalties.count(); ++i) {
            node_t *brother = createBroter(m_CurrentNode);
            BBTask *task = createBBTask<ParallelPenaltyTask>(brother, m_TopBound, m_Matrix, penalties.at(i).vertices);
            emit subtaskCreated(task);
        }
    }
   createNextBranch(penalties.first().vertices);
}

void BBTask::createNextBranch(const QPair<int, int>& edge) {
    node_t *leftNode = createNode(m_CurrentNode);
    node_t *rightNode = createNode(m_CurrentNode);

    m_CurrentNode->left = leftNode;
    m_CurrentNode->right = rightNode;

    createLeftNode(leftNode, edge);
    createRightNode(rightNode, edge);
    if(rightNode->weight == leftNode->weight) {
        BBTask *task = createBBTask<BranchTask>(rightNode, m_TopBound, m_Matrix);
        emit subtaskCreated(task);
    }
    if(rightNode->weight < leftNode->weight) {
        if(rightNode->weight > m_TopBound) {
            m_CurrentNode->visitedVertices.fill(m_RootNode->matrix.count());
            m_CurrentNode = m_RootNode;
            return;
        }
        m_CurrentNode = rightNode;
    } else {
        if(leftNode->weight > m_TopBound) {
            m_CurrentNode->visitedVertices.fill(m_RootNode->matrix.count());
            m_CurrentNode = m_RootNode;
            return;
        }
        m_CurrentNode = leftNode;
    }
}

double BBTask::findSimpleWay(const GraphMatrix &matrix) {
    double weight = 0;
    for(int i = 0; i < matrix.count() - 1; ++i) {
        weight += matrix[i][i + 1];
    }
    weight += matrix[matrix.count() - 1][0];
    return weight;
}

double BBTask::reduceMatrix(GraphMatrix &matrix) {
    QList<double> rowReduce = getMinByRows(matrix);
    for(int i = 0; i < matrix.count(); ++i) {
        for(int j = 0; j < matrix.count(); ++j) {
            if(matrix[i][j] != MAX_VALUE) {
                matrix[i][j] -= rowReduce.at(i);
            }
        }
    }
    QList<double> columnReduce = getMinByColumns(matrix);
    for(int j = 0; j < matrix.count(); ++j) {
        for(int i = 0; i < matrix.count(); ++i) {
            if(matrix[i][j] != MAX_VALUE) {
                matrix[i][j] -= columnReduce.at(j);
            }
        }
    }
    rowReduce.removeAll(MAX_VALUE);
    columnReduce.removeAll(MAX_VALUE);
    double minBound = std::accumulate(rowReduce.begin(), rowReduce.end(), 0) +
            std::accumulate(columnReduce.begin(), columnReduce.end(), 0);
    return minBound;
}

QList<double> BBTask::getMinByRows(const GraphMatrix &matrix) {
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

QList<double> BBTask::getMinByColumns(const GraphMatrix &matrix) {
    QList<double> minByColumns;
    for(int j = 0; j < matrix.count(); ++j) {
        double min = MAX_VALUE;
        for(int i = 0; i < matrix.count(); ++i) {
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

QList<penalty_t> BBTask::getPathWithMaxPenalty(GraphMatrix &matrix) const {
    QList<penalty_t> penalties;
    for(int i = 0; i < matrix.count(); ++i) {
        for(int j = 0; j < matrix.count(); ++j) {
            if(matrix[i][j] != MAX_VALUE) {
                if(matrix[i][j] == 0) {
                    double weight = getMinByRowExcept(matrix, i, j) + getMinByColumnExcept(matrix, j, i);
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

double BBTask::getMinByRowExcept(GraphMatrix &matrix, int row, int exceptionIndex) const {
    double value = matrix[row][exceptionIndex];
    matrix[row][exceptionIndex] = MAX_VALUE;
    double min = *std::min_element(matrix.at(row).begin(), matrix.at(row).end());
    matrix[row][exceptionIndex] = value;
    return min;
}

double BBTask::getMinByColumnExcept(GraphMatrix &matrix, int col, int exceptionIndex) const {
    double min = MAX_VALUE;
    double value = matrix[exceptionIndex][col];
    matrix[exceptionIndex][col] = MAX_VALUE;
    for(int i = 0; i < matrix.count(); ++i) {
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

void BBTask::createLeftNode(node_t *leftNode, const QPair<int, int> &edge) {
    for(int i = 0; i < leftNode->matrix.count(); ++i) {
        leftNode->matrix[i][edge.second] = MAX_VALUE;
        leftNode->matrix[edge.first][i] = MAX_VALUE;
    }
    leftNode->matrix[edge.second][edge.first] = MAX_VALUE;
    double weight = reduceMatrix(leftNode->matrix);
    leftNode->weight += weight;
    leftNode->includedEdges.append(edge);
    leftNode->visitedVertices.append(edge.second);
}

void BBTask::createRightNode(node_t *rightNode, const QPair<int, int> &edge) {
    rightNode->matrix[edge.first][edge.second] = MAX_VALUE;
    double weight = reduceMatrix(rightNode->matrix);
    rightNode->weight += weight;
    rightNode->excludedEdges.append(edge);
}

/*!  WARNING! VERY STUPID CODE HERE!!!*/
bool BBTask::checkLoop(const QList<QPair<int, int>> &edges) {
    QSet<int> vertices;
    for(const auto& edge : edges) {
        vertices.insert(edge.first);
        vertices.insert(edge.second);
    }
    for(const auto& edge : edges) {
        vertices.remove(edge.second);
    }
    return vertices.empty();
}

void BBTask::removeLoop(node_t *node) {
    for(int i = 0; i < node->matrix.count(); ++i) {
        for(int j = 0; j < node->matrix.count(); ++j) {
            if(node->matrix.at(i).at(j) != MAX_VALUE) {
                QList<QPair<int, int>> edges = node->includedEdges;
                edges.append({i, j});
                if(checkLoop(edges)) {
                    node->matrix[i][j] = MAX_VALUE;
                }
            }
        }
    }
}

void BBTask::deleteTree(node_t *endNode) {
    while(endNode) {
        if(!endNode->parent) {
            delete endNode;
            break;
        }
        endNode = endNode->parent;
        if(endNode->left) {
            delete endNode->left;
        }
        if(endNode->right) {
            delete endNode->right;
        }
    }
}

BranchAndBound::BranchAndBound(QObject *parent) : QObject(parent) {
    connect(&m_Pool, &StaticThreadPool::taskFinished, this, &BranchAndBound::handleBB);
}

void BranchAndBound::start() {
    node_t *node = new node_t;

    for(int i = 0; i < m_Matrix.count(); ++i) {
        m_Matrix[i][i] = MAX_VALUE;
    }
    double topBound = BBTask::findSimpleWay(m_Matrix);
    node->weight= BBTask::reduceMatrix(m_Matrix);
    node->matrix = m_Matrix;
    node->isInPath = true;
    BBTask *task = createBBTask<BranchTask>(node, topBound, m_Matrix);
    m_Pool.putTask(task);
}

void BranchAndBound::setGraphMatrix(const GraphMatrix &matrix) {
    m_Matrix = matrix;
    for(int i = 0; i < m_Matrix.count(); ++i) {
        m_Matrix[i][i] = MAX_VALUE;
    }
    start();
}

void BranchAndBound::setPenaltyMatrix(const GraphMatrix &penaltyMatrix) {
    m_PenaltyMatrix = penaltyMatrix;
}

void BranchAndBound::handleBB(node_t *node) {
    if(node == nullptr) {
        findOptimalPath();
    } else {
        m_Results.append(node);
    }

}

void BranchAndBound::findOptimalPath() {
    node_t* minWeight = *std::min_element(m_Results.begin(), m_Results.end(), [](node_t *lhs, node_t *rhs) {
        return lhs->weight < rhs->weight;
    });
    qDebug() << "MIN" << minWeight->weight;

}
