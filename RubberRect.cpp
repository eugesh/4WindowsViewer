#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "RubberRect.h"


RubberRect::RubberRect(QRectF RectF, QRectF maxRectF, QPointF Ctr, int th, QGraphicsItem *parent): QGraphicsItem(parent)
{
    Center = Ctr;
    max_imgRect = maxRectF;
    // sizeF_max = RectF.size();
    thickness = th;
    bchanges = false;
    m_editing = false;
    bmove = false;
    corner = 0;
    coord1 = RectF.topLeft();
    coord2 = RectF.topRight();
    coord3 = RectF.bottomRight();
    coord4 = RectF.bottomLeft();
    // sizeF = QRectF(coord1, coord3).size();
    m_OutlineColor = Qt::darkGreen;
    setFlags(ItemIsMovable | ItemIsSelectable);
    // setFlag(ItemIgnoresTransformations,true);
}


void RubberRect::setCenter(QPointF C) {
   Center = C;
   qreal sizeX = boundingRect().width(); // sizeF.width ();
   qreal sizeY = boundingRect().height(); // sizeF.height();
   change_coord(QPointF(Center.x() - sizeX / 2, Center.y() - sizeY / 2), 1);
   change_coord(QPointF(Center.x() + sizeX / 2, Center.y() - sizeY / 2), 2);
   change_coord(QPointF(Center.x() + sizeX / 2, Center.y() + sizeY / 2), 3);
   change_coord(QPointF(Center.x() - sizeX / 2, Center.y() + sizeY / 2), 4);
}


/*void RubberRect::SetSize(int size) {
   change_coord(QPointF(Center.x() - size / 2, Center.y() - size / 2), 1);
   change_coord(QPointF(Center.x() + size / 2, Center.y() - size / 2), 2);
   change_coord(QPointF(Center.x() + size / 2, Center.y() + size / 2), 3);
   change_coord(QPointF(Center.x() - size / 2, Center.y() + size / 2), 4);
   // sizeF = QSizeF(size, size);
}*/


void RubberRect::setP1(QPointF newp1){
    if (newp1 != coord1) {
        prepareGeometryChange();
        coord1 = newp1;
        // coord4.setX(coord1.x());
        // coord2.setY(coord1.y());
        // sizeF = QRectF(coord1, coord3).size();
        update();
    }
}

void RubberRect::setP2(QPointF newp2){
     if (newp2 != coord2) {
         prepareGeometryChange();
         coord2 = newp2;
         // coord3.setX(coord2.x());
         // coord1.setY(coord2.y());
         // sizeF = QRectF(coord1, coord3).size();
         update();
     }
}

void RubberRect::setP3(QPointF newp3){
     if (newp3 != coord3) {
         prepareGeometryChange();
         coord3 = newp3;
         // coord2.setX(coord3.x());
         // coord4.setY(coord3.y());
         // sizeF = QRectF(coord1, coord3).size();
         update();
     }
}

void RubberRect::setP4(QPointF newp4) {
     if (newp4 != coord4) {
         prepareGeometryChange();
         coord4 = newp4;
         // coord1.setX(coord4.x());
         // coord3.setY(coord4.y());
         // sizeF = QRectF(coord1, coord3).size();
         update();
     }
}

int RubberRect::changed_corner( QPointF pos ) {
    corner = 0;
    QRectF rect1(coord1, QPointF(coord1.x() + H_CORNER * thickness, coord1.y() + H_CORNER * thickness));
    QRectF rect2(QPointF(coord2.x() - H_CORNER * thickness, coord2.y()),
                 QPointF(coord2.x(), coord2.y() + H_CORNER * thickness));
    QRectF rect3(QPointF(coord3.x() - H_CORNER * thickness, coord3.y() - H_CORNER * thickness), coord3);
    QRectF rect4(QPointF(coord4.x(), coord4.y() - H_CORNER * thickness),
                 QPointF(coord4.x() + H_CORNER * thickness, coord4.y()));

    if (boundingRect().contains(pos))
        corner = 5;
    if (rect1.contains(pos))
        corner = 1;   //topleft
    if (rect2.contains(pos))
        corner = 2;   //topright
    if (rect3.contains(pos))
        corner = 3;   //bottomright
    if (rect4.contains(pos))
        corner = 4;   //bottomleft

    return corner;
}

