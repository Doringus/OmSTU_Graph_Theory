#pragma once

#include <QObject>

#include "tablemodel.h"
#include "bbprofiler.h"

class ProfilerBackend : public QObject {
    Q_OBJECT
public:
    explicit ProfilerBackend(QObject *parent = nullptr);

    Q_PROPERTY(QAbstractTableModel* bbProfilerTable READ getBbProfilerTable NOTIFY bbProfilerTableChanged)
    Q_PROPERTY(QAbstractTableModel* gaProfilerTable READ getGaProfilerTable NOTIFY gaProfilerTableChanged)
    Q_PROPERTY(QAbstractTableModel* cudaProfilerTable READ getCudaProfilerTable NOTIFY cudaProfilerTableChanged)
    Q_PROPERTY(bool isRunning READ isRunning WRITE setIsRunning NOTIFY isRunningChanged)

    void setIsRunning(bool running);

    QAbstractTableModel* getBbProfilerTable() const;
    QAbstractTableModel* getGaProfilerTable() const;
    QAbstractTableModel* getCudaProfilerTable() const;
    bool isRunning() const;


    Q_INVOKABLE void startBbProfiler();
    Q_INVOKABLE void stopBbProfiler();

    Q_INVOKABLE void startGaProfiler();
    Q_INVOKABLE void stopGaProfiler();

    Q_INVOKABLE void startCudaProfiler();
    Q_INVOKABLE void stopCudaProfiler();
signals:
    void bbProfilerTableChanged();
    void gaProfilerTableChanged();
    void cudaProfilerTableChanged();
    void isRunningChanged();
private slots:
    void handleBbTest(int verticesCount, double time, double distance);
    void handleGaTest(int verticesCount, double time, double distance);
    void handleCudaTest(int verticesCount, double time, double distance);

private:
    TableModel *m_BbProfilerTableModel, *m_GaProfilerTableModel, *m_CudaProfilerTableModel;
    Profiler  m_Profiler;
    bool m_IsRunning;
};

