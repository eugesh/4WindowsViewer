#ifndef ASM_OPENCV_H
#define ASM_OPENCV_H

#include <iostream>
#include <QDebug>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QtGlobal>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

constexpr bool DEBUG = 0;

/*
    Endianness
    ---

    Although not totally clear from the docs, some of QImage's formats we use here are
    endian-dependent. For example:

    Little Endian
        QImage::Format_ARGB32 the bytes are ordered:    B G R A
        QImage::Format_RGB32 the bytes are ordered:     B G R (255)
        QImage::Format_RGB888 the bytes are ordered:    R G B

    Big Endian
        QImage::Format_ARGB32 the bytes are ordered:    A R G B
        QImage::Format_RGB32 the bytes are ordered:     (255) R G B
        QImage::Format_RGB888 the bytes are ordered:    R G B

    Notice that Format_RGB888 is the same regardless of endianness. Since OpenCV
    expects (B G R) we need to swap the channels for this format.

    This is why some conversions here swap red and blue and others do not.

    This code assumes little endian. It would be possible to add conversions for
    big endian machines though. If you are using such a machine, please feel free
    to submit a pull request on the GitHub page.
*/
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#error Some of QImage's formats are endian-dependant. This file assumes little endian. See comment at top of header.
#endif

namespace ASM {
    // NOTE: This does not cover all cases - it should be easy to add new ones as required.
    inline QImage cvMatToQImage_pxw(const cv::Mat &inMat) // Pixel wise
    {
        switch (inMat.type())
        {
            // 8-bit, 4 channel
            case CV_8UC4:
            {
                QImage image(inMat.cols, inMat.rows,
                             QImage::Format_ARGB32);

                for (auto i = 0; i < inMat.rows; ++i) {
                    for (auto j = 0; j < inMat.cols; ++j) {
                        cv::Vec4b intensity = inMat.at<cv::Vec4b>(i, j);
                        QColor color;
                        color.setAlpha(intensity[3]);
                        color.setRed(intensity[2]);
                        color.setGreen(intensity[1]);
                        color.setBlue(intensity[0]);
                        image.setPixelColor(j, i, color);
                    }
                }

                return image;
            }

            // 8-bit, 3 channel
            case CV_8UC3:
            {
                QImage image(inMat.cols, inMat.rows, QImage::Format_RGB888);

                for (auto i = 0; i < inMat.rows; ++i) {
                    for (auto j = 0; j < inMat.cols; ++j) {
                        cv::Vec3b intensity = inMat.at<cv::Vec3b>(i, j);
                        QColor color;
                        color.setRed(intensity[2]);
                        color.setGreen(intensity[1]);
                        color.setBlue(intensity[0]);
                        image.setPixelColor(j, i, color);
                    }
                }


                return image; //image.rgbSwapped();
            }

            // 8-bit, 1 channel
            case CV_8UC1:
            {
/*#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
                QImage image(inMat.data,
                             inMat.cols, inMat.rows,
                             static_cast<int>(inMat.step),
                             QImage::Format_Grayscale8);
                image.setColorCount(256);
#else*/
                static QVector<QRgb> sColorTable;

                QImage image(inMat.cols, inMat.rows,
                             QImage::Format_Indexed8);

                // only create our color table the first time
                if (sColorTable.isEmpty()) {
                    sColorTable.resize(256);

                    for (int i = 0; i < 256; ++i) {
                        sColorTable[i] = qRgb(i, i, i);
                    }
                }

                image.setColorTable(sColorTable);

                //memcpy(image.bits(), inMat.data, size_t(inMat.cols * inMat.rows) * sizeof (uchar));

                for (auto i = 0; i < inMat.rows; ++i) {
                    for (auto j = 0; j < inMat.cols; ++j) {
                        image.setPixel(j, i, inMat.at<uchar>(i, j));
                    }
                }
//#endif

                if (DEBUG) image.save("tmp/qimage_Indexed8.png");
                return image;
            }

            default:
                qWarning() << "ASM::cvMatToQImage_pxw() - cv::Mat image type not handled in switch:" << inMat.type();
                break;
        }

        return QImage();
    }

    inline QPixmap cvMatToQPixmap(const cv::Mat &inMat)
    {
        return QPixmap::fromImage(cvMatToQImage_pxw( inMat ));
    }

