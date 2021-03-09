#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent) {

}

int TableModel::rowCount(const QModelIndex &) const {
    return m_Data.count();
}

int TableModel::columnCount(const QModelIndex &) const {
    return m_Data.count();
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Qt::DisplayRole: {
            return m_Data.at(index.row()).at(index.column());
        }
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> TableModel::roleNames() const {
    return { {Qt::DisplayRole, "display"} };
}

void TableModel::setMatrix(const QList<QList<double>>& data) {
    beginResetModel();
    m_Data = data;
    endResetModel();
}
