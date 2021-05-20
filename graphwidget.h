#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QDomNode>
#include <QMessageBox>
#include <QDomElement>
#include <QKeyEvent>
#include <QGraphicsTextItem>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include <QLine>
#include <QXmlStreamReader>

class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:

    GraphWidget(QWidget *parent = nullptr);

    void parseXml(QRectF* par_node, QDomElement root);
    void run();
    void read();
    void processRates();
    void processRate();
    QString readNextText();

    QString getStringJson(QJsonObject jobj, QString key);
    QRectF getRect(QString text, int xpos, int ypos);
    int calculateX(int i, int count);
    int countChild(QDomElement root);
    QLineF* getLineBetweenRects(QRectF* parent, QRectF rect);
    int getXStartPos(QDomElement root, int xpos);

    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

protected:
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void scaleView(int scaleFactor);

private:
    QGraphicsScene* scene;

    QString _filename;
    bool rightMousePressed = false;
    int _panStartY = 0;
    int _panStartX = 0;
};

#endif
