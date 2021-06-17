#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <opencv/cv.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/stereo.hpp>
#include <opencv2/calib3d.hpp>

class ImageItem : public QGraphicsItem
{
public:
    ImageItem();
    ~ImageItem() override;

    QRectF boundingRect() const override;
    //QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // temporal method.
    void setImage(const QImage &img) {
        m_image = img;
        m_image_part2draw = img;
    }
    QImage setFiltered(const QImage &img);
    QImage getImage() const { return m_image; }
    QImage getFiltered() const { return m_image_part2draw; }
    // QImage applyFilterOnImage();
    // QImage applyFilterOnFiltered();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // Original
    QImage m_image;
    // Filtered
    QImage m_image_part2draw;
    // Temporal storage for OpenCV functions
    cv::Mat m_mat;
    QPixmap m_pmap;
    QPointF cur_pose;
    bool m_inUpdateProcess = false;
};

#endif // IMAGEITEM_H
