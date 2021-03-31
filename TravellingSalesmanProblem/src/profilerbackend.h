#pragma once

#include <QObject>

#include "tablemodel.h"
#include "bbprofiler.h"

class ProfilerBackend : public QObject {
    Q_OBJECT
public:
    explicit ProfilerBackend(QObject *parent = nullptr);

    Q_PROPERTY(QAbstractTableModel* profilerTable READ getProfilerTable NOTIFY profilerTableChanged)
    Q_PROPERTY(bool isRunning READ isRunning WRITE setIsRunning NOTIFY isRunningChanged)

    void setIsRunning(bool running);

    QAbstractTableModel* getProfilerTable() const;
    bool isRunning() const;


    Q_INVOKABLE void startProfiler();
    Q_INVOKABLE void stopProfiler();
signals:
    void profilerTableChanged();
    void isRunningChanged();
private slots:
    void handleTest(int verticesCount, double time, double memory);

private:
    TableModel *m_ProfilerTableModel;
    BBProfiler m_Profiler;
    bool m_IsRunning;
};

