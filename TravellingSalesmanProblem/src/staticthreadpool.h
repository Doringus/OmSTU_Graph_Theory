#pragma once

#include <QObject>
#include <QThread>

#include "taskqueue.h"

struct node_t;

class StaticThreadPool : public QObject {
    Q_OBJECT
public:
    explicit StaticThreadPool(QObject *parent = nullptr);
public slots:
    void putTask(BranchAndBound *task);
private:
    void workerRoutine();
private slots:
    void handleBB(node_t *endNode, node_t *rootNode);
private:
    QList<QThread*> m_Threads;
    TaskQueue m_TaskQueue;
};

