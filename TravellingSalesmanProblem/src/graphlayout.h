#pragma once

#include <QQuickPaintedItem>
#include <QObject>

#include "drawingalgorithm.h"

class GraphLayout : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(DrawingAlgorithm *algorithm READ getAlgorithm WRITE setAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY(GraphMatrix adjacencyMatrix READ getAdjacencyMatrix WRITE setAdjacencyMatrix NOTIFY adjacencyMatrixChanged)
    Q_PROPERTY(bool drawEdges READ getDrawEdges WRITE setDrawEdges NOTIFY drawEdgesChanged)
public:
    GraphLayout(QQuickPaintedItem *parent = nullptr);
    void paint(QPainter *painter) override;

    DrawingAlgorithm *getAlgorithm();
    void setAlgorithm(DrawingAlgorithm* algorithm);

    GraphMatrix getAdjacencyMatrix() const;
    void setAdjacencyMatrix(const GraphMatrix& matrix);

    Q_INVOKABLE void drawPath(const QList<QPair<int, int>>& path);

    bool getDrawEdges() const;
    void setDrawEdges(bool draw);
private:
    void createEdges();
    void drawEdge(edge_t& edge, QPainter *painter);
    void createPathEdges();
signals:
    void algorithmChanged();
    void adjacencyMatrixChanged();
    void drawEdgesChanged();
private:
    DrawingAlgorithm *m_Algorithm;
    GraphMatrix m_GraphMatrix;
    bool m_DrawEdges;
    QList<QPointF> m_Vertices;
    QList<edge_t> m_Edges, m_PathEdges;
    QList<QPair<int, int>> m_Path;
};

