#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent) {

}

int TableModel::rowCount(const QModelIndex &) const {
    return m_Data.count();
}

int TableModel::columnCount(const QModelIndex &) const {
    if(!m_Data.isEmpty()) {
        return m_Data.first().count();
    }
    return 0;
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

void TableModel::insertRow(const QList<double> &row) {
    beginInsertRows(QModelIndex(), m_Data.size(), m_Data.size() + 1);
    m_Data.append(row);
    endInsertRows();
}

void TableModel::clearTable() {
    beginResetModel();
    m_Data.clear();
    endResetModel();
}