    // If inImage exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inImage's
    // data with the cv::Mat directly
    //    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
    //    NOTE: This does not cover all cases - it should be easy to add new ones as required.
    inline cv::Mat QImageToCvMat_pxw(const QImage &inImage, bool inCloneImageData = true)
    {
        switch (inImage.format())
        {
            // 8-bit, 4 channel
            case QImage::Format_ARGB32:
            case QImage::Format_ARGB32_Premultiplied:
            {
                cv::Mat mat(inImage.height(), inImage.width(), CV_8UC4);

                for (auto i = 0; i < inImage.height(); ++i) {
                    for (auto j = 0; j < inImage.width(); ++j) {
                        // mat.at<uchar>(i, j) = qGray(inImage.pixel(j, i));
                        mat.at<cv::Vec4b>(i, j)[0] = inImage.pixelColor(j, i).blue();
                        mat.at<cv::Vec4b>(i, j)[1] = inImage.pixelColor(j, i).green();
                        mat.at<cv::Vec4b>(i, j)[2] = inImage.pixelColor(j, i).red();
                        mat.at<cv::Vec4b>(i, j)[3] = inImage.pixelColor(j, i).alpha();
                    }
                }

                if (DEBUG) cv::imwrite("tmp/mat_converted_Format_ARGB32.png", mat);

                return (inCloneImageData ? mat.clone() : mat);
            }

            // 8-bit, 3 channel
            case QImage::Format_RGB32:
            {
                if (!inCloneImageData)
                {
                    qWarning() << "ASM::QImageToCvMat_pxw() - Conversion requires cloning so we don't modify the original QImage data";
                }

                if (DEBUG) inImage.save("tmp/InImage_Format_RGB32.png");

                cv::Mat mat(inImage.height(), inImage.width(), CV_8UC3);

                //cv::Mat matNoAlpha;

                //cv::cvtColor(mat, matNoAlpha, cv::COLOR_BGRA2BGR);   // drop the all-white alpha channel

                for (auto i = 0; i < inImage.height(); ++i) {
                    for (auto j = 0; j < inImage.width(); ++j) {
                        // mat.at<uchar>(i, j) = qGray(inImage.pixel(j, i));
                        mat.at<cv::Vec3b>(i, j)[0] = inImage.pixelColor(j, i).blue();
                        mat.at<cv::Vec3b>(i, j)[1] = inImage.pixelColor(j, i).green();
                        mat.at<cv::Vec3b>(i, j)[2] = inImage.pixelColor(j, i).red();
                    }
                }

                if (DEBUG) {
                    std::cout << "mat: " << mat << std::endl;
                    cv::String str = "tmp/mat_converted_Format_RGB32.png";
                    cv::imwrite(str, mat);
                }

                return mat;
            }

            // 8-bit, 3 channel
            case QImage::Format_RGB888:
            {
                if (!inCloneImageData)
                {
                    qWarning() << "ASM::QImageToCvMat_pxw() - Conversion requires cloning so we don't modify the original QImage data";
                }

                QImage swapped = inImage.rgbSwapped();

                cv::Mat mat = cv::Mat(swapped.height(), swapped.width(),
                                               CV_8UC3,
                                               const_cast<uchar*>(swapped.bits()),
                                               static_cast<size_t>(swapped.bytesPerLine())
                                               );

                if (DEBUG) cv::imwrite("tmp/mat_converted_Format_RGB888.png", mat);

                return mat.clone();
             }

             // 8-bit, 1 channel
             case QImage::Format_Grayscale8:
             case QImage::Format_Indexed8:
             {
                 cv::Mat mat(inImage.height(), inImage.width(), CV_8UC1);

                 //memcpy(mat.data, inImage.bits(), size_t(inImage.height() * inImage.width()) * sizeof (uchar));


                 for (auto i = 0; i < inImage.height(); ++i) {
                     for (auto j = 0; j < inImage.width(); ++j) {
                         mat.at<uchar>(i, j) = qGray(inImage.pixel(j, i));
                     }
                 }

                 if (DEBUG) cv::imwrite("tmp/mat_converted_Format_Indexed8.png", mat);
                 return (inCloneImageData ? mat.clone() : mat);
             }

             default:
                 qWarning() << "ASM::QImageToCvMat_pxw() - QImage format not handled in switch:" << inImage.format();
                 break;
        }

        return cv::Mat();
    }

    // If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
    // with the cv::Mat directly
    //    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
    inline cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true)
    {
        return QImageToCvMat_pxw(inPixmap.toImage(), inCloneImageData);
    }
}

#endif // ASM_OPENCV_H
