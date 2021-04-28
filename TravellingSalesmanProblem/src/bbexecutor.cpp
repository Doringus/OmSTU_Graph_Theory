#include "bbexecutor.h"

#include "branchandbound.h"

BBExecutor::BBExecutor(QObject *parent) : IExecutor(parent), m_TaskCount(0) {

}

void BBExecutor::putTask(Task *task) {
    BBTask *bbTask = qobject_cast<BBTask*>(task);
    connect(bbTask, &BBTask::finished, this, &BBExecutor::handleBB, Qt::BlockingQueuedConnection);
    connect(bbTask, &BBTask::subtaskCreated, this, &BBExecutor::putTask, Qt::BlockingQueuedConnection);
    m_TaskCount++;
    m_Pool->putTask(std::shared_ptr<BBTask>(bbTask));
}

void BBExecutor::handleBB(Node *endNode) {
    m_TaskCount--;
    emit taskFinished(endNode);
    if(m_TaskCount == 0) {
        /// Let bb know that we finished all tasks
        emit taskFinished(nullptr);
    }
}