bool RubberRect::change_coord(QPointF point, int corner) {
    bool is_changed = 0;

    switch(corner) {
    case 1:
        if (max_imgRect.topLeft().x() > point.x()) //отслеживается выход
            point.setX(max_imgRect.topLeft().x()); //за границу растра
        if (max_imgRect.topLeft().y() > point.y())
            point.setY(max_imgRect.topLeft().y());
        setP1(point);
    break;
    case 2:
        if (max_imgRect.topRight().x() < point.x())
            point.setX(max_imgRect.topRight().x());
        if (max_imgRect.topRight().y() > point.y())
            point.setY(max_imgRect.topRight().y());
        setP2(point);
    break;
    case 3:
        if (max_imgRect.bottomRight().x() < point.x())
            point.setX(max_imgRect.bottomRight().x());
        if (max_imgRect.bottomRight().y() < point.y())
            point.setY(max_imgRect.bottomRight().y());
        setP3(point);
    break;
    case 4:
        if (max_imgRect.bottomLeft().x() > point.x())
            point.setX(max_imgRect.bottomLeft().x());
        if (max_imgRect.bottomLeft().y() < point.y())
            point.setY(max_imgRect.bottomLeft().y());
        setP4(point);
    break;
    }

    return is_changed;
}

QRectF RubberRect::boundingRect() const
{
    double right, left, top, bottom;

    right = std::max(coord2.x(), coord3.x());
    left = std::min(coord1.x(), coord4.x());
    top = std::min(coord1.y(), coord2.y());
    bottom = std::max(coord3.y(), coord4.y());

    return QRectF(QPointF(left, top), QPointF(right, bottom));
}

void RubberRect::mousePressEvent (QGraphicsSceneMouseEvent * event)
{   
    if (m_editing) {
        corner = changed_corner(event->pos());
        if (corner < 5 && corner > 0){
            change_coord(event->pos(), corner);
            bchanges = true;
        }
        if (corner == 5) {
            bmove = true;
        }
    }
    parentItem()->scene()->update();
}

void RubberRect::mouseMoveEvent (QGraphicsSceneMouseEvent * event)
{
    if (bchanges) {
        change_coord(event->pos(), corner);
    }

    if (bmove) {
        // moveBy (event->pos().x()-origin.x(),event->pos().y()-origin.y());
    }

    parentItem()->scene()->update();
}

void RubberRect::mouseReleaseEvent (QGraphicsSceneMouseEvent * event) {
    bchanges = false;
    bmove = false;
    // parentItem()->scene()->update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void RubberRect::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *,
                       QWidget *)
{
    QPen pen(m_OutlineColor);
    pen.setWidth(2 * thickness);
    painter->setPen(pen);
    // QRectF rect(coord1, coord3); //= outlineRect();
    // painter->drawRect(rect);

    QLineF line = QLineF(coord1, coord2);
    painter->drawLine(line);

    line = QLineF(coord2, coord3);
    painter->drawLine(line);

    line = QLineF(coord3, coord4);
    painter->drawLine(line);

    line = QLineF(coord1, coord4);
    painter->drawLine(line);

    pen.setWidth(thickness);
    painter->setPen(pen);
    QRectF rect1(coord1, QPointF(coord1.x() + H_CORNER * thickness, coord1.y() + H_CORNER * thickness));
    painter->drawRect(rect1);

    QRectF rect2(QPointF(coord2.x() - H_CORNER * thickness, coord2.y()),
                 QPointF(coord2.x(), coord2.y() + H_CORNER * thickness));
    painter->drawRect(rect2);

    QRectF rect3(QPointF(coord3.x() - H_CORNER * thickness, coord3.y() - H_CORNER * thickness), coord3);
    painter->drawRect(rect3);

    QRectF rect4(QPointF(coord4.x(), coord4.y() - H_CORNER * thickness),
                 QPointF(coord4.x() + H_CORNER * thickness, coord4.y()));
    painter->drawRect(rect4);
}

/*
QVariant Node::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        foreach (Link *link, myLinks)
            link->trackNodes();
    }
    return QGraphicsItem::itemChange(change, value);
}
*/
/*QRectF Krest::outlineRect() const
{
    const int Padding = 50;
    QFontMetricsF metrics = qApp->font();
    QRectF rect = metrics.boundingRect("+");
//    QRectF rect = metrics.boundingRect(rect1,Qt::AlignRight,myText);
    rect.adjust(-Padding, -Padding, +Padding, +Padding);
//    rect.translate(-rect.center());
    rect.translate(-3.2,4.65);
    return rect;
      //рисуем крестик
  //  painter->setBrush(myKrestColor);
 //   QLine vline()
//    QRectF rect = metrics.boundingRect(myText);
}
*/
