#include "profilerbackend.h"

ProfilerBackend::ProfilerBackend(QObject *parent) : QObject(parent), m_ProfilerTableModel(new TableModel(this)),
                                                    m_IsRunning(false) {
    connect(&m_Profiler, &BBProfiler::testFinished, this, &ProfilerBackend::handleTest);
    connect(&m_Profiler, &BBProfiler::profilerFinished, this, [&](){
       setIsRunning(false);
    });
}

void ProfilerBackend::setIsRunning(bool running) {
    m_IsRunning = running;
    emit isRunningChanged();
}

QAbstractTableModel *ProfilerBackend::getProfilerTable() const {
    return m_ProfilerTableModel;
}

bool ProfilerBackend::isRunning() const {
    return m_IsRunning;
}

void ProfilerBackend::startProfiler() {
    setIsRunning(true);
    m_ProfilerTableModel->clearTable();
    m_Profiler.start();
}

void ProfilerBackend::stopProfiler() {
    m_Profiler.stop();
}

void ProfilerBackend::handleTest(int verticesCount, double time, double memory) {
    m_ProfilerTableModel->insertRow({static_cast<double>(verticesCount), time, memory});
}
