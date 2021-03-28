#pragma once

#include <QList>

class MatrixMultiplier {
public:
    MatrixMultiplier() = default;
    QList<QList<double>> multiply(const QList<QList<double>> &lhs, const QList<QList<double>> &rhs) {
        QList<QList<double>> result;
        result.reserve(lhs.count());
        for(int i = 0; i < lhs.count(); ++i) {
            QList<double> row;
            row.reserve(lhs.at(i).count());
            for(int j = 0; j < lhs.count(); ++j) {
                row.append(lhs.at(i).at(j) * rhs.at(i).at(j));
            }
            result.append(row);
        }
        return result;
    }
};
