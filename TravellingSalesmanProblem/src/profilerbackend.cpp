#include "profilerbackend.h"

ProfilerBackend::ProfilerBackend(QObject *parent) : QObject(parent), m_BbProfilerTableModel(new TableModel(this)),
                                                    m_GaProfilerTableModel(new TableModel(this)), m_CudaProfilerTableModel(new TableModel(this)),
                                                    m_IsRunning(false) {
    connect(&m_Profiler, &Profiler::bbTestFinished, this, &ProfilerBackend::handleBbTest);
    connect(&m_Profiler, &Profiler::gaTestFinished, this, &ProfilerBackend::handleGaTest);
    connect(&m_Profiler, &Profiler::cudaTestFinished, this, &ProfilerBackend::handleCudaTest);
    connect(&m_Profiler, &Profiler::profilerFinished, this, [&](){
       setIsRunning(false);
    });
}

void ProfilerBackend::setIsRunning(bool running) {
    m_IsRunning = running;
    emit isRunningChanged();
}

QAbstractTableModel *ProfilerBackend::getGaProfilerTable() const {
    return m_GaProfilerTableModel;
}

QAbstractTableModel *ProfilerBackend::getCudaProfilerTable() const {
    return m_CudaProfilerTableModel;
}

QAbstractTableModel *ProfilerBackend::getBbProfilerTable() const {
    return m_BbProfilerTableModel;
}

bool ProfilerBackend::isRunning() const {
    return m_IsRunning;
}

void ProfilerBackend::startBbProfiler() {
    setIsRunning(true);
    m_BbProfilerTableModel->clearTable();
    m_Profiler.startBb();
}

void ProfilerBackend::stopBbProfiler() {
    m_Profiler.stopBb();
}

void ProfilerBackend::startGaProfiler() {
    setIsRunning(true);
    m_GaProfilerTableModel->clearTable();
    m_Profiler.startGa();
}

void ProfilerBackend::stopGaProfiler() {
    m_Profiler.stopGa();
}

void ProfilerBackend::startCudaProfiler() {
    setIsRunning(true);
    m_CudaProfilerTableModel->clearTable();
    m_Profiler.startCuda();
}

void ProfilerBackend::stopCudaProfiler() {
    m_Profiler.stopCuda();
}

void ProfilerBackend::handleGaTest(int verticesCount, double time, double distance) {
    m_GaProfilerTableModel->insertRow({static_cast<double>(verticesCount), time, distance});
}

void ProfilerBackend::handleCudaTest(int verticesCount, double time, double distance) {
    m_CudaProfilerTableModel->insertRow({static_cast<double>(verticesCount), time, distance});
}

void ProfilerBackend::handleBbTest(int verticesCount, double time, double distance) {
    m_BbProfilerTableModel->insertRow({static_cast<double>(verticesCount), time, distance});
}
