#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QPromise>
#include <QFutureSynchronizer>

#include "ialgorithm.h"
#include "staticthreadpool.h"
#include "bbexecutor.h"

class Node {
public:
    Node();
    Node(const Node& other);
    ~Node();

    static Node* createChild(Node *parent);
    static Node* createBrother(Node *brother);

    QList<QPair<int, int>> getIncludedEdges();
    void removeLoops();
    void includeEdge(const QPair<int, int>& edge);
    void excludeEdge(const QPair<int, int>& edge);

    void deleteLeftChild();
    void deleleRightChild();

    bool hasEdges();
    void clearUnusedData();

    /*****************************************/
    Node* getLeftChild() const;
    Node* getRightChild() const;
    Node* getParent() const;
    Node* getBrother() const;

    /* For std::swap */
    Node*& getLeftChildRef();
    Node*& getRightChildRef();
    Node*& getParentRef();

    void setLeftChild(Node* node);
    void setRightChild(Node* node);
    void setParent(Node* node);
    void setBrother(Node* node);

    GraphMatrix getMatrix() const;
    GraphMatrix& getMatrixRef();
    void setMatrix(const GraphMatrix& matrix);

    double getWeight() const;
    void setWeight(double weight);

    bool isInPath() const;
    void setIsInPath(bool path);

    QPair<int, int> getLastIncludedEdge() const;
    QPair<int, int> getLastExcludedEdge() const;

    int getVisitedVerticesCount() const;
    void setVisitedVerticesCount(int count);
private:
    Node *m_Parent, *m_Left, *m_Right, *m_Brother;
    GraphMatrix m_Matrix;
    QList<QList<QPair<int, int>>> m_Edges;
    QList<QPair<int, int>> m_ExcludedEdges;
    QPair<int, int> m_LastIncludedEdge, m_LastExcludedEdge;
    int m_VisitedVerticesCount;
    double m_Weight;
    bool m_IsInPath;
};
Q_DECLARE_METATYPE(Node)
Q_DECLARE_TYPEINFO(Node, Q_RELOCATABLE_TYPE);

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

class BBTask : public Task {
    Q_OBJECT
public:
    explicit BBTask(Node *rootNode, double topBound, int totalVerticesCount, QObject *parent = nullptr) : Task(parent),
                                   m_TopBound(topBound), m_TotalVerticesCount(totalVerticesCount){ m_CurrentNode = rootNode;}
    virtual void run() override = 0;

    double getTopBound() const { return m_TopBound;}
    Node* getCurrentNode() const {return m_CurrentNode;}
    int getTotalVerticesCount() const {return m_TotalVerticesCount;}
public:
    void iterate();
    void createNextBranch(const QPair<int, int>& edge);
signals:
    void finished(Node *endNode);
    void subtaskCreated(BBTask *task);
protected:
    double m_TopBound;
    Node *m_CurrentNode;
    int m_TotalVerticesCount;
};

class BranchTask : public BBTask {
    Q_OBJECT
public:
    explicit BranchTask(Node *rootNode, double topBound, int totalVerticesCount, QObject *parent = nullptr) :
                                BBTask(rootNode, topBound, totalVerticesCount, parent) {}
    void run() override;
};

class ParallelPenaltyTask : public BBTask {
    Q_OBJECT
public:
    explicit ParallelPenaltyTask(Node *rootNode, double topBound, int totalVerticesCount, QPair<int, int> edge, QObject *parent = nullptr);
    void run() override;
private:
    QPair<int, int> m_Edge;
};

class BranchAndBound : public IAlgorithm {
    Q_OBJECT
public:
    explicit BranchAndBound(QObject *parent = nullptr);
    Q_INVOKABLE virtual void start(const GraphMatrix &matrix) override;
    void findOptimalPath();
signals:
    void bbFinished(Node *endNode, Node *rootNode);

    void lastTaskFinished();
    void finished(double distance);
private slots:
    void handleBB(Node *node);
private:
    void deleteOldTree();
private:
    GraphMatrix m_Matrix;
    BBExecutor m_Executor;
    StaticThreadPool m_Pool;
    QList<Node*> m_Results;
    Node *m_RootNode;
};

namespace bb {
    double findSimpleWay(const GraphMatrix& matrix);
    double reduceMatrix(GraphMatrix& matrix);
    QList<double> getMinByRows(const GraphMatrix& matrix);
    QList<double> getMinByColumns(const GraphMatrix& matrix);
    QList<penalty_t> getPathWithMaxPenalty(GraphMatrix &matrix);
    double getMinByRowExcept(GraphMatrix& matrix, int row, int exceptionIndex);
    double getMinByColumnExcept(GraphMatrix& matrix, int col, int exceptionIndex);
    double getMinByRowExcept(QList<double> row, int exceptionIndex);
    void createLeftNode(Node *leftNode, const QPair<int, int> &edge);
    void createRightNode(Node *rightNode, const QPair<int, int> &edge);
}
