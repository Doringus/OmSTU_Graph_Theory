#include "taskqueue.h"

#include <QMutexLocker>

TaskQueue::TaskQueue(QObject *parent) : QObject(parent) {

}

void TaskQueue::put(Task *task) {
    QMutexLocker locker(&m_Mutex);
    m_Buffer.push_back(task);
    m_Wc.wakeOne();
}

Task* TaskQueue::take() {
    QMutexLocker locker(&m_Mutex);
    while(m_Buffer.empty()) {
        m_Wc.wait(&m_Mutex);
    }
    return takeLocked();
}

Task* TaskQueue::takeLocked() {
    Task *front = m_Buffer.front();
    m_Buffer.pop_front();
    return front;
}
