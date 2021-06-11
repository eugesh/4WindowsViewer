#include "imageprocessor.h"

/*ImageProcessor::ImageProcessor()
{

}*/
namespace geom {

inline unsigned char fitIn255(int val) {
    if (val > 255) val = 255;
    if (val < 0) val = 0;
    return static_cast<unsigned char>(val);
}

QImage convertTo8(const QImage &InImg)
{
    QImage::Format format = QImage::Format_Grayscale8;
/*#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    QImage curImg (InImg.size(), QImage::Format_Grayscale8);
#else*/
    format = QImage::Format_Indexed8;
    QImage curImg(InImg.size(), QImage::Format_Indexed8);

    static QVector <QRgb> color_table;
    if (color_table.empty()) {
        for (int k = 0; k < 256; k++)
            color_table << qRgb(k, k, k);
    }
    curImg.setColorTable(color_table);
//#endif

    if (InImg.format() != format) {
        for (int i = 0; i < InImg.height(); ++i)
            for (int j = 0; j < InImg.width(); ++j) {
                //cout << " x, j = " << j << "y, i = " << i << "qGray( InImg.pixel(j,i) ) = " << qGray( InImg.pixel(j,i) ) << endl ;
                curImg.setPixel(j, i, fitIn255(qGray(InImg.pixel(j, i)))) ;
            }
      //QVector < QRgb > color_table ;
      //for( int k = 0 ; k < 256 ; k++ ) color_table << qRgb( k, k, k ) ;
      //curImg = curImg.convertToFormat( QImage::Format_Indexed8, color_table ) ;
    } else {
        curImg = InImg;
    }

    return curImg;
}

QImage getChannel(const QImage &InImg, ColorSpace cs, int ch)
{
 /*#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    QImage curImg(InImg.size(), QImage::Format_Grayscale8);
    curImg.setColorCount(256);
 #else*/
    QImage curImg (InImg.size(), QImage::Format_Indexed8);

    static QVector<QRgb> color_table;
    if (color_table.empty()) {
        for (int k = 0; k < 256; k++)
            color_table << qRgb(k, k, k);
    }
    curImg.setColorTable(color_table);
//#endif

    for (int i = 0; i < InImg.height(); ++i) {
        for (int j = 0; j < InImg.width(); ++j) {
            int val[3];
            switch (cs) {
            case RGB:
                InImg.pixelColor(j, i).getRgb(&val[0], &val[1], &val[2]);
                break;
            case HSV:
                InImg.pixelColor(j, i).getHsv(&val[0], &val[1], &val[2]);
                break;
            case HSI:
                break;
            case HSL:
                break;
            }
            curImg.setPixel(j, i, fitIn255(val[ch]));
        }
    }

    return curImg;
}

QImage getBWThresholded()
{

}

QImage getChannel(const QImage & InImg, ColorName chName)
{
/*#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
   QImage curImg (InImg.size(), QImage::Format_Grayscale8);
   curImg.setColorCount(256);
#else*/
    QImage curImg(InImg.size(), QImage::Format_Indexed8);

    static QVector <QRgb> color_table;
    if (color_table.empty()) {
        for (int k = 0; k < 256; k++)
            color_table << qRgb(k, k, k);
    }
    curImg.setColorTable(color_table);
//#endif

    for (int i = 0; i < InImg.height(); ++i) {
        for (int j = 0; j < InImg.width(); ++j) {
            int val = 0;
            switch (chName) {
            case gray:
                val = qGray(InImg.pixel(j, i));
                break;
            case red:
                val = qRed(InImg.pixel(j, i));
                break;
            case green:
                val = qGreen(InImg.pixel(j, i));
                break;
            case blue:
                val = qBlue(InImg.pixel(j, i));
                break;
            case hue:
                val = InImg.pixelColor(j, i).hsvHue();
                break;
            case saturation:
                val = InImg.pixelColor(j, i).hsvSaturation();
                break;
            case value:
                val = InImg.pixelColor(j, i).value();
                break;
            case lightness:
                break;
            case intensity:
                break;
            default:
                break;
            }

            curImg.setPixel(j, i, val);
        }
    }

    return curImg;
}

}
