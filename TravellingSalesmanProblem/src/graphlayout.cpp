#include "graphlayout.h"

#include <QPainter>

GraphLayout::GraphLayout(QQuickPaintedItem *parent) : QQuickPaintedItem(parent),
                                                      m_Algorithm(nullptr) {

}

void GraphLayout::paint(QPainter *painter) {
    if(!m_Algorithm) {
        return;
    }
    painter->setRenderHints(QPainter::Antialiasing, true);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::GlobalColor::cyan));
    for(auto p : m_Algorithm->getVertices()) {
        painter->drawEllipse(p.x(), p.y(), 30, 30);
    }

    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->setPen(QPen(Qt::GlobalColor::lightGray, 2, Qt::SolidLine, Qt::RoundCap));
    for(auto edge : m_Algorithm->getEdges()) {
        painter->drawLine(edge.x1 + 15, edge.y1 + 15, edge.x2 + 15, edge.y2 + 15);
    }
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setFamily("Arial");
    font.setPixelSize(20);
    painter->setFont(font);

    int i = 1;
    for(auto p : m_Algorithm->getVertices()) {
        painter->drawText(QRect(p.x(), p.y(), 30, 30), Qt::AlignCenter, QString::number(i));
        ++i;
    }
}

DrawingAlgorithm *GraphLayout::getAlgorithm() {
    return m_Algorithm;
}

void GraphLayout::setAlgorithm(DrawingAlgorithm *algorithm) {
    /// Disconnect signal from previous alg
    if(m_Algorithm) {
        disconnect(m_Algorithm, &DrawingAlgorithm::layoutChanged, nullptr, nullptr);
    }
    m_Algorithm = algorithm;
    connect(m_Algorithm, &DrawingAlgorithm::layoutChanged, this,[=](){
        update();
    });
    emit algorithmChanged();
}
