#include "taskqueue.h"

#include <QMutexLocker>

TaskQueue::TaskQueue(QObject *parent) : QObject(parent) {

}

void TaskQueue::put(BranchAndBound *task) {
    QMutexLocker locker(&m_Mutex);
    m_Buffer.push_back(task);
    m_Wc.wakeOne();
}

BranchAndBound *TaskQueue::take() {
    m_Mutex.lock();
    while(m_Buffer.empty()) {
        m_Wc.wait(&m_Mutex);
    }
    m_Mutex.unlock();
    return takeLocked();
}

BranchAndBound *TaskQueue::takeLocked() {
    BranchAndBound *front = m_Buffer.front();
    m_Buffer.pop_front();
    return front;
}
