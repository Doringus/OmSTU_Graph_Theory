#include "backend.h"

#include "matrixloader.h"
#include "matrixmultiplier.h"
#include "branchandbound.h"

QVector<QPair<int, int>> sortEdges(QVector<QPair<int, int>> &edges) {
    QVector<QPair<int, int>> result;
    result.reserve(edges.size());
    result.append(edges.first());
    edges.removeFirst();
    QPair<int, int> currentEdge = result.first();

    while(edges.count() > 0) {
        auto isNext = [&](const QPair<int, int>& edge) {
            return edge.first == currentEdge.second;
        };
        auto nextEdge = std::find_if(edges.begin(), edges.end(), isNext);
        result.append(*nextEdge);
        currentEdge = *nextEdge;
        edges.removeOne(*nextEdge);
    }
    return result;
}

Backend::Backend(QObject *parent) : QObject(parent), m_GraphMatrixModel(new TableModel(this)),
                                                    m_PenaltyMatrixModel(new TableModel(this)),
                                                    m_BranchAndBound(new BranchAndBound()){
    connect(m_BranchAndBound, &BranchAndBound::bbFinished, this, &Backend::onBbFinished, Qt::QueuedConnection);
}

void Backend::openGraphMatrixFile(const QUrl& url) {
    MatrixLoader loader;
    QList<QList<double>> r = loader.load(url);
    m_Matrix = r;
    m_GraphMatrixModel->setMatrix(r);
    emit adjacencyMatrixLoaded(r);
    multiplyMatrix();
}

void Backend::openPenaltyMatrixFile(const QUrl &url) {
    MatrixLoader loader;
    QList<QList<double>> r = loader.load(url);
    m_PenaltyMatrixModel->setMatrix(r);
    MatrixMultiplier mm;
    m_Penalties = r;
    multiplyMatrix();
}

void Backend::enablePenalties() {
    m_PenaltiesEnabled = !m_PenaltiesEnabled;
}

void Backend::startBB() {
    if(m_PenaltiesEnabled) {
        m_BranchAndBound->start(m_PenaltiedMatrix);
    } else {
        m_BranchAndBound->start(m_Matrix);
    }
 }

QAbstractTableModel *Backend::getGraphMatrix() const {
    return m_GraphMatrixModel;
}

QAbstractTableModel *Backend::getPenaltyMatrix() const {
    return m_PenaltyMatrixModel;
}

QString Backend::getOptimalPathBB() const {
    return m_OptimalPathBB;
}

void Backend::setOptimalPathBB(const QString &path) {
    m_OptimalPathBB = path;
    emit optimalPathBBChanged();
}

void Backend::onBbFinished(node_t *endNode, node_t *rootNode) {
    QString path("Оптимальный путь:\n");
    QVector<QPair<int, int>> edges = endNode->includedEdges;
    edges = sortEdges(edges);
    if(edges.count() == 0) {
        path += "0";
        for(int i = 1; i < endNode->matrix.count(); ++i) {
            path+= "->" + QString::number(i);
        }
    } else {
        path += QString::number(edges.first().first + 1);
        for(const auto& edge : edges) {
            path+= "->" + QString::number(edge.second + 1);
        }
    }
    path += "\nДлина: " + QString::number(endNode->weight);
    setOptimalPathBB(path);
    emit graphPathChanged(endNode->includedEdges);
    emit treeNodeReceived(rootNode);
}

void Backend::multiplyMatrix() {
    if(!m_Matrix.count() || !m_Penalties.count()) {
        return;
    }
    MatrixMultiplier mm;
    m_PenaltiedMatrix = mm.multiply(m_Matrix, m_Penalties);
}

