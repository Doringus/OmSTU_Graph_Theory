#include "pdfwriter.h"

#include <QDebug>
#include <QUrl>
#include <QPageSize>
#include <QPdfWriter>
#include <QPainter>

PdfWriter::PdfWriter(QObject *parent) : QObject(parent) {

}

void PdfWriter::saveToFile(QUrl& url, const QImage& image, const QString& path) {
    QPdfWriter writer(url.toLocalFile());
    writer.setPageMargins(QMargins(30, 30, 30, 30));
    QPainter painter(&writer);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.drawImage(QRectF(30, 30, painter.viewport().width(), 5000), image);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Times", 14));
    painter.drawText(QRectF(30, 5040, painter.viewport().width(), painter.viewport().height()), path);
    painter.end();
}
