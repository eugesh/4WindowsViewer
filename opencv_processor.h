#ifndef OPENCV_PROCESSOR_H
#define OPENCV_PROCESSOR_H

#include "imageprocessor.h"
#include "image-processing-common.h"
#include "ocv_qt.h"

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

// const std::vector<geom::PixelPoint()> &rubberRectPoints

QImage calc_projection_4points(const std::vector<QPointF> &points1, const std::vector<QPointF> &points2, const QImage &imageIn, QSize sizeOut)
{
    // QImage outImage;
    // Convert points from Qt to OpenCV
    std::vector<cv::Point2d> ocv_pointsIn = pointsConverter(points1);
    std::vector<cv::Point2d> ocv_pointsOut = pointsConverter(points2);
    // Convert images
    cv::Mat ocv_imageIn = imageConverter(imageIn);
    cv::Mat ocv_imageOut;

    cv::Mat M = cv::findHomography(ocv_pointsIn, ocv_pointsOut);
    /*CV_EXPORTS_W Mat findHomography( InputArray srcPoints, InputArray dstPoints,
                                     int method = 0, double ransacReprojThreshold = 3,
                                     OutputArray mask=noArray(), const int maxIters = 2000,
                                     const double confidence = 0.995);*/

    // cv::warpPerspective(ocv_imageIn, ocv_imageOut, M, cv::Size2d(ocv_imageIn.rows, ocv_imageIn.cols));
    cv::warpPerspective(ocv_imageIn, ocv_imageOut, M, cv::Size2d(sizeOut.width(), sizeOut.height()));

    // Convert images back to Qt

    return imageConverter(ocv_imageOut);
}

#endif // OPENCV_PROCESSOR_H
