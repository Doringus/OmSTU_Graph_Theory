#include "staticthreadpool.h"

#include <QDebug>

#include "branchandbound.h"

StaticThreadPool::StaticThreadPool(QObject *parent) : QObject(parent) {
    for(int i = 0; i < 4; ++i) {
        QThread *thread = QThread::create([this](){
            workerRoutine();
         });
        thread->start();
    }
}

void StaticThreadPool::putTask(BranchAndBound *task) {
    // We need to remove object thread affinity
    // so it can be pulled to worker thread
    connect(task, &BranchAndBound::bbFinished, this, &StaticThreadPool::handleBB, Qt::QueuedConnection);
    connect(task, &BranchAndBound::bbSubtreeCreated, this, &StaticThreadPool::putTask, Qt::QueuedConnection);
    task->moveToThread(nullptr);
    m_TaskQueue.put(task);
}

void StaticThreadPool::workerRoutine() {
    while(true) {
        auto task = m_TaskQueue.take();
        task->moveToThread(QThread::currentThread());
        task->start();
    }
}

void StaticThreadPool::handleBB(node_t *endNode, node_t *rootNode) {
    qDebug() << endNode->weight << QThread::currentThreadId();
}
