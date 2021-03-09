#pragma once

#include <QUrl>
#include <QList>
#include <QFile>
#include <QTextStream>

class MatrixLoader {
public:
    MatrixLoader() = default;
    QList<QList<double>> load(const QUrl& url) const {
        QFile file(url.toLocalFile());
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QList<QList<double>>();
        }
        QList<QList<double>> result;
        QTextStream in(&file);
        while(!in.atEnd()) {
            QString line = in.readLine();
            QStringList numbers = line.split(" ");
            QList<double> data;
            for(QString& stringNubmer : numbers) {
                data.append(stringNubmer.toDouble());
            }
            result.append(data);
        }
        file.close();
        return result;
    }
};

