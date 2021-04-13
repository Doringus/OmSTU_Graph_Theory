#include "staticthreadpool.h"

#include <QDebug>

#include "branchandbound.h"

StaticThreadPool::StaticThreadPool(QObject *parent) : QObject(parent) {
    for(int i = 0; i < 6; ++i) {
        QThread *thread = QThread::create([this](){
            workerRoutine();
         });
        thread->start();
    }
}

StaticThreadPool::~StaticThreadPool() {
    for(auto& thread : m_Threads) {
        thread->quit();
        thread->deleteLater();
    }
}

void StaticThreadPool::putTask(BBTask *task) {
    connect(task, &BBTask::finished, this, &StaticThreadPool::handleBB, Qt::BlockingQueuedConnection);
    connect(task, &BBTask::subtaskCreated, this, &StaticThreadPool::putTask, Qt::BlockingQueuedConnection);
    m_TaskCount++;
    m_TaskQueue.put(task);
}

void StaticThreadPool::workerRoutine() {
    while(true) {
        auto task = m_TaskQueue.take();
        task->moveToThread(QThread::currentThread());
        if(task != nullptr) {
            task->run();
        }
    }
}

void StaticThreadPool::handleBB(Node *endNode) {
    m_TaskCount--;
    emit taskFinished(endNode);
    if(m_TaskCount == 0) {
        emit taskFinished(nullptr);
    }
}
