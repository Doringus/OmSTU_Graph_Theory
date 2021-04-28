#pragma once

#include <QObject>
#include <QThread>

#include <memory>

#include "taskqueue.h"

class Node;

class StaticThreadPool : public QObject {
    Q_OBJECT
public:
    explicit StaticThreadPool(QObject *parent = nullptr);
    ~StaticThreadPool();
public slots:
    void putTask(std::shared_ptr<Task> task);
private:
    void workerRoutine();
private:
    QList<QThread*> m_Threads;
    TaskQueue m_TaskQueue;
    int m_TaskCount = 0;
};

class IExecutor : public QObject {
    Q_OBJECT
public:
    IExecutor(QObject *parent = nullptr) : QObject(parent) {}
    virtual void putTask(Task *task) = 0;
    void setThreadPool(StaticThreadPool* pool) {
        m_Pool = pool;
    }
protected:
    StaticThreadPool *m_Pool;
};
