#include "staticthreadpool.h"

#include <QDebug>

#include "branchandbound.h"

StaticThreadPool::StaticThreadPool(QObject *parent) : QObject(parent) {
    for(int i = 0; i < 8; ++i) {
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

void StaticThreadPool::putTask(Task *task) {
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
