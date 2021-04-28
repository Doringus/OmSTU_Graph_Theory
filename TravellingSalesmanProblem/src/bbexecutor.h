#pragma once

#include <QObject>

#include "staticthreadpool.h"

class BBExecutor : public IExecutor {
    Q_OBJECT
public:
    BBExecutor(QObject *parent = nullptr);
    virtual void putTask(Task *task) override;
signals:
    void taskFinished(Node *endNode);
private slots:
    void handleBB(Node *endNode);
private:
    int m_TaskCount;
};

