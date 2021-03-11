#pragma once

#include <QObject>
#include <QAbstractTableModel>

#include "tablemodel.h"
#include "drawingalgorithm.h"

struct node_t;

class Backend : public QObject {
    Q_OBJECT

    /*!
      Table for ui
    */
    Q_PROPERTY(QAbstractTableModel* graphMatrix READ getGraphMatrix NOTIFY graphMatrixChanged)

    /*!
        Path from branch and bound alg
    */
    Q_PROPERTY(QString optimalPathBB READ getOptimalPathBB WRITE setOptimalPathBB NOTIFY optimalPathBBChanged)
public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void openGraphMatrixFile(const QUrl &url);

    QAbstractTableModel* getGraphMatrix() const;

    QString getOptimalPathBB() const;
    void setOptimalPathBB(const QString& path);
private slots:
    void onBbFinished(node_t *endNode);
signals:
    void graphMatrixChanged();
    void adjacencyMatrixLoaded(const GraphMatrix& matrix);
    void optimalPathBBChanged();
private:
    TableModel *m_GraphMatrixModel;
    QString m_OptimalPathBB;
};

