#include "pixelruler.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

constexpr int H_CORNER = 10;

PixelRuler::PixelRuler(QLineF line, QRectF maxRectF, QGraphicsItem *parent) :
    QGraphicsItem(parent), max_imgRect(maxRectF)
{
    setFlags(ItemIsMovable | ItemIsSelectable);
    setLine(line);
}

void PixelRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen(m_OutlineColor);
    pen.setWidth(2 * thickness);
    painter->setPen(pen);

    painter->drawLine(line());

    pen.setWidth(thickness);
    painter->setPen(pen);

    painter->drawEllipse(line().p1(), H_CORNER * thickness, H_CORNER * thickness);
    painter->drawEllipse(line().p2(), H_CORNER * thickness, H_CORNER * thickness);

    QPoint textPoint;
    textPoint.setX(int(line().p2().x()));
    textPoint.setY(int(line().p2().y() - 25));
    QString distanceText = QString("%1").arg(line().length());
    painter->drawText(textPoint, distanceText);
}

QRectF PixelRuler::boundingRect() const
{
    double right, left, top, bottom;

    right = std::max(line().p1().x(), line().p2().x())  + H_CORNER * thickness;
    left = std::min(line().p1().x(), line().p2().x()) - H_CORNER * thickness;
    top = std::min(line().p1().y(), line().p2().y()) - H_CORNER * thickness;
    bottom = std::max(line().p1().y(), line().p2().y()) + H_CORNER * thickness;

    return QRectF(QPointF(left, top), QPointF(right, bottom));
}

void PixelRuler::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_editing) {
        m_corner = changed_corner(event->pos());
        if (m_corner <= EndPoint && m_corner >= StartPoint){
            change_coord(event->pos(), m_corner);
            bchanges = true;
        }
        if (m_corner == WholeRuler) {
            bmove = true;
        }
    }
    parentItem()->scene()->update();
}

void PixelRuler::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (bchanges) {
        change_coord(event->pos(), m_corner);
    }

    if (bmove) {
        // moveBy (event->pos().x()-origin.x(),event->pos().y()-origin.y());
    }

    parentItem()->scene()->update();
}

void PixelRuler::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    bchanges = false;
    bmove = false;
    // parentItem()->scene()->update();
    QGraphicsItem::mouseReleaseEvent(event);
}

PointType PixelRuler::changed_corner(const QPointF &pos) {
    if (boundingRect().contains(pos))
        m_corner = WholeRuler;

    QRectF rect1(QPointF(line().p1().x() - H_CORNER * thickness, line().p1().y() - H_CORNER * thickness),
                 QPointF(line().p1().x() + H_CORNER * thickness, line().p1().y() + H_CORNER * thickness));

    QRectF rect2(QPointF(line().p2().x() - H_CORNER * thickness, line().p2().y() - H_CORNER * thickness),
                 QPointF(line().p2().x() + H_CORNER * thickness, line().p2().y() + H_CORNER * thickness));

    if (rect1.contains(pos))
        m_corner = StartPoint;
        //return StartPoint;

    if (rect2.contains(pos))
        m_corner = EndPoint;
        //return EndPoint;

    return m_corner;
}

bool PixelRuler::change_coord(QPointF point, PointType corner)
{
    bool is_changed = 0;

    QLineF lineF = line();

    switch(corner) {
    case StartPoint:
        /*if (max_imgRect.topLeft().x() > point.x()) //отслеживается выход
            point.setX(max_imgRect.topLeft().x()); //за границу растра
        if (max_imgRect.topLeft().y() > point.y())
            point.setY(max_imgRect.topLeft().y());*/
        lineF.setP1(point);
    break;
    case EndPoint:
        /*if (max_imgRect.topRight().x() < point.x())
            point.setX(max_imgRect.topRight().x());
        if (max_imgRect.topRight().y() > point.y())
            point.setY(max_imgRect.topRight().y());*/
        lineF.setP2(point);
    break;
    case MiddlePoint:
    case WholeRuler:
        break;
    }

    setLine(lineF);

    //m_vRRPoints[corner] = {point};

    return is_changed;
}
