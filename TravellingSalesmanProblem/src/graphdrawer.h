#pragma once

#include <QQuickItem>

class GraphDrawer : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(Algorithm algorithm READ getAlgorithm WRITE setAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY(bool drawEdges READ getDrawEdges WRITE setDrawEdges NOTIFY drawEdgesChanged)
public:
    explicit GraphDrawer(QQuickItem *parent = 0);

    enum Algorithm {FR};
    Q_ENUM(Algorithm)

    Algorithm getAlgorithm() const;
    void setAlgorithm(Algorithm algorithm);

    bool getDrawEdges() const;
    void setDrawEdges(bool draw);
protected:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData) override;
signals:
    void algorithmChanged();
    void drawEdgesChanged();
private:
    Algorithm m_Algorithm;
    bool m_DrawEdges;
};

