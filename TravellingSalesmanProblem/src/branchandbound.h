#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QPromise>
#include <QFutureSynchronizer>

#include "drawingalgorithm.h"

struct node_t {
    node_t *parent = nullptr;
    node_t *left = nullptr, *right = nullptr;
    QList<node_t*> brothers;
    GraphMatrix matrix;
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

class BranchAndBound : public QObject {
    Q_OBJECT
public:
    explicit BranchAndBound(QObject *parent = nullptr);
    void start(GraphMatrix &matrix);


    void setMatrix(const GraphMatrix& matrix);
public slots:
     void start();
private:
    node_t *branchAndBound(node_t *root);
    void iterate();
    void iterate(node_t *root);
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
    void deleteTree(node_t *endNode);
signals:
    void bbFinished(node_t *endNode, node_t *rootNode);
    void bbSubtreeCreated(BranchAndBound *bb);
private:
    double m_LowBound, m_TopBound;
    node_t *m_CurrentNode, *m_RootNode;
    QFutureSynchronizer<node_t*> m_Synchronizer;
    GraphMatrix m_Matrix;
};

