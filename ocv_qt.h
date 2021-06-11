#ifndef OCV_QT_H
#define OCV_QT_H

#include "asm_opencv.h"

#include <vector>
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
#include <QtCore>

std::vector<cv::Point2d> pointsConverter(const std::vector<QPointF> &pointsIn)
{
    std::vector<cv::Point2d> pointsOut;

    for (size_t i = 0; i < pointsIn.size(); ++i) {
        pointsOut.push_back(cv::Point2d(pointsIn[i].x(), pointsIn[i].y()));
    }

    return pointsOut;
}

std::vector<cv::Point2i> pointsConverter(const std::vector<QPoint> &pointsIn)
{
    std::vector<cv::Point2i> pointsOut;

    for (size_t i = 0; i < pointsIn.size(); ++i) {
        pointsOut.push_back(cv::Point2i(pointsIn[i].x(), pointsIn[i].y()));
    }

    return pointsOut;
}

std::vector<QPoint> pointsConverter(const std::vector<cv::Point2i> &pointsIn)
{
    std::vector<QPoint> pointsOut;

    for (size_t i = 0; i < pointsIn.size(); ++i) {
        pointsOut.push_back(QPoint(pointsIn[i].x, pointsIn[i].y));
    }

    return pointsOut;
}

int qt2cvImageFormat(int qtFormat)
{

}

int cv2qtImageFormat(int cvFormat)
{

}

cv::Mat imageConverter(const QImage &qimage)
{
    std::cout << "qimage.format(): " << qimage.format() << std::endl;
    cv::Mat mat = ASM::QImageToCvMat(qimage, true);



    return mat;
}

QImage imageConverter(const cv::Mat &mat)
{
    //QImage qimage = ASM::cvMatToQImage(mat);

    return ASM::cvMatToQImage(mat);

    //return qimage;
}

#endif // OCV_QT_H
