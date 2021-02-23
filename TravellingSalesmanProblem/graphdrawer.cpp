#include "graphdrawer.h"

#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QQuickWindow>
#include <QPainter>
#include <QSGImageNode>

GraphDrawer::GraphDrawer(QQuickItem *parent) : QQuickItem(parent){
    setFlag(QQuickItem::ItemHasContents);
    connect(this, &QQuickItem::heightChanged, [=](){
    });
}

GraphDrawer::Algorithm GraphDrawer::getAlgorithm() const {
    return m_Algorithm;
}

void GraphDrawer::setAlgorithm(GraphDrawer::Algorithm algorithm) {
    m_Algorithm = algorithm;
    emit algorithmChanged();
}

QSGNode *GraphDrawer::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) {
    Q_UNUSED(updatePaintNodeData)
    if(!oldNode) {
        QSGGeometry *dot = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 20);
        dot->setDrawingMode(QSGGeometry::DrawLineLoop);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        QSGGeometryNode *node = new QSGGeometryNode;
        node->setGeometry(dot);
        node->setFlag(QSGNode::OwnsGeometry);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

        QImage img(width(), height(), QImage::Format_RGBA8888);
        img.fill(QColor("red"));
        QPainter painter(&img);
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setFamily("Arial");
        font.setPixelSize(25);
        painter.setFont(font);
        painter.drawText(QRect(20, 20, 50, 50), "SAS");
        painter.drawEllipse(15, 15, 30, 30);
        QSGTexture *texture = window()->createTextureFromImage(img);
        QSGImageNode *imageNode = window()->createImageNode();
        imageNode->setTexture(texture);
        imageNode->setOwnsTexture(true);
        imageNode->setRect(QRect(0, 0, width(), height()));
        imageNode->setSourceRect(QRect(0, 0, width(), height()));
        return imageNode;
    }
    oldNode->markDirty(QSGNode::DirtyGeometry);
    return oldNode;
}
