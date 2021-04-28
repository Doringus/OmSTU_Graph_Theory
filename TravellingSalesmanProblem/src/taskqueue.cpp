#include "taskqueue.h"

#include <QMutexLocker>

TaskQueue::TaskQueue(QObject *parent) : QObject(parent) {

}

void TaskQueue::put(std::shared_ptr<Task> task) {
    QMutexLocker locker(&m_Mutex);
    m_Buffer.push_back(std::move(task));
    m_Wc.wakeOne();
}

std::shared_ptr<Task> TaskQueue::take() {
    QMutexLocker locker(&m_Mutex);
    while(m_Buffer.empty()) {
        m_Wc.wait(&m_Mutex);
    }
    return takeLocked();
}

std::shared_ptr<Task> TaskQueue::takeLocked() {
    auto front = std::move(m_Buffer.front());
    m_Buffer.pop_front();
    return front;
}
