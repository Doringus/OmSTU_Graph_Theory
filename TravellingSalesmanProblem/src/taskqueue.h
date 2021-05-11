#pragma once

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QDebug>

class Task : public QObject {
    Q_OBJECT
public:
    Task(QObject *parent) : QObject(parent){}
    virtual ~Task() {}
    virtual void run() = 0;
};

class TaskQueue : public QObject {
    Q_OBJECT
public:
    explicit TaskQueue(QObject *parent = nullptr);

    void put(std::shared_ptr<Task> task);

    std::shared_ptr<Task> take();
private:
    std::shared_ptr<Task> takeLocked();
private:
    QQueue<std::shared_ptr<Task>> m_Buffer;
    QMutex m_Mutex;
    QWaitCondition m_Wc;
};

