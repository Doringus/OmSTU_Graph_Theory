#pragma once

#include <QObject>

#include "drawingalgorithm.h"

struct node_t {
    node_t *parent;
    node_t *left, *right;
    GraphMatrix matrix;
    QPair<int, int> edge;
    QList<QPair<int, int>> includedEdges;
    QList<QPair<int, int>> excludedEdges;
    QList<int> visitedVertices;
    double weight;
};

struct penalty_t {
    QPair<int, int> vertices;
    double weight;
};


namespace branchAndBound {
    void createLeftNode(node_t *leftNode);
    void createRightNode(node_t *rightNode);

    void splitNode();

    double findSimpleWay(const GraphMatrix& matrix);
    double reduceMatrix(GraphMatrix& matrix);
    QList<double> getMinByRows(const GraphMatrix& matrix);
    QList<double> getMinByColumns(const GraphMatrix& matrix);
    QList<QPair<int, int>> getPathWithMaxPenalty(const GraphMatrix& matrix);
}

class BranchAndBound : public QObject {
    Q_OBJECT
public:
    explicit BranchAndBound(QObject *parent = nullptr);
    void start(GraphMatrix &matrix);
private:
    void iterate();
    double findSimpleWay(const GraphMatrix& matrix) const;
    double reduceMatrix(GraphMatrix& matrix) const;
    QList<double> getMinByRows(const GraphMatrix& matrix) const;
    QList<double> getMinByColumns(const GraphMatrix& matrix) const;
    QList<penalty_t> getPathWithMaxPenalty(GraphMatrix &matrix) const;
    double getMinByRowExcept(GraphMatrix& matrix, int row, int exceptionIndex) const;
    double getMinByColumnExcept(GraphMatrix& matrix, int col, int exceptionIndex) const;
    void createLeftNode(node_t *leftNode, const QPair<int, int> &edge);
    void createRightNode(node_t *rightNode, const QPair<int, int> &edge);
    bool checkLoop(node_t *node, int index);
signals:
private:
    double m_LowBound, m_TopBound;
    node_t *m_CurrentNode, *m_RootNode;
};

