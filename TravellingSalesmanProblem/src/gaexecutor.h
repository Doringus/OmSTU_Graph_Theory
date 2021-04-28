#pragma once

#include <QObject>

#include "staticthreadpool.h"
#include "gafunctions.h"

class GAExecutor : public IExecutor {
    Q_OBJECT
public:
    GAExecutor(QObject *parent = nullptr);
    virtual void putTask(Task *task) override;
signals:
    void taskFinished(QList<ga::individual_t> children);
private slots:
    void handleTask(QList<ga::individual_t> children);
};

