#include "graphlayout.h"

#include <QPainter>
#include <QPainterPath>

GraphLayout::GraphLayout(QQuickPaintedItem *parent) : QQuickPaintedItem(parent),
                                                      m_Algorithm(nullptr) {

}

void GraphLayout::paint(QPainter *painter) {
    if(!m_Algorithm) {
        return;
    }
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::TextAntialiasing);

    if(m_DrawEdges) {
        painter->setPen(QPen(Qt::GlobalColor::lightGray, 2, Qt::SolidLine, Qt::RoundCap));
        for(auto edge : m_Edges) {
            drawEdge(edge, painter);
        }
    }

    for(auto edge : m_Path) {
        painter->setPen(QPen(edge.color, 2, Qt::SolidLine, Qt::RoundCap));
        drawEdge(edge, painter);
    }


    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::GlobalColor::cyan));
    for(auto p : m_Algorithm->getVertices()) {
        painter->drawEllipse(p.x() - 15, p.y() - 15, 30, 30);
    }
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setFamily("Arial");
    font.setPixelSize(20);
    painter->setFont(font);

    int i = 1;
    for(auto p : m_Algorithm->getVertices()) {
        painter->drawText(QRect(p.x() - 15, p.y() - 15, 30, 30), Qt::AlignCenter, QString::number(i));
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
    connect(m_Algorithm, &DrawingAlgorithm::layoutChanged, this, &GraphLayout::createEdges);
    emit algorithmChanged();
}

GraphMatrix GraphLayout::getAdjacencyMatrix() const {
    return m_GraphMatrix;
}

void GraphLayout::setAdjacencyMatrix(const GraphMatrix &matrix) {
    m_GraphMatrix = matrix;
    m_Algorithm->setAdjacencyMatrix(matrix);
    emit adjacencyMatrixChanged();
}

void GraphLayout::drawPath(const QList<QPair<int, int>>& path) {
    m_Path.clear();
    for(const auto& v : path) {
        edge_t edge;
        edge.color = Qt::GlobalColor::green;
        edge.line = {m_Vertices.at(v.first).x(), m_Vertices.at(v.first).y(),
                    m_Vertices.at(v.second).x(), m_Vertices.at(v.second).y()};
        m_Path.append(edge);
    }
}


bool GraphLayout::getDrawEdges() const {
    return m_DrawEdges;
}

void GraphLayout::setDrawEdges(bool draw) {
    m_DrawEdges = draw;
    emit drawEdgesChanged();
    update();
}

void GraphLayout::createEdges() {
    m_Edges.clear();
    m_Vertices = m_Algorithm->getVertices();
    for(int i = 0; i < m_GraphMatrix.count(); ++i) {
        for(int j = i; j < m_GraphMatrix.count(); ++j) {
            if(!m_GraphMatrix.at(i).at(j) || i == j) {
                continue;
            }
            edge_t edge;
            edge.line = {m_Vertices.at(i).x() + 15, m_Vertices.at(i).y() + 15,
                        m_Vertices.at(j).x() + 15, m_Vertices.at(j).y() + 15};
            m_Edges.push_back(edge);
        }
    }
    update();
}

void GraphLayout::drawEdge(edge_t &edge, QPainter *painter) {
    QPainterPath path;
    painter->drawLine(edge.line);
    edge.line.setLength(edge.line.length() - 15);
    double angle = std::atan2(-edge.line.dy(), edge.line.dx());
    QPointF arrowP1 = edge.line.p2() - QPointF(sin(angle + M_PI / 6) * 15,
                                        cos(angle + M_PI / 6) * 15);
    QPointF arrowP2 = edge.line.p2() - QPointF(sin(angle + M_PI - M_PI / 6) * 15,
                                        cos(angle + M_PI - M_PI / 6) * 15);
    path.addPolygon({arrowP1 , arrowP2, edge.line.p2()});
    painter->fillPath(path, Qt::green);
    painter->drawPath(path);
}

