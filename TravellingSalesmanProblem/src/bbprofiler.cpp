#include "bbprofiler.h"

#include <QList>
#include <QDebug>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <random>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <psapi.h>

#include "branchandbound.h"

BBProfiler::BBProfiler(QObject *parent) : QObject(parent) {
    connect(&m_BB, &BranchAndBound::finished, this, &BBProfiler::handleTest);
    connect(&m_BB, &BranchAndBound::lastTaskFinished, this, &BBProfiler::handleMemory,Qt::DirectConnection);
}

void BBProfiler::start() {
    for(size_t i = 1; i < 20; ++i) {
        GraphMatrix matrix;
        fillRandomMatrix(120, matrix);
        m_Tests.append(matrix);
    }
    runTest();
}

void BBProfiler::stop() {
    m_Tests.clear();
}

void BBProfiler::handleTest() {
    m_Measurements.last().endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = m_Measurements.last().endTime - m_Measurements.last().startTime;
    double time = fp_ms.count();
    double memory = (m_Measurements.last().endMem - m_Measurements.last().startMem) / 1024;
    emit testFinished(m_Measurements.last().verticesCount, time, memory);
    runTest();
}

void BBProfiler::handleMemory() {
    PROCESS_MEMORY_COUNTERS memCounter;
    BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
                                       &memCounter,
                                       sizeof(memCounter));
    m_Measurements.last().endMem = memCounter.PeakWorkingSetSize;
}

void BBProfiler::fillRandomMatrix(size_t size, GraphMatrix& matrix) {
     std::uniform_real_distribution<double> dist(1.0, 200.0);
    for(size_t i = 0; i < size; ++i) {
        QList<double> row;
        for(size_t j = 0; j < size; ++j) {
            row.append(dist(*QRandomGenerator::global()));
        }
        matrix.append(row);
    }
}

void BBProfiler::runTest() {
    if(!m_Tests.empty()) {
        measure_t measure;
        measure.startTime = std::chrono::high_resolution_clock::now();
        PROCESS_MEMORY_COUNTERS memCounter;
        BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
                                           &memCounter,
                                           sizeof(memCounter));
        measure.startMem = memCounter.WorkingSetSize;
        measure.verticesCount = m_Tests.front().count();
        m_Measurements.append(measure);
        m_BB.start(m_Tests.front());
        m_Tests.pop_front();
    } else {
        qDebug() << "TESTS ENDED";
        emit profilerFinished();
    }

}
