#pragma once

#include <QObject>
#include <QThread>

#include "taskqueue.h"

class Node;

class StaticThreadPool : public QObject {
    Q_OBJECT
public:
    explicit StaticThreadPool(QObject *parent = nullptr);
    ~StaticThreadPool();
public slots:
    void putTask(BBTask *task);
signals:
    void taskFinished(Node *endNode);
private:
    void workerRoutine();
private slots:
    void handleBB(Node *endNode);
private:
    QList<QThread*> m_Threads;
    TaskQueue m_TaskQueue;
    int m_TaskCount = 0;
};

