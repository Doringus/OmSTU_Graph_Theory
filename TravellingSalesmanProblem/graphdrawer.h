#pragma once

#include <QQuickItem>

class GraphDrawer : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(Algorithm algorithm READ getAlgorithm WRITE setAlgorithm NOTIFY algorithmChanged)
public:
    explicit GraphDrawer(QQuickItem *parent = 0);

    enum Algorithm {FR};
    Q_ENUM(Algorithm)

    Algorithm getAlgorithm() const;
    void setAlgorithm(Algorithm algorithm);
protected:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData) override;
signals:
    void algorithmChanged();
private:
    Algorithm m_Algorithm;
};

