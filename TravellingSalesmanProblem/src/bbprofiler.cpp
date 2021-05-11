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

Profiler::Profiler(QObject *parent) : QObject(parent) {
    connect(&m_BB, &BranchAndBound::finished, this, &Profiler::handleBbTest);
    connect(&m_Ga, &GeneticAlgorithm::pathReady, this, &Profiler::handleGaTest);
    connect(&m_Cuda, &GACudaWrapper::pathReady, this, &Profiler::handleCudaTest);
    m_Ga.setGenerations(2000);
    m_Ga.setPopulationSize(320);
}

void Profiler::startBb() {
    if(m_Tests.empty()) {
        for(size_t i = 1; i < 2; ++i) {
            GraphMatrix matrix;
            fillRandomMatrix(i * 400, matrix);
            m_Tests.append(matrix);
        }
    }

    runBbTest();
}

void Profiler::stopBb() {
    m_Tests.clear();
}

void Profiler::startGa() {
    if(m_Tests.empty()) {
        for(size_t i = 1; i < 2; ++i) {
            GraphMatrix matrix;
            fillRandomMatrix(i * 400, matrix);
            m_Tests.append(matrix);
        }
    }

    runGaTest();
}

void Profiler::stopGa() {
    m_Tests.clear();
}

void Profiler::startCuda() {
    if(m_Tests.empty()) {
        for(size_t i = 1; i < 2; ++i) {
            GraphMatrix matrix;
            fillRandomMatrix(i * 400, matrix);
            m_Tests.append(matrix);
        }
    }

    runCudaTest();
}

void Profiler::stopCuda() {
    m_Tests.clear();
}

void Profiler::handleBbTest(double distance) {
    m_Measurements.last().endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = m_Measurements.last().endTime - m_Measurements.last().startTime;
    double time = fp_ms.count();
    emit bbTestFinished(m_Measurements.last().verticesCount, time, distance);
    m_Index++;
    runBbTest();
}

void Profiler::handleGaTest(double distance) {
    m_Measurements.last().endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = m_Measurements.last().endTime - m_Measurements.last().startTime;
    double time = fp_ms.count();
    emit gaTestFinished(m_Measurements.last().verticesCount, time, distance);
    m_Index++;
    runGaTest();
}

void Profiler::handleCudaTest(double distance) {
    m_Measurements.last().endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = m_Measurements.last().endTime - m_Measurements.last().startTime;
    double time = fp_ms.count();
    emit cudaTestFinished(m_Measurements.last().verticesCount, time, distance);
    m_Index++;
    runCudaTest();
}


void Profiler::fillRandomMatrix(size_t size, GraphMatrix& matrix) {
     std::uniform_real_distribution<double> dist(1.0, 200.0);
    for(size_t i = 0; i < size; ++i) {
        QList<double> row;
        for(size_t j = 0; j < size; ++j) {
            row.append(dist(*QRandomGenerator::global()));
        }
        matrix.append(row);
    }
}

void Profiler::runGaTest() {
    if(m_Index < m_Tests.count()) {
        measure_t measure;
        measure.startTime = std::chrono::high_resolution_clock::now();
        measure.verticesCount = m_Tests.at(m_Index).count();
        m_Measurements.append(measure);
        m_Ga.start(m_Tests.at(m_Index));
    } else {
        qDebug() << "TESTS ENDED";
        m_Index = 0;
        m_Measurements.clear();
        emit profilerFinished();
    }
}

void Profiler::runCudaTest() {
    if(m_Index < m_Tests.count()) {
        measure_t measure;
        measure.startTime = std::chrono::high_resolution_clock::now();
        measure.verticesCount = m_Tests.at(m_Index).count();
        m_Measurements.append(measure);
        qDebug() << m_Index << m_Tests.count();
        m_Cuda.start(m_Tests.at(m_Index));
    } else {
        qDebug() << "TESTS ENDED";
        m_Index = 0;
        m_Measurements.clear();
        emit profilerFinished();
    }
}

void Profiler::runBbTest() {
    if(m_Index < m_Tests.count()) {
        measure_t measure;
        measure.startTime = std::chrono::high_resolution_clock::now();
        measure.verticesCount = m_Tests.at(m_Index).count();
        m_Measurements.append(measure);
        m_BB.start(m_Tests.at(m_Index));
    } else {
        qDebug() << "TESTS ENDED";
        m_Index = 0;
        m_Measurements.clear();
        emit profilerFinished();
    }
}
