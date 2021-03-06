#pragma once

#include <QQuickPaintedItem>
#include <QObject>

#include "branchandbound.h"


class TreeDrawer : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(Node* root READ getRoot WRITE setRoot NOTIFY rootChanged)
    Q_PROPERTY(int contentWidth READ getContentWidth WRITE setContentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(int contentHeight READ getContentHeight WRITE setContentHeight NOTIFY contentHeightChanged)
public:
    TreeDrawer(QQuickPaintedItem *parent = nullptr);
    void paint(QPainter *painter) override;

    Node* getRoot() const;
    void setRoot(Node *root);

    int getContentWidth() const;
    void setContentWidth(int width);

    int getContentHeight() const;
    void setContentHeight(int height);
private:
    void drawNode(QPainter *painter, int x, int y, const QString& edge, const QString& weight, bool isInPath);
signals:
    void rootChanged();
    void contentWidthChanged();
    void contentHeightChanged();
private:
    Node *m_Root;
    int m_ContentWidth, m_ContentHeight;
    int m_RootX;
};

