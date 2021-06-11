#include <QRectF>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "image-item.h"

constexpr int DEBUG = 1;

ImageItem::ImageItem()
{
    setZValue(100);

    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptHoverEvents(true);
}

ImageItem::~ImageItem() {

}

QRectF
ImageItem::boundingRect() const {
    return QRectF(QPointF(0, 0), m_image_part2draw.size());
}

/*QPainterPath
ImageItem::shape() const
{
    QPainterPath path;
    path.addRect(14, 14, 82, 42);
    return path;
}*/

void
ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget)
    Q_UNUSED(option)
    // painter->drawImage(QPoint(0,0), m_image_part2draw);
    if (! m_inUpdateProcess) {
        m_inUpdateProcess = true;
        Qt::ImageConversionFlag flag = Qt::ImageConversionFlag::AutoColor;
        /*if (m_image_part2draw.format() == QImage::Format_Indexed8) {
            flag = Qt::ImageConversionFlag::MonoOnly;
            m_image_part2draw.save("tmp/outGray.png");
        }*/
        QImage img = m_image_part2draw.convertToFormat(QImage::Format_ARGB32, Qt::ImageConversionFlag::ColorOnly);
        if (DEBUG) img.save("tmp/befor_draw.png");
        painter->drawImage(cur_pose, img, boundingRect(), flag);
    }
    m_inUpdateProcess = false;
}

QImage ImageItem::setFiltered(const QImage &img)
{
    m_image_part2draw = img;
    scene()->update();
    return m_image_part2draw;
}

void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}

void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        cur_pose = event->pos();
        update();
        return;
    }
    scene()->update();
    QGraphicsItem::mouseMoveEvent(event);
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}
