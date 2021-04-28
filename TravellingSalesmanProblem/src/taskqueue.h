#pragma once

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

class BBTask;

class Task : public QObject {
    Q_OBJECT
public:
    Task(QObject *parent) : QObject(parent){}
    virtual void run() = 0;
};

class TaskQueue : public QObject {
    Q_OBJECT
public:
    explicit TaskQueue(QObject *parent = nullptr);

    void put(Task *task);

    Task* take();
private:
    Task* takeLocked();
private:
    QQueue<Task*> m_Buffer;
    QMutex m_Mutex;
    QWaitCondition m_Wc;
};

