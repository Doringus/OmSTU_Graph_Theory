#pragma once

#include <QObject>
#include <QQueue>
#include <chrono>

#include "branchandbound.h"
#include "ga.h"

using GraphMatrix = QList<QList<double>>;

struct measure_t {
    int verticesCount;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
    size_t startMem, endMem;
};

class Profiler : public QObject {
    Q_OBJECT
public:
    explicit Profiler(QObject *parent = nullptr);

    Q_INVOKABLE void startBb();
    Q_INVOKABLE void stopBb();

    Q_INVOKABLE void startGa();
    Q_INVOKABLE void stopGa();

    Q_INVOKABLE void startCuda();
    Q_INVOKABLE void stopCuda();
signals:
    void bbTestFinished(int n, double time, double distance);
    void gaTestFinished(int n, double time, double distance);
    void cudaTestFinished(int n, double time, double distance);
    void profilerFinished();
private slots:
    void handleBbTest(double distance);
    void handleGaTest(double distance);
    void handleCudaTest(double distance);
private:
    void fillRandomMatrix(size_t size, GraphMatrix &matrix);
    void runBbTest();
    void runGaTest();
    void runCudaTest();
private:
    QQueue<GraphMatrix> m_Tests;
    QList<measure_t> m_Measurements;
    BranchAndBound m_BB;
    GeneticAlgorithm m_Ga;
    GACudaWrapper m_Cuda;
    int m_Index = 0;
};
