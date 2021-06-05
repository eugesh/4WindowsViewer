#ifndef RUBBER_RECT_H
#define RUBBER_RECT_H

#include "imageprocessor.h"

#include <QApplication>
#include <QColor>
#include <QGraphicsItem>
#include <QSet>
#include <iostream>

#define H_CORNER 10

enum CornerType {
    TopLeft = 0,
    TopRight,
    BottomRight,
    BottomLeft,
    WholeRect
};

class RubberRect : public QGraphicsItem
{
public:
    explicit RubberRect(QRectF RectF, QRectF maxRectF, QPointF Ctr, int th = 1, QGraphicsItem *parent = nullptr);

    void setEditable(bool isEdit) { m_editing = isEdit; }
    bool isEditable() { return m_editing; }

    void setP1(QPointF newp1);
    void setP2(QPointF newp2);
    void setP3(QPointF newp3);
    void setP4(QPointF newp4);
    bool change_coord(QPointF point, CornerType corner);
    void setCenter(QPointF C);
    std::vector<QPointF> getPoints() const { return m_vCorners; }
    bool hasAuxLines() { return m_hasAuxLines; }
    void setHasAuxLines(bool has) { m_hasAuxLines = has; }

protected:
    virtual QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    //! Oтработка нажатия на кнопки мыши.
    virtual void  mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    //! Oтработка отпускания кнопки мыши.
    virtual void  mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;
    //! Oтработка сигнала перемещения мыши.
    virtual void  mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    // QVariant itemChange(GraphicsItemChange change,
    //                        const QVariant &value);

private:
    CornerType changed_corner(QPointF pos);
    void paintAuxLines(QPainter *painter);
    QLineF infiniteLine(QPointF p1, QPointF p2);

private:
    QColor m_OutlineColor;
    bool bchanges;
    bool m_editing;
    bool bmove;
    CornerType m_corner;
    int thickness; // Толщина рамки.
    QPointF Center;
    QRectF max_imgRect;
    std::vector<QPointF> m_vCorners;
    // std::vector<geom::PixelPoint> m_vRRPoints;
    bool m_hasAuxLines = false;
};

#endif
