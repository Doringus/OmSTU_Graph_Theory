#include "treedrawer.h"

#include <QPainter>

const int rectWidth = 100;
const int rectHeight = 80;

QString getPairText(const QPair<int, int> pair) {
    return "(" + QString::number(pair.first) + ", " + QString::number(pair.second) + ")";
}

TreeDrawer::TreeDrawer(QQuickPaintedItem *parent) : QQuickPaintedItem(parent) ,
                                                    m_Root(nullptr) {

}

void TreeDrawer::paint(QPainter *painter) {
    if(!m_Root) {
        return;
    }

    painter->setRenderHints(QPainter::Antialiasing, true);
    QFont font = painter->font();
    font.setFamily("Arial");
    font.setPixelSize(20);
    painter->setFont(font);


    int level = 1;
    int rectY = 50;
    // Draw root node
    painter->setPen(QPen(Qt::GlobalColor::lightGray, 2, Qt::SolidLine, Qt::RoundCap));
    painter->drawRect(m_RootX, rectY, rectWidth, rectHeight);

    int leftRectX = m_RootX - rectWidth / 2  - 20;
    int rightRectX = m_RootX + rectWidth / 2 + 20;
    int parentX = m_RootX;

    node_t *root = m_Root;

    int maxWidth = rightRectX;
    while(root) {
        if(!root->left && !root->right) {
            break;
        }
        rectY += rectHeight + 30;

        //draw childrens
        painter->drawRect(leftRectX, rectY, rectWidth, rectHeight);
        painter->drawRect(rightRectX, rectY, rectWidth, rectHeight);

        //draw edges
        painter->drawLine(QPointF(leftRectX + rectWidth / 2, rectY), QPointF(parentX + rectWidth / 2, rectY - 30));
        painter->drawLine(QPointF(rightRectX + rectWidth / 2, rectY), QPointF(parentX + rectWidth / 2, rectY - 30));

        // draw node text
        painter->setPen(Qt::black);
        painter->drawText(QRect(leftRectX, rectY, rectWidth, rectHeight), Qt::AlignCenter, getPairText(root->left->includedEdges.last()));
        painter->drawText(QRect(rightRectX, rectY, rectWidth, rectHeight), Qt::AlignCenter, "!" + getPairText(root->right->excludedEdges.last()));

        if(root->left->left) {
            parentX = leftRectX;
            leftRectX = leftRectX - (rectWidth / 2 + 20);
            rightRectX = leftRectX + rectWidth + 20;
            root = root->left;

        } else {
            parentX = rightRectX;
            leftRectX = rightRectX - rectWidth / 2 - 20;
            rightRectX += rectWidth / 2 + 20;
            root = root->right;
        }
        level++;
    }
    qDebug() << maxWidth;
}

node_t *TreeDrawer::getRoot() const {
    return m_Root;
}

void TreeDrawer::setRoot(node_t *root) {
    m_Root = root;

    // find level with min X at left node
    int rectX = 0;
    int leftRectX = rectX - rectWidth / 2  - 20;
    int rightRectX = rectX + rectWidth / 2 + 20;
    int rectY = 50;

    int maxWidth = rightRectX;
    while(root) {
        rectY += rectHeight + 30;
        if(leftRectX <= 0) {
            int offset = 20 - leftRectX;
            rightRectX += offset;
            maxWidth += offset;
            rectX += offset;
            leftRectX = 20;
        }

        if(rightRectX > maxWidth) {
            maxWidth = rightRectX;
        }

        if(!root->left && !root->right) {
            break;
        }
        if(root->left->left) {
            root = root->left;
            leftRectX = leftRectX - (rectWidth / 2 + 20);
            rightRectX = leftRectX + rectWidth + 20;
        } else {
            leftRectX = rightRectX - rectWidth / 2 - 20;
            rightRectX += rectWidth / 2 + 20;
            root = root->right;
        }
    }
    m_RootX = rectX;
    setHeight(rectY + rectHeight + 20);
    setWidth(maxWidth + rectWidth + 20);
    emit rootChanged();
    update();
}

int TreeDrawer::getContentWidth() const {
    return m_ContentWidth;
}

void TreeDrawer::setContentWidth(int width) {
    m_ContentWidth = width;
    emit contentWidthChanged();
}

int TreeDrawer::getContentHeight() const {
    return m_ContentHeight;
}

void TreeDrawer::setContentHeight(int height) {
    m_ContentHeight = height;
    emit contentHeightChanged();
}
