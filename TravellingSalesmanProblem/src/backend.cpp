#include "backend.h"

#include "matrixloader.h"
#include "branchandbound.h"

Backend::Backend(QObject *parent) : QObject(parent), m_GraphMatrixModel(new TableModel) {

}

void Backend::openGraphMatrixFile(const QUrl& url) {
    MatrixLoader loader;
    QList<QList<double>> r = loader.load(url);
    m_GraphMatrixModel->setMatrix(r);
    emit adjacencyMatrixLoaded(r);
    BranchAndBound bb;
    bb.start(r);
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

void Backend::onBbFinished(node_t *endNode) {

}

