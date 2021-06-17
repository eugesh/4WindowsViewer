#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QRectF>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "image-item.h"

constexpr int DEBUG = 0;

ImageItem::ImageItem() : cur_pose(QPointF(0, 0))
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
    std::cout << "paint"<<std::endl;
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
        // QImage img = m_image_part2draw.convertToFormat(QImage::Format_ARGB32, Qt::ImageConversionFlag::ColorOnly);
        // if (DEBUG) img.save("tmp/befor_draw.png");
        // painter->drawImage(cur_pose, img, boundingRect(), flag);
        if (!QFileInfo::exists(QString("tmp/%1").arg(intptr_t(this)))) {
            //qDebug() << QString("tmp/%1").arg(intptr_t(this));
            //QDir dir(QString("tmp/%1").arg(intptr_t(this)));
            //dir.mkdir(QString("tmp/%1").arg(intptr_t(this)));
            QDir().mkdir(QString("tmp/%1").arg(intptr_t(this)));
        }
        m_pmap = QPixmap::fromImage(m_image_part2draw);
        // if (DEBUG) m_image_part2draw.save("tmp/befor_draw.png");
        if (DEBUG) m_image_part2draw.save(QString("tmp/%1/%2").arg(intptr_t(this)).arg("befor_draw.png"));
        //  if (DEBUG) m_pmap.save("tmp/befor_draw.bmp");
        if (DEBUG) m_pmap.save(QString("tmp/%1/%2").arg(intptr_t(this)).arg("befor_draw.bmp"));
        painter->drawPixmap(cur_pose, m_pmap);
    }
    m_inUpdateProcess = false;
}

QImage ImageItem::setFiltered(const QImage &img)
{
    std::cout << "setFiltered: ";
    std::cout << "old format: " << m_image_part2draw.format();
    std::cout << "new format: " << img.format();
    if (DEBUG) img.save("tmp/img_setFiltered.png");
    m_image_part2draw = img;
    std::cout << "became format: " << m_image_part2draw.format();
    if (DEBUG) m_image_part2draw.save("tmp/m_image_part2draw_setFiltered.png");
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
