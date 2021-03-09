#pragma once

#include <QQuickPaintedItem>
#include <QObject>

#include "drawingalgorithm.h"

class GraphLayout : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(DrawingAlgorithm *algorithm READ getAlgorithm WRITE setAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY(GraphMatrix adjacencyMatrix READ getAdjacencyMatrix WRITE setAdjacencyMatrix NOTIFY adjacencyMatrixChanged)
public:
    GraphLayout(QQuickPaintedItem *parent = nullptr);
    void paint(QPainter *painter) override;

    DrawingAlgorithm *getAlgorithm();
    void setAlgorithm(DrawingAlgorithm* algorithm);

    GraphMatrix getAdjacencyMatrix() const;
    void setAdjacencyMatrix(const GraphMatrix& matrix);

    Q_INVOKABLE void setEdgeColor(int from, int to, const QColor& color);
signals:
    void algorithmChanged();
    void adjacencyMatrixChanged();
private:
    DrawingAlgorithm *m_Algorithm;
    GraphMatrix m_GraphMatrix;
};

