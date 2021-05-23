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
        qDebug()<<"Error open";
       return;
    }
    QByteArray data = file.readAll();
    QDomDocument doc;
    if (!doc.setContent(data))
            return;
    QDomElement root = doc.documentElement();
    readRootElement(root);
}

int GraphWidget::calculateX(int index) {
    return (150 * index + 30);
}

void GraphWidget::countLevel(QDomElement root) {
    size++;
    for (int i = 0; i < root.childNodes().count(); i++)
    {
        QDomElement item = root.childNodes().at(i).toElement();
        if (item.childNodes().count() > 0)
            countLevel(item);
    }
}

void GraphWidget::parseXmlAndDraw(QRectF* parent, QDomElement root, int arrayCountChild[], int level)
{
    y += 100;
    for (int j = 0; j < root.childNodes().count(); j++)
    {
        QDomElement item = root.childNodes().at(j).toElement();
        QRectF itemNode = getRect(item.attribute("name"), calculateX(arrayCountChild[level]), 100 * (level + 1));
        arrayCountChild[level]++;
        scene->addRect(itemNode);
        scene->addLine(*getLineBetweenRects(parent, itemNode));
        if (item.childNodes().count() > 0) {
            QRectF* parentItem = &itemNode;
            parseXmlAndDraw(parentItem, item, arrayCountChild, level + 1);
        }
    }
}


void GraphWidget::readRootElement(QDomElement root)
{
    QRectF rootNode = getRect(root.attribute("name"), calculateX(0), 0);
    scene->addRect(rootNode);
    QRectF* parent = &rootNode;
    countLevel(root);
    int arrayCountChild[size];
    for (int i = 0; i < size; i++) {
        arrayCountChild[i] = 0;
    }
    parseXmlAndDraw(parent, root, arrayCountChild, 0);
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
