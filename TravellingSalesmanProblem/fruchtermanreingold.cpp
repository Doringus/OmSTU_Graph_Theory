#include "fruchtermanreingold.h"

#include <QDebug>
#include <QPointF>
#include <QRandomGenerator>
#include <QVector2D>
#include <QThread>

#include <cmath>

FruchtermanReingold::FruchtermanReingold(QObject *parent) : DrawingAlgorithm(parent) {

}

void FruchtermanReingold::start() {
    /// Validate all params
    if(m_AreaWidth <= 0 || m_AreaHeight <= 0) {
        qWarning() << "Empty area";
        return;
    }
    for(auto& line: m_GraphMatrix) {
        if(m_GraphMatrix.count() != line.count()) {
            qWarning() << "Matrix is not square";
            return;
        }
    }
    /// Calculate ideal distance
    double idealDistance = calculateIdealDistance(m_AreaWidth, m_AreaHeight, 6);

    /// Prepare list of vertuces randomly placed
    QList<QVector2D> vertices;
    vertices.reserve(m_GraphMatrix.count());
    for(int i = 0; i < m_GraphMatrix.count(); ++i) {
        QVector2D vertex(QRandomGenerator::global()->bounded(m_AreaWidth),
                         QRandomGenerator::global()->bounded(m_AreaHeight));
        vertices.push_back(vertex);
    }

    double temp = 8 * sqrt(vertices.count());

    /// Start algorithm
    for(int i = 0; i < 15; ++i) {
        iterate(vertices, idealDistance, temp);
        temp *= 0.7;
        if(m_ShowSteps) {
            emit layoutChanged();
        }
    }

    /// Fill internal list
    m_Vertices.clear();
    m_Vertices.reserve(vertices.count());
    for(auto vec : vertices) {
        m_Vertices.push_back(vec.toPointF());
    }
    emit layoutChanged();
}

void FruchtermanReingold::setArea(double width, double height) {
    m_AreaWidth = width;
    m_AreaHeight = height;
}

double FruchtermanReingold::getWidth() const {
    return m_AreaWidth;
}

void FruchtermanReingold::setWidth(double width) {
    m_AreaWidth = width;
}

double FruchtermanReingold::getHeight() const {
    return m_AreaHeight;
}

void FruchtermanReingold::setHeight(double height) {
    m_AreaHeight = height;
}

double FruchtermanReingold::calculateIdealDistance(double width, double height, unsigned int verticesCount) const {
    return std::sqrt((width * height) / static_cast<double>(verticesCount));
}

void FruchtermanReingold::iterate(QList<QVector2D> &vertices, double idealDistance, double temp) {
    QList<QVector2D> displacement(vertices.count());
    displacement.fill({0, 0});

    for(int i = 0; i < vertices.count(); ++i) {

        /// Calculate repulsion force
        for(int j = i + 1; j < vertices.count(); ++j) {
            if(i == j) {
                continue;
            }
            QVector2D delta(vertices.at(i) - vertices.at(j));
            double deltaLength = delta.length();

            /// Repulsion force
            QVector2D repForce = (delta / deltaLength) * (std::pow(idealDistance, 2) / deltaLength);
            displacement[i] += repForce;
            displacement[j] -= repForce;
        }

        /// Calculate attraction force
        for(int j = 0; j < vertices.count(); ++j) {
            /// Skip repeating and non adj vertices
            if(j <= i || m_GraphMatrix.at(i).at(j) == 0) {
                continue;
            }
            QVector2D delta(vertices.at(i) - vertices.at(j));
            double deltaLength = delta.length();

            /// Attr force
            QVector2D attrForce = (delta / deltaLength) * (std::pow(deltaLength, 2) / idealDistance);
            displacement[i] -= attrForce;
            displacement[j] += attrForce;
        }
    }

    /// Cap max displacement by temperature
    /// and apply it to positions
    for(int i = 0; i < displacement.count(); ++i) {
        double length = displacement.at(i).length();
        double optimalLength = std::min(length, temp);
        vertices[i] += (displacement[i] / length) * optimalLength;

        /// Check if vertex out of bound
        if(vertices[i].x() > m_AreaWidth) {
            vertices[i].setX(m_AreaWidth);
        }
        if(vertices[i].x() < 0) {
            vertices[i].setX(0);
        }
        if(vertices[i].y() > m_AreaHeight) {
            vertices[i].setY(m_AreaHeight);
        }
        if(vertices[i].y() < 0) {
            vertices[i].setY(0);
        }
    }
}
