#pragma once

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

class BranchAndBound;

class TaskQueue : public QObject {
    Q_OBJECT
public:
    explicit TaskQueue(QObject *parent = nullptr);

    void put(BranchAndBound *task);

    BranchAndBound *take();

signals:
private:
    BranchAndBound *takeLocked();
private:
    QQueue<BranchAndBound*> m_Buffer;
    QMutex m_Mutex;
    QWaitCondition m_Wc;
};

