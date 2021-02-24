#pragma once

#include <QQuickPaintedItem>
#include <QObject>

#include "drawingalgorithm.h"

class GraphLayout : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(DrawingAlgorithm *algorithm READ getAlgorithm WRITE setAlgorithm NOTIFY algorithmChanged)
public:
    GraphLayout(QQuickPaintedItem *parent = nullptr);
    void paint(QPainter *painter) override;

    DrawingAlgorithm *getAlgorithm();
    void setAlgorithm(DrawingAlgorithm* algorithm);
signals:
    void algorithmChanged();
private:
    DrawingAlgorithm *m_Algorithm;
};

