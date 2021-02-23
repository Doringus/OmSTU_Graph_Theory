#pragma once

#include "drawingalgorithm.h"

class FruchtermanReingold : public DrawingAlgorithm {
    Q_OBJECT
public:
    FruchtermanReingold(QObject *parent = nullptr);

    Q_INVOKABLE void start() override;

    void setArea(double width, double height);
private:
    /*!
     * \brief Function calculates "ideal"
     * \param width drawing area width
     * \param height drawing area height
     * \param verticesCount number of vertices to draw
     * \return return ideal distance by sqrt((width * height) / verticesCount)
     */
    double calculateIdealDistance(double width, double height, unsigned int verticesCount) const;

    /*!
     * \brief Fucntion makes one iteration over algorithm
     * \param vertices list of vertices randomly placed
     * \param idealDistance ideal distance
     * \param temp current temperature
     */
    void iterate(QList<QVector2D> &vertices, double idealDistance, double temp);
private:
    /*!
     * \brief Properties used for calculating "ideal" distance between vertices
     */
    double m_AreaWidth, m_AreaHeight;

    /*!
     * \brief Property for caping displacement
     */
};

