#pragma once

#include <QObject>
#include <QAbstractTableModel>

#include "tablemodel.h"
#include "drawingalgorithm.h"
#include "staticthreadpool.h"

struct node_t;
class BranchAndBound;

class Backend : public QObject {
    Q_OBJECT

    /*!
      Table for ui
    */
    Q_PROPERTY(QAbstractTableModel* graphMatrix READ getGraphMatrix NOTIFY graphMatrixChanged)

    Q_PROPERTY(QAbstractTableModel* penaltyMatrix READ getPenaltyMatrix NOTIFY penaltyMatrixChanged)

    Q_PROPERTY(QAbstractTableModel* profilerTable READ getProfilerTable NOTIFY profilerTableChanged)
    /*!
        Path from branch and bound alg
    */
    Q_PROPERTY(QString optimalPathBB READ getOptimalPathBB WRITE setOptimalPathBB NOTIFY optimalPathBBChanged)
public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void openGraphMatrixFile(const QUrl &url);
    Q_INVOKABLE void openPenaltyMatrixFile(const QUrl &url);
    Q_INVOKABLE void enablePenalties();
    Q_INVOKABLE void startBB();

    QAbstractTableModel* getGraphMatrix() const;
    QAbstractTableModel* getPenaltyMatrix() const;
    QAbstractTableModel* getProfilerTable() const;

    QString getOptimalPathBB() const;
    void setOptimalPathBB(const QString& path);
private slots:
    void onBbFinished(node_t *endNode, node_t *rootNode);
private:
    void multiplyMatrix();
signals:
    void graphMatrixChanged();
    void penaltyMatrixChanged();
    void profilerTableChanged();
    void adjacencyMatrixLoaded(const GraphMatrix& matrix);
    void optimalPathBBChanged();
    void graphPathChanged(const QList<QPair<int, int>>& path);
    void treeNodeReceived(node_t *node);
private:
    TableModel *m_GraphMatrixModel, *m_PenaltyMatrixModel, *m_ProfilerTableModel;
    BranchAndBound *m_BranchAndBound;
    QString m_OptimalPathBB;
    StaticThreadPool m_Pool;
    GraphMatrix m_Matrix, m_PenaltiedMatrix, m_Penalties;
    bool m_PenaltiesEnabled = false;
};

