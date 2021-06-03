#ifndef PIXELRULER_H
#define PIXELRULER_H

#include <QGraphicsLineItem>

enum PointType {
    StartPoint = 0,
    EndPoint,
    MiddlePoint, // ToDo
    WholeRuler // ToDo
};

class PixelRuler : public QGraphicsItem
{
public:
    PixelRuler(QLineF line, QRectF maxRectF, QGraphicsItem *parent = nullptr);

    void setEditable(bool isEdit) { m_editing = isEdit; }
    bool isEditable() { return m_editing; }
    QLineF line() const { return m_line; }
    void setLine(QLineF line) { m_line = line; }

protected:
    QRectF boundingRect() const override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    PointType changed_corner(const QPointF &pos);
    bool change_coord(QPointF point, PointType corner);
    // void setPoint(QPointF newPoint);

/*signals:
    void finishEdit(double length);*/
private:
    QLineF m_line;
    QColor m_OutlineColor = Qt::red;
    bool bchanges = false;
    bool m_editing = false;
    bool bmove = false;
    PointType m_corner = WholeRuler;
    int thickness = 1; // Толщина рамки.
    // QPointF Center;
    QRectF max_imgRect;
    // std::vector<QPointF> m_vCorners;
};

#endif // PIXELRULER_H
