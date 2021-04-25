#pragma once

#include <QObject>

#include "common.h"

class IAlgorithm : public QObject {
    Q_OBJECT
public:
    IAlgorithm(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE virtual void start(const GraphMatrix& matrix) = 0;
};
