#include <QApplication>
#include <QtGui>
#include <QGraphicsScene>
#include <QInputDialog>
#include <QFontMetrics>
#include <QLineEdit>
#include <QObject>
#include <QString>

#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "pointitem.h"

PointItem::PointItem(const QString &name, quint32 id, QObject *parent_object, QGraphicsItem *parent)
    : QObject(parent_object), QGraphicsItem(parent), m_id(id)
{
    setObjectName(name);
    myTextColor = Qt::darkGreen;
    myOutlineColor = Qt::darkGreen;
    myBackgroundColor = QColor(255, 255, 255, 0);
    myKrestColor = Qt::darkRed;

    setFlags(ItemIsMovable | ItemIsSelectable);
    // setFlag(ItemIgnoresTransformations, true); // Todo: Find another way to not change size on scale change. This way looses pixel coord on image.
}

void PointItem::setText(const QString &text)
{
    prepareGeometryChange( );
    myText = text;
    update( );
}

QString PointItem::text( ) const
{
    return myText;
}

QRectF
PointItem::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

void
PointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

}

void
PointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    m_coord = event->pos();
    parentItem()->scene()->update();
}

void PointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_coord = event->pos();
}

void PointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)

    QPen pen(myOutlineColor);
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(myBackgroundColor);
    QRectF rect = outlineRect();

    painter->drawRoundedRect(rect,25,25);
    painter->setPen(myTextColor);
    painter->setFont(QFont("Times",25,1));
    painter->drawText(rect,QTextOption::LeftTab,myText);
    QVector<QPointF> pointPairs;
    pointPairs << QPointF (m_coord.x(), m_coord.y() + 25) << QPointF (m_coord.x(), m_coord.y() - 25);
    painter->drawLines(pointPairs);
    pointPairs.clear ();
    pointPairs << QPointF (m_coord.x() + 25, m_coord.y()) << QPointF (m_coord.x() - 25, m_coord.y());
    painter->drawLines(pointPairs);
    //! Cохранение координат
    //coord = this->pos();
}

void PointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QString text = QInputDialog::getText(event->widget(),
                           QObject::tr("Text change"), QT_TR_NOOP("Input new text:"),
                           QLineEdit::Normal, myText);
    if (!text.isEmpty()) {
        setText(text);
        emit textChanged(text);
    }
}

QRectF PointItem::outlineRect() const
{
    const int Padding = 50;
    QFontMetricsF metrics = qApp->fontMetrics();
    // QRectF rect = metrics.boundingRect("+");
    QRectF rect = QRectF(m_coord - QPoint(50, 50), m_coord + QPoint(50, 50));
    rect.adjust(-Padding, -Padding, +Padding, +Padding);
    // rect.translate(-3.2, 4.65);
    return rect;
}
