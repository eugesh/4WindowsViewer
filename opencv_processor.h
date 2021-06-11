#ifndef OPENCV_PROCESSOR_H
#define OPENCV_PROCESSOR_H

#include "imageprocessor.h"
#include "image-processing-common.h"
#include "ocv_qt.h"

#include <vector>
#include <QGenericMatrix>
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
constexpr int DEBUG = 1;

// @MOVE_TO_LIB, @MOVETOLIB
//QMatrix3x3
template <int N, int M, typename T, typename C>
QGenericMatrix<N, M, T> MatToMatrix(const cv::Mat &mat)
{
    QGenericMatrix<N, M, T> matrix;

    if (N != mat.rows || M != mat.rows)
        return QGenericMatrix<N, M, T>();

    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            matrix(i, j) = mat.at<C>(j, i);
        }
    }

    return matrix;
}

// @MOVE_TO_LIB, @MOVETOLIB
template <int N, int M, typename T>
cv::Mat MatrixToMat(const QGenericMatrix<N, M, T> &matrix)
{
    cv::Mat mat(M, N, CV_32FC1); // ToDo: make dependent from T

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            mat.at<T>(j, i) = matrix(i, j);
        }
    }

    return mat;
}

// @MOVE_TO_LIB, @MOVETOLIB
QMatrix3x3 estimatePerspectiveProjection(const std::vector<QPointF> &points1, const std::vector<QPointF> &points2)
{
    QMatrix3x3 out;
    // Convert points from Qt to OpenCV
    std::vector<cv::Point2d> ocv_pointsIn = pointsConverter(points1);
    std::vector<cv::Point2d> ocv_pointsOut = pointsConverter(points2);

    cv::Mat M = cv::findHomography(ocv_pointsIn, ocv_pointsOut);

    out = MatToMatrix<3, 3, float, double>(M);

    return out;
}

// @MOVE_TO_LIB, @MOVETOLIB
QImage applyPerspectiveProjection(const QMatrix3x3 &matrix, const QImage &imageIn, QSize sizeOut)
{
    // Convert images
    cv::Mat ocv_imageIn = imageConverter(imageIn);
    cv::Mat ocv_imageOut;
    cv::Mat M = MatrixToMat(matrix);

    cv::warpPerspective(ocv_imageIn, ocv_imageOut, M, cv::Size2d(sizeOut.width(), sizeOut.height()));

    return imageConverter(ocv_imageOut);
}

QImage calc_projection_4points(QMatrix3x3 &matrix, const std::vector<QPointF> &points1, const std::vector<QPointF> &points2, const QImage &imageIn, QSize sizeOut)
{
    // Convert points from Qt to OpenCV
    std::vector<cv::Point2d> ocv_pointsIn = pointsConverter(points1);
    std::vector<cv::Point2d> ocv_pointsOut = pointsConverter(points2);
    // Convert images
    cv::Mat ocv_imageIn = imageConverter(imageIn);
    cv::Mat ocv_imageOut;

    /*CV_EXPORTS_W Mat findHomography( InputArray srcPoints, InputArray dstPoints,
                                     int method = 0, double ransacReprojThreshold = 3,
                                     OutputArray mask=noArray(), const int maxIters = 2000,
                                     const double confidence = 0.995);*/
    cv::Mat M = cv::findHomography(ocv_pointsIn, ocv_pointsOut);

    // cv::warpPerspective(ocv_imageIn, ocv_imageOut, M, cv::Size2d(ocv_imageIn.rows, ocv_imageIn.cols));
    cv::warpPerspective(ocv_imageIn, ocv_imageOut, M, cv::Size2d(sizeOut.width(), sizeOut.height()));

    /*for (int i = 0; i < M.rows; ++i) {
        for (int j = 0; j < M.cols; ++j) {
            matrix(i, j) = M.at<double>(j, i);
            if (DEBUG) std::cout << M.at<double>(j, i) << " ";
        }
        if (DEBUG) std::cout << '\n';
    }*/
    matrix = MatToMatrix<3, 3, float, double>(M);

    // Convert images back to Qt

    return imageConverter(ocv_imageOut);
}

#endif // OPENCV_PROCESSOR_H
