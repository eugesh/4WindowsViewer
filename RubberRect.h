#ifndef RUBBER_RECT_H
#define RUBBER_RECT_H

#include <QApplication>
#include <QColor>
#include <QGraphicsItem>
#include <QSet>
#include <iostream>
#define H_CORNER 10


class RubberRect : public QGraphicsItem
{
public:
    explicit RubberRect(QRectF RectF, QRectF maxRectF, QPointF Ctr, int th = 1, QGraphicsItem *parent = nullptr);

    void setEditable(bool isEdit) { m_editing = isEdit; }
    bool isEditable() { return m_editing; }

    int  changed_corner(QPointF pos);
    void SetSize(int size);
    void setP1(QPointF newp1);
    void setP2(QPointF newp2);
    void setP3(QPointF newp3);
    void setP4(QPointF newp4);
    bool change_coord(QPointF point, int corner);
    virtual QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setCenter(QPointF C);

protected:
    //! Oтработка нажатия на кнопки мыши.
    virtual void  mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    //! Oтработка отпускания кнопки мыши.
    virtual void  mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;
    //! Oтработка сигнала перемещения мыши.
    virtual void  mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    // QVariant itemChange(GraphicsItemChange change,
    //                        const QVariant &value);

private:
    QColor m_OutlineColor;
    bool bchanges;
    bool m_editing;
    bool bmove;
    int corner;
    int thickness; // Толщина рамки.
    QPointF Center;
    QRectF max_imgRect;
    QSizeF sizeF;
    QPointF coord1, coord2, coord3, coord4;
};

#endif
