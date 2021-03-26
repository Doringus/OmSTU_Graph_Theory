#pragma once

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

class BBTask;

class TaskQueue : public QObject {
    Q_OBJECT
public:
    explicit TaskQueue(QObject *parent = nullptr);

    void put(BBTask *task);

    BBTask* take();

signals:
private:
    BBTask* takeLocked();
private:
    QQueue<BBTask*> m_Buffer;
    QMutex m_Mutex;
    QWaitCondition m_Wc;
};

