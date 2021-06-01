#ifndef IMAGEPROCESSINGCOMMON_H
#define IMAGEPROCESSINGCOMMON_H

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

#endif // IMAGEPROCESSINGCOMMON_H
