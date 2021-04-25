#pragma once

#include <QObject>
#include <QAbstractTableModel>

#include "tablemodel.h"
#include "drawingalgorithm.h"
#include "staticthreadpool.h"
#include "ialgorithm.h"

class Node;
class BranchAndBound;
class GeneticAlgorithm;

class Backend : public QObject {
    Q_OBJECT

    /*!
      Table for ui
    */
    Q_PROPERTY(QAbstractTableModel* graphMatrix READ getGraphMatrix NOTIFY graphMatrixChanged)

    Q_PROPERTY(QAbstractTableModel* penaltyMatrix READ getPenaltyMatrix NOTIFY penaltyMatrixChanged)

    Q_PROPERTY(QAbstractTableModel* profilerTable READ getProfilerTable NOTIFY profilerTableChanged)

    /*!
      Algorithm
    */
    Q_PROPERTY(IAlgorithm* currentAlgorithm READ getCurrentAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY(int currentAlgorithmIndex READ getCurrentAlgorithmIndex WRITE setCurrentAlgorithmIndex NOTIFY currentAlgorithmIndexChanged)
    /*!
        Path from branch and bound alg
    */
    Q_PROPERTY(QString optimalPath READ getOptimalPath WRITE setOptimalPath NOTIFY optimalPathChanged)
public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void openGraphMatrixFile(const QUrl &url);
    Q_INVOKABLE void openPenaltyMatrixFile(const QUrl &url);
    Q_INVOKABLE void enablePenalties();
    Q_INVOKABLE GraphMatrix getLoadedMatrix();

    QAbstractTableModel* getGraphMatrix() const;
    QAbstractTableModel* getPenaltyMatrix() const;
    QAbstractTableModel* getProfilerTable() const;

    IAlgorithm* getCurrentAlgorithm() const;

    QString getOptimalPath() const;
    void setOptimalPath(const QString& path);

    int getCurrentAlgorithmIndex() const;
    void setCurrentAlgorithmIndex(int index);
private slots:
    void onBbFinished(Node *endNode, Node *rootNode);
    void onGaFinished(double optimalDistance, QList<int> optimalPath);
private:
    void multiplyMatrix();
signals:
    void graphMatrixChanged();
    void penaltyMatrixChanged();
    void profilerTableChanged();
    void adjacencyMatrixLoaded(const GraphMatrix& matrix);
    void optimalPathChanged();
    void graphPathChanged(const QList<QPair<int, int>>& path);
    void treeNodeReceived(Node *node);
    void algorithmChanged();
    void currentAlgorithmIndexChanged();
private:
    TableModel *m_GraphMatrixModel, *m_PenaltyMatrixModel, *m_ProfilerTableModel;
    QList<IAlgorithm*> m_Algorithms;
    int m_CurrentAlgorithmIndex;
    QList<QString> m_OptimalPaths;
    StaticThreadPool m_Pool;
    GraphMatrix m_Matrix, m_PenaltiedMatrix, m_Penalties;
    bool m_PenaltiesEnabled = false;
};

