#pragma once

#include <QObject>

#include "drawingalgorithm.h"

struct node_t {
    node_t *parent;
    node_t *left = nullptr, *right = nullptr;
    GraphMatrix matrix;
    QPair<int, int> edge;
    QList<QPair<int, int>> includedEdges;
    QList<QPair<int, int>> excludedEdges;
    QList<int> visitedVertices;
    double weight;
    bool isInPath = false;
};
Q_DECLARE_METATYPE(node_t)


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
    bool checkLoop(const QList<QPair<int,int>>& edges);
    void removeLoop(node_t *node);
signals:
    void bbFinished(node_t *endNode, node_t *rootNode);
private:
    double m_LowBound, m_TopBound;
    node_t *m_CurrentNode, *m_RootNode;
};

