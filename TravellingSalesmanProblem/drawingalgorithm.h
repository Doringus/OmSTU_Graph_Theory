#pragma once

#include <QObject>
#include <QList>

using GraphMatrix = QList<QList<double>>;


/*!
 * \brief Base class for all drawing algs
 */
class DrawingAlgorithm : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool showSteps READ showSteps WRITE setShowSteps NOTIFY showStepsChanged)
    Q_PROPERTY(GraphMatrix adjacencyMatrix READ getAdjacencyMatrix WRITE setAdjacencyMatrix NOTIFY adjacencyMatrixChanged)
public:
    DrawingAlgorithm(QObject *parent = nullptr) : QObject(parent){}

    void setShowSteps(bool show) {
        m_ShowSteps = show;
        emit showStepsChanged();
    }

    bool showSteps() const {
        return m_ShowSteps;
    };

    void setAdjacencyMatrix(const GraphMatrix& matrix) {
        m_GraphMatrix = matrix;
        emit adjacencyMatrixChanged();
    }

    GraphMatrix getAdjacencyMatrix() const {
        return m_GraphMatrix;
    }

    /*!
     * \brief Function starts algorithm
     */
    Q_INVOKABLE virtual void start() = 0;
signals:
    void showStepsChanged();
    void adjacencyMatrixChanged();

    /*!
     * \brief Signal emited when vertices`s positions changed
     * If showSteps is true, then it will be emited after every iteration
     * of algorithm. Else it will be emited only after last iteration
     */
    void layoutChanged();
protected:
    bool m_ShowSteps;
    GraphMatrix m_GraphMatrix;
};
