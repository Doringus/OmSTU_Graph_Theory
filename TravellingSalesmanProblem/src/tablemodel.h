#pragma once

#include <QAbstractTableModel>
#include <QObject>


class TableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    TableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setMatrix(const QList<QList<double> > &data);

    void insertRow(const QList<double>& row);
    void clearTable();
private:
    QList<QList<double>> m_Data;
};

