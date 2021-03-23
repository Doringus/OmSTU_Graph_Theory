#include "backend.h"

#include "matrixloader.h"
#include "branchandbound.h"

void sortEdges(QVector<QPair<int, int>> &edges) {
    QPair<int, int> currentEdge = edges.first();
    for(int i = 1; i < edges.count() - 1; ++i) {
        for(int j = i + 1; j < edges.count(); ++j) {
            if(edges[j].first == currentEdge.second) {
                currentEdge = edges[j];
                edges.swapItemsAt(i, j);
                break;
            }
        }
    }
}

Backend::Backend(QObject *parent) : QObject(parent), m_GraphMatrixModel(new TableModel(this)),
                                                    m_BranchAndBound(new BranchAndBound()){
    connect(m_BranchAndBound, &BranchAndBound::bbFinished, this, &Backend::onBbFinished);
}

void Backend::openGraphMatrixFile(const QUrl& url) {
    MatrixLoader loader;
    QList<QList<double>> r = loader.load(url);
    m_GraphMatrixModel->setMatrix(r);
    m_BranchAndBound->setMatrix(r);
    emit adjacencyMatrixLoaded(r);
  //  m_BranchAndBound->start(r);
    m_Pool.putTask(m_BranchAndBound);
}

QAbstractTableModel *Backend::getGraphMatrix() const {
    return m_GraphMatrixModel;
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
    sortEdges(edges);
    if(edges.count() == 0) {
        path += "0";
        for(int i = 1; i < endNode->matrix.count(); ++i) {
            path+= "->" + QString::number(i);
        }
    } else {
        qDebug() << "EDGES" << edges;
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

