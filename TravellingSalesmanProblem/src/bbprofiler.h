#pragma once

#include <QObject>
#include <QQueue>
#include <chrono>

#include "branchandbound.h"

using GraphMatrix = QList<QList<double>>;

struct measure_t {
    int verticesCount;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
    size_t startMem, endMem;
};

class BBProfiler : public QObject {
    Q_OBJECT
public:
    explicit BBProfiler(QObject *parent = nullptr);

    Q_INVOKABLE void start();
signals:
private slots:
    void handleTest();
    void handleMemory();
private:
    void fillRandomMatrix(size_t size, GraphMatrix &matrix);
    void runTest();
private:
    QQueue<GraphMatrix> m_Tests;
    QList<measure_t> m_Measurements;
    BranchAndBound m_BB;
};
