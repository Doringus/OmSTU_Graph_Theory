#pragma once

#include <QObject>
#include <QQuickItemGrabResult>

class PdfWriter : public QObject {
    Q_OBJECT
public:
    explicit PdfWriter(QObject *parent = nullptr);
    Q_INVOKABLE void saveToFile(QUrl &url, const QImage &image, const QString& path);
};

