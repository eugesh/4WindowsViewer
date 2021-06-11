#ifndef IMAGEPROCESSINGCOMMON_H
#define IMAGEPROCESSINGCOMMON_H

#include <QString>

namespace geom {

enum ColorSpace {
    RGB,
    HSV,
    HSL,
    HSI
};

enum ColorName {
    gray=0,
    red,
    green,
    blue,
    hue,
    saturation,
    value,
    lightness,
    intensity
};

}

//QString textFromColorSpace(geom::ColorSpace s);
//QString textFromColorName(geom::ColorName c);



#endif // IMAGEPROCESSINGCOMMON_H
