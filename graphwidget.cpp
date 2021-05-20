#include "graphwidget.h"

#include <math.h>


GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(500, 300);

    run();
}

void GraphWidget::run()
{

    QFile file("/home/veu/test2.xml");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"error";
       return;
    }
    QByteArray data = file.readAll();
    QDomDocument doc;
    if (!doc.setContent(data))
            return;
    QDomElement root = doc.documentElement();
    parseXml(nullptr, root);
}

int GraphWidget::getXStartPos(QDomElement root, int xpos)
{
    return xpos - (root.childNodes().count() * 50) - 30;
}

int GraphWidget::calculateX(int i, int count) {
    int x = count * 150;
    return (x / count * i + 30);
}

int GraphWidget::countChild(QDomElement root) {
    int count = 0;
    for (int i = 0; i < root.childNodes().count(); i++)
    {
        QDomElement item = root.childNodes().at(i).toElement();
        count += item.childNodes().count();
    }
    return count;
}


void GraphWidget::parseXml(QRectF* parent, QDomElement root)
{
    QRectF rootNode = getRect(root.attribute("name"), calculateX(0, 1), 0);
    parent = &rootNode;
    int numChild = countChild(root);
    int iChild = 0;
    for (int i = 0; i < root.childNodes().count(); i++)
    {
        QDomElement item = root.childNodes().at(i).toElement();
        QRectF itemNode = getRect(item.attribute("name"), calculateX(i, root.childNodes().count()), 100);
        scene->addRect(itemNode);
        scene->addLine(*getLineBetweenRects(parent, itemNode));
        QRectF* parentItem = &itemNode;
        for (int j = 0; j < item.childNodes().count(); j++)
        {
                QDomElement subItem = item.childNodes().at(j).toElement();
                QRectF subItemNode = getRect(subItem.attribute("name"), calculateX(iChild++, numChild), 200);
                scene->addRect(subItemNode);
                scene->addLine(*getLineBetweenRects(parentItem, subItemNode));
                QRectF* parentSubItem = &itemNode;
                for (int v = 0; v < subItem.childNodes().count(); v++)
                {
                    QDomElement subSubItem = subItem.childNodes().at(v).toElement();
                    QRectF subsubItemNode = getRect(subSubItem.attribute("name"), calculateX(v, subSubItem.childNodes().count()), 300);
                    scene->addRect(subsubItemNode);
                    scene->addLine(*getLineBetweenRects(parentSubItem, subsubItemNode));
                }
            }

        }

}

QLineF* GraphWidget::getLineBetweenRects(QRectF* parent, QRectF rect)
{
    if (parent != nullptr)
    {
        QPoint p1;
        p1.setX((rect.topRight().x() + rect.topLeft().x()) / 2);
        p1.setY(rect.topRight().y());

        QPoint p2;
        p2.setX((parent->bottomLeft().x() + parent->bottomRight().x()) / 2);
        p2.setY(parent->bottomLeft().y());

        QLineF* line = new QLineF(p2, p1);
        double angle = ::acos(line->dx() / line->length());
        if (line->dy() >= 0)
            angle = 3.14*2 - angle;
        QPointF destArrowP1 = p1 + QPointF(sin(angle - 3.14 / 3) * 10,
                                                       cos(angle - 3.14 / 3) * 10);
        QPointF destArrowP2 = p1 + QPointF(sin(angle - 3.14 + 3.14 / 3) * 10,
                     cos(angle - 3.14 + 3.14 / 3) * 10);
        scene->addPolygon(QPolygonF() << line->p2() << destArrowP1 << destArrowP2);
        return line;
    }
    return nullptr;
}

QRectF GraphWidget::getRect(QString text, int xpos, int ypos)
{
    QGraphicsTextItem* textItem = scene->addText(text);
    textItem->setTextWidth(100);
    textItem->setPos(xpos, ypos);

    QRectF rect = textItem->boundingRect();
    rect.moveTo(xpos, ypos);
    return rect;
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow(2., -event->angleDelta().y() / 240.0));
}

void GraphWidget::scaleView(int scaleFactor)
{
    int factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton)
    {
        rightMousePressed = true;
        _panStartX = event->pos().x();
        _panStartY = event->pos().y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton)
    {
        rightMousePressed = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    event->ignore();
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event){
    if (rightMousePressed)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->pos().x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->pos().y() - _panStartY));
        _panStartX = event->pos().x();
        _panStartY = event->pos().y();
        event->accept();
        return;
    }
    event->ignore();

}
