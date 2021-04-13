#include "treedrawer.h"

#include <QPainter>

const int rectWidth = 100;
const int rectHeight = 80;
const int verticalSpacing = 30;
const int horizontalSpacing = 20;

QString getPairText(const QPair<int, int> pair) {
    return "(" + QString::number(pair.first + 1) + ", " + QString::number(pair.second + 1) + ")";
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
    drawNode(painter, m_RootX, rectY, "", QString::number(m_Root->getWeight()), m_Root->isInPath());

    int leftRectX = m_RootX - rectWidth / 2  - horizontalSpacing;
    int rightRectX = m_RootX + rectWidth / 2 + horizontalSpacing;
    int parentX = m_RootX;

    Node *root = m_Root;

    while(root) {
        if(!root->getLeftChild() && !root->getRightChild()) {
            break;
        }
        rectY += rectHeight + verticalSpacing;

        //draw childrens
        drawNode(painter, leftRectX, rectY, getPairText(root->getLeftChild()->getLastIncludedEdge()),
                 QString::number(root->getLeftChild()->getWeight()), root->getLeftChild()->isInPath());
        drawNode(painter, rightRectX, rectY, "!" + getPairText(root->getRightChild()->getLastExcludedEdge()),
                 QString::number(root->getRightChild()->getWeight()), root->getRightChild()->isInPath());

        //draw edges
        painter->drawLine(QPointF(leftRectX + rectWidth / 2, rectY), QPointF(parentX + rectWidth / 2, rectY - verticalSpacing));
        painter->drawLine(QPointF(rightRectX + rectWidth / 2, rectY), QPointF(parentX + rectWidth / 2, rectY - verticalSpacing));

        if(root->getLeftChild()->getLeftChild()) {
            parentX = leftRectX;
            leftRectX = leftRectX - (rectWidth / 2 + horizontalSpacing);
            rightRectX = leftRectX + rectWidth + horizontalSpacing;
            root = root->getLeftChild();

        } else {
            parentX = rightRectX;
            leftRectX = rightRectX - rectWidth / 2 - horizontalSpacing;
            rightRectX += rectWidth / 2 + horizontalSpacing;
            root = root->getRightChild();
        }
        level++;
    }
}

Node *TreeDrawer::getRoot() const {
    return m_Root;
}

void TreeDrawer::setRoot(Node *root) {
    m_Root = root;

    // find level with min X at left node
    int rectX = 0;
    int leftRectX = rectX - rectWidth / 2  - horizontalSpacing;
    int rightRectX = rectX + rectWidth / 2 + horizontalSpacing;
    int rectY = 50;

    int maxWidth = rightRectX;
    while(root) {
        rectY += rectHeight + verticalSpacing;
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

        if(!root->getLeftChild() && !root->getRightChild()) {
            break;
        }
        if(root->getLeftChild()->getLeftChild()) {
            root = root->getLeftChild();
            leftRectX = leftRectX - (rectWidth / 2 + horizontalSpacing);
            rightRectX = leftRectX + rectWidth + horizontalSpacing;
        } else {
            leftRectX = rightRectX - rectWidth / 2 - horizontalSpacing;
            rightRectX += rectWidth / 2 + horizontalSpacing;
            root = root->getRightChild();
        }
    }
    m_RootX = rectX;
    setHeight(rectY + rectHeight + horizontalSpacing);
    setWidth(maxWidth + rectWidth + horizontalSpacing);
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

void TreeDrawer::drawNode(QPainter *painter, int x, int y, const QString &edge, const QString &weight, bool isInPath) {
    if(isInPath) {
        painter->setPen(QPen(Qt::GlobalColor::green, 2, Qt::SolidLine, Qt::RoundCap));
    }
    painter->drawRect(x, y, rectWidth, rectHeight);
    painter->drawLine(x, y + rectHeight / 2, x + rectWidth, y + rectHeight / 2);

    painter->setPen(Qt::black);
    painter->drawText(QRect(x, y, rectWidth, rectHeight / 2), Qt::AlignCenter, edge);
    painter->drawText(QRect(x, y + rectWidth / 2, rectWidth, rectHeight / 2), Qt::AlignCenter, weight);
    painter->setPen(QPen(Qt::GlobalColor::lightGray, 2, Qt::SolidLine, Qt::RoundCap));
}
