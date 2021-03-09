#pragma once

#include <QObject>
#include <QAbstractTableModel>

#include "tablemodel.h"
#include "drawingalgorithm.h"

class Backend : public QObject {
    Q_OBJECT

    /*!
      Table for ui
    */
    Q_PROPERTY(QAbstractTableModel* graphMatrix READ getGraphMatrix NOTIFY graphMatrixChanged)
public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void openGraphMatrixFile(const QUrl &url);

    QAbstractTableModel* getGraphMatrix() const;
signals:
    void graphMatrixChanged();
    void adjacencyMatrixLoaded(const GraphMatrix& matrix);
private:
    TableModel *m_GraphMatrixModel;
};

