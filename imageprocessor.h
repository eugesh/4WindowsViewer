#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "image-processing-common.h"
#include <QImage>

namespace geom {

struct PixelPoint {
    PixelPoint(const QPoint &p) { x = p.x(); y = p.y(); }
    PixelPoint(const QPointF &p) { x = p.x(); y = p.y(); }
    double x;
    double y;
};

QImage getChannel(const QImage &InImg, ColorSpace cs, int ch);
QImage getChannel(const QImage &InImg, ColorName chName);
QImage convertTo8(const QImage &InImg);

}

/*class ImageProcessor
{
public:
    explicit ImageProcessor();
};*/

#endif // IMAGEPROCESSOR_H
