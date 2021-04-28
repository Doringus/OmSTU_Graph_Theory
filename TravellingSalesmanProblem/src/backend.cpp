#include "backend.h"

#include "matrixloader.h"
#include "matrixmultiplier.h"
#include "branchandbound.h"
#include "ga.h"

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
                                                    m_ProfilerTableModel(new TableModel(this)),
                                                    m_CurrentAlgorithmIndex(0) {
    m_OptimalPaths.fill("", 2);
    BranchAndBound *bb = new BranchAndBound(this);
    connect(bb, &BranchAndBound::bbFinished, this, &Backend::onBbFinished, Qt::QueuedConnection);
    GeneticAlgorithm *ga = new GeneticAlgorithm(this);
    connect(ga, &GeneticAlgorithm::finished, this, &Backend::onGaFinished, Qt::QueuedConnection);
    m_Algorithms.append(bb);
    m_Algorithms.append(ga);
}

void Backend::openGraphMatrixFile(const QUrl& url) {
    MatrixLoader loader;
    QList<QList<double>> r = loader.load(url);
    m_Matrix = r;
    m_GraphMatrixModel->setMatrix(r);
    if(m_Matrix.count() < 10) {
        emit adjacencyMatrixLoaded(r);
    }
    multiplyMatrix();
}

void Backend::openPenaltyMatrixFile(const QUrl &url) {
    MatrixLoader loader;
    QList<QList<double>> r = loader.load(url);
    m_PenaltyMatrixModel->setMatrix(r);
    m_Penalties = r;
    multiplyMatrix();
}

void Backend::enablePenalties() {
    m_PenaltiesEnabled = !m_PenaltiesEnabled;
}

GraphMatrix Backend::getLoadedMatrix() {
    if(m_PenaltiesEnabled) {
        return m_PenaltiedMatrix;
    } else {
        return m_Matrix;
    }
}

QAbstractTableModel *Backend::getGraphMatrix() const {
    return m_GraphMatrixModel;
}

QAbstractTableModel *Backend::getPenaltyMatrix() const {
    return m_PenaltyMatrixModel;
}

QAbstractTableModel *Backend::getProfilerTable() const {
    return m_ProfilerTableModel;
}

IAlgorithm *Backend::getCurrentAlgorithm() const {
    return m_Algorithms.at(m_CurrentAlgorithmIndex);
}

QString Backend::getOptimalPath() const {
    return m_OptimalPaths.at(m_CurrentAlgorithmIndex);
}

void Backend::setOptimalPath(const QString &path) {
    m_OptimalPaths[m_CurrentAlgorithmIndex] = std::move(path);
    emit optimalPathChanged();
}

int Backend::getCurrentAlgorithmIndex() const {
    return m_CurrentAlgorithmIndex;
}

void Backend::setCurrentAlgorithmIndex(int index) {
    m_CurrentAlgorithmIndex = index;
    emit currentAlgorithmIndexChanged();
    emit algorithmChanged();
    emit optimalPathChanged();
}

void Backend::onBbFinished(Node *endNode, Node *rootNode) {
    qDebug() << "EDGES: " << endNode->getIncludedEdges();
    QString path("Оптимальный путь:\n");
    QList<QPair<int, int>> edges = endNode->getIncludedEdges();
    if(edges.count() == 0) {
        path += "1";
        for(int i = 2; i <= m_Matrix.count(); ++i) {
            path+= "->" + QString::number(i);
        }
        path += "->1";
    } else {
       // edges = sortEdges(edges);
        path += QString::number(edges.first().first + 1);
        for(const auto& edge : edges) {
            path+= "->" + QString::number(edge.second + 1);
        }
    }
    path += "\nДлина: " + QString::number(endNode->getWeight());
    setOptimalPath(path);
    if(m_Matrix.count() < 10) {
        emit graphPathChanged(endNode->getIncludedEdges());
        emit treeNodeReceived(rootNode);
    }
}

void Backend::onGaFinished(double optimalDistance, QList<int> optimalPath) {
    qDebug() << optimalDistance << optimalPath;
    QString path("Оптимальный путь:\n");
    for(const auto& p : optimalPath) {
        path += QString::number(p + 1) + "->";
    }
    path += QString::number(optimalPath.first() + 1);
    path += "\nДлина: " + QString::number(optimalDistance);
    setOptimalPath(path);
    QList<QPair<int, int>> edges;
    for(int i = 1; i < optimalPath.count(); ++i) {
        edges.append({optimalPath.at(i - 1), optimalPath.at(i)});
    }
    edges.append({optimalPath.last(), optimalPath.first()});
    if(m_Matrix.count() < 10) {
        emit graphPathChanged(edges);
    }
}

void Backend::multiplyMatrix() {
    if(!m_Matrix.count() || !m_Penalties.count()) {
        return;
    }
    MatrixMultiplier mm;
    m_PenaltiedMatrix = mm.multiply(m_Matrix, m_Penalties);
}

