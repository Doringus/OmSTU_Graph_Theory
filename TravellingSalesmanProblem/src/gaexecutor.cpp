#include "gaexecutor.h"
#include "ga.h"

GAExecutor::GAExecutor(QObject *parent) : IExecutor(parent) {

}

void GAExecutor::putTask(Task *task) {
    GATask *ga = qobject_cast<GATask*>(task);
    connect(ga, &GATask::finished, this, &GAExecutor::handleTask, Qt::BlockingQueuedConnection);
    m_Pool->putTask(task);
}

void GAExecutor::handleTask(QList<ga::individual_t> children) {
    emit taskFinished(children);
}

