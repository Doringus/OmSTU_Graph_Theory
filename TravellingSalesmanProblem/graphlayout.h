#pragma once

#include <QQuickPaintedItem>
#include <QObject>

class GraphLayout : public QQuickPaintedItem {
    Q_OBJECT


public:
    GraphLayout(QQuickPaintedItem *parent = nullptr);
    void paint(QPainter *painer) override;
};

