#include "branchandbound.h"

#include <QDebug>
#include <iostream>

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

BranchAndBound::BranchAndBound(QObject *parent) : QObject(parent) {
    m_RootNode = new node_t;
    m_CurrentNode = m_RootNode;
}

void BranchAndBound::start(GraphMatrix &matrix) {
    for(int i = 0; i < matrix.count(); ++i) {
        matrix[i][i] = MAX_VALUE;
    }
    m_TopBound = findSimpleWay(matrix);
    m_LowBound = reduceMatrix(matrix);
    qDebug() << m_TopBound << m_LowBound;
    auto penalties = getPathWithMaxPenalty(matrix);
    if(penalties.count() > 1) {
        /* Create thread here for another branches */
    }
    m_RootNode->matrix = matrix;
    m_RootNode->weight = m_LowBound;
    printMatrix(matrix);
    for(int i = 0; i < 5; ++i) {
        iterate();
    }
}

void BranchAndBound::iterate() {
    auto penalties = getPathWithMaxPenalty(m_CurrentNode->matrix);
    if(penalties.count() > 1) {
        for(int i = 0; i < penalties.count(); ++i) {
            if(!checkLoop(m_CurrentNode, penalties.at(i).vertices.second)) {
                penalties.removeAt(i);
            }
        }
    }
    node_t *leftNode = createNode(m_CurrentNode);
    node_t *rightNode = createNode(m_CurrentNode);
    createLeftNode(leftNode, penalties.first().vertices);
    createRightNode(rightNode, penalties.first().vertices);
    if(rightNode->weight < rightNode->weight) {
        if(rightNode->weight > m_TopBound) {
            /* End */
        }
        m_CurrentNode = rightNode;
    } else {
        if(leftNode->weight > m_TopBound) {
            /* End */
        }
        m_CurrentNode = leftNode;
    }
    qDebug() << "LEFT" << leftNode->weight << "RIGHT" << rightNode->weight;
    qDebug() << m_CurrentNode->weight << m_CurrentNode->includedEdges;
    printMatrix(m_CurrentNode->matrix);
}

double BranchAndBound::findSimpleWay(const GraphMatrix &matrix) const {
    double weight = 0;
    for(int i = 0; i < matrix.count() - 1; ++i) {
        weight += matrix[i][i + 1];
    }
    return weight;
}

double BranchAndBound::reduceMatrix(GraphMatrix &matrix) const {
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

QList<double> BranchAndBound::getMinByRows(const GraphMatrix &matrix) const {
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

QList<double> BranchAndBound::getMinByColumns(const GraphMatrix &matrix) const {
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

QList<penalty_t> BranchAndBound::getPathWithMaxPenalty(GraphMatrix &matrix) const {
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

double BranchAndBound::getMinByRowExcept(GraphMatrix &matrix, int row, int exceptionIndex) const {
    double value = matrix[row][exceptionIndex];
    matrix[row][exceptionIndex] = MAX_VALUE;
    double min = *std::min_element(matrix.at(row).begin(), matrix.at(row).end());
    matrix[row][exceptionIndex] = value;
    return min;
}

double BranchAndBound::getMinByColumnExcept(GraphMatrix &matrix, int col, int exceptionIndex) const {
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

void BranchAndBound::createLeftNode(node_t *leftNode, const QPair<int, int> &edge) {
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

void BranchAndBound::createRightNode(node_t *rightNode, const QPair<int, int> &edge) {
    rightNode->matrix[edge.first][edge.second] = MAX_VALUE;
    double weight = reduceMatrix(rightNode->matrix);
    rightNode->weight += weight;
    rightNode->excludedEdges.append(edge);
}

bool BranchAndBound::checkLoop(node_t *node, int index) {
    return node->visitedVertices.contains(index);
}
