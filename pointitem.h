#ifndef POINTITEM_H
#define POINTITEM_H

#include <QColor>
#include <QGraphicsItem>
#include <QObject>


class PointItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 1 };
    PointItem(const QString &name="ControlPoint", quint32 id = 0, QObject *parent_object = nullptr, QGraphicsItem *parent = nullptr);

    QPointF coord() const { return mapToItem(parentItem(), m_coord); }
    QString text() const;
    quint32 id() const { return m_id; }
    int type() const override { return Type; } // Enable the use of qgraphicsitem_cast with this item.

protected:
    QRectF boundingRect() const override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void textChanged(const QString &text);

public slots:
    void setText(const QString &text);

private:
    QRectF outlineRect() const;
    quint32 m_id;
    QPointF m_coord;
    QString myText;
    QColor myTextColor;
    QColor myBackgroundColor;
    QColor myOutlineColor;
    QColor myKrestColor;
};

#endif // POINTITEM_H
