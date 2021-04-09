#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QPromise>
#include <QFutureSynchronizer>

#include "drawingalgorithm.h"
#include "staticthreadpool.h"

class Node {
public:
    Node();
    Node(const Node& other);
    ~Node();

    QList<QPair<int, int>> getIncludedEdges();
    void removeLoops();


    Node *parent, *left, *right, *brother;
    GraphMatrix matrix;
    QList<QPair<int, int>> includedEdges;
    QList<QList<QPair<int, int>>> edges;
    QList<QPair<int, int>> excludedEdges;
    QList<int> visitedVertices;
    double weight;
    bool isInPath;
};

struct node_t {
    ~node_t() {
        if(left != nullptr) {
            delete left;
            left = nullptr;
        }
        if(right != nullptr) {
            delete right;
            right = nullptr;
        }
    };
    node_t(){};
    node_t(const node_t& other) : parent(other.parent),
    left(other.left), right(other.right), brother(other.brother),
    matrix(other.matrix), includedEdges(other.includedEdges),
    excludedEdges(other.excludedEdges), visitedVertices(other.visitedVertices),
    weight(other.weight), isInPath(other.isInPath) {}

    node_t *parent = nullptr;
    node_t *left = nullptr, *right = nullptr;
    node_t* brother = nullptr;
    GraphMatrix matrix;
    QList<QPair<int, int>> includedEdges;
    QList<QList<QPair<int, int>>> edges;
    QList<QPair<int, int>> excludedEdges;
    QList<int> visitedVertices;
    double weight;
    bool isInPath = false;
};
Q_DECLARE_METATYPE(node_t)
Q_DECLARE_TYPEINFO(node_t, Q_RELOCATABLE_TYPE);

struct penalty_t {
    QPair<int, int> vertices;
    double weight;
};

template<typename Task, typename ...Params>
Task* createBBTask(Params ...args) {
    Task *task = new Task(args...);
    task->moveToThread(nullptr);
    return task;
}

class BBTask : public QObject {
    Q_OBJECT
public:
    explicit BBTask(node_t *rootNode, double topBound, const GraphMatrix& matrix, QObject *parent = nullptr) : QObject(parent),
                                   m_TopBound(topBound), m_Matrix(matrix){ m_CurrentNode = rootNode;}
    virtual void run() = 0;

    double getTopBound() const { return m_TopBound;}
    node_t* getCurrentNode() const {return m_CurrentNode;}
    GraphMatrix getMatrix() const {return m_Matrix;}
public:
    void iterate();
    void createNextBranch(const QPair<int, int>& edge);
    static double findSimpleWay(const GraphMatrix& matrix);
    static double reduceMatrix(GraphMatrix& matrix);
    static QList<double> getMinByRows(const GraphMatrix& matrix);
    static QList<double> getMinByColumns(const GraphMatrix& matrix);
    QList<penalty_t> getPathWithMaxPenalty(GraphMatrix &matrix) const;
    double getMinByRowExcept(GraphMatrix& matrix, int row, int exceptionIndex) const;
    double getMinByColumnExcept(GraphMatrix& matrix, int col, int exceptionIndex) const;
    void createLeftNode(node_t *leftNode, const QPair<int, int> &edge);
    void createRightNode(node_t *rightNode, const QPair<int, int> &edge);
    bool checkLoop(const QList<QPair<int,int>>& edges);
    void removeLoop(node_t *node);
    node_t* createNode(node_t *parent);
    node_t* createBroter(node_t *brother);
signals:
    void finished(node_t *endNode);
    void subtaskCreated(BBTask *task);
protected:
    double m_TopBound;
    node_t *m_CurrentNode;
    GraphMatrix m_Matrix;
};

class BranchTask : public BBTask {
    Q_OBJECT
public:
    explicit BranchTask(node_t *rootNode, double topBound, const GraphMatrix& matrix, QObject *parent = nullptr) :
                                BBTask(rootNode, topBound, matrix, parent) {}
    void run() override;
};

class ParallelPenaltyTask : public BBTask {
    Q_OBJECT
public:
    explicit ParallelPenaltyTask(node_t *rootNode, double topBound, const GraphMatrix& matrix, QPair<int, int> edge, QObject *parent = nullptr);
    void run() override;
private:
    QPair<int, int> m_Edge;
};

class BranchAndBound : public QObject {
    Q_OBJECT
public:
    explicit BranchAndBound(QObject *parent = nullptr);
    void start(const GraphMatrix &matrix);
    void setPenaltyMatrix(const GraphMatrix& penaltyMatrix);
    void findOptimalPath();
signals:
    void bbFinished(node_t *endNode, node_t *rootNode);
    void lastTaskFinished();
    void finished();
private slots:
    void handleBB(node_t *node);
private:
    void deleteOldTree();
private:
    GraphMatrix m_Matrix, m_PenaltyMatrix;
    StaticThreadPool m_Pool;
    QList<node_t*> m_Results;
    node_t *m_RootNode;
};
