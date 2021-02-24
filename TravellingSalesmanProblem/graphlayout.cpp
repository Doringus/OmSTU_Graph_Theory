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
