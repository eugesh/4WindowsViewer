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
//constexpr int DEBUG = 1;

QImage calc_projection_4points_ocv(cv::Mat &matrix,
                                   const std::vector<cv::Point2d> &points1,
                                   const std::vector<cv::Point2d> &points2,
                                   const cv::Mat &imageIn,
                                   cv::Size2d sizeOut);

QImage calc_projection_4points(QMatrix3x3 &matrix,
                               const std::vector<QPointF> &points1,
                               const std::vector<QPointF> &points2,
                               const QImage &imageIn, QSize sizeOut);

QImage applyBWThreshold(const QImage &qimg, int threshold, bool isInverse = false);
cv::Mat applyBWThreshold(const cv::Mat &mat, int threshold, bool isInverse = false);

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

QImage calc_projection_4points(QMatrix3x3 &matrix,
                               const std::vector<QPointF> &points1,
                               const std::vector<QPointF> &points2,
                               const QImage &imageIn, QSize sizeOut)
{
    // Convert points from Qt to OpenCV
    std::vector<cv::Point2d> ocv_pointsIn = pointsConverter(points1);
    std::vector<cv::Point2d> ocv_pointsOut = pointsConverter(points2);

    // Convert images
    cv::Mat ocv_imageIn = imageConverter(imageIn);

    cv::Mat M;

    QImage qimg = calc_projection_4points_ocv(M,
                                              ocv_pointsIn,
                                              ocv_pointsOut,
                                              ocv_imageIn,
                                              cv::Size2d(sizeOut.width(), sizeOut.height()));

    matrix = MatToMatrix<3, 3, float, double>(M);

    return qimg;
}

QImage calc_projection_4points_ocv(cv::Mat &matrix,
                                   const std::vector<cv::Point2d> &points1,
                                   const std::vector<cv::Point2d> &points2,
                                   const cv::Mat &imageIn,
                                   cv::Size2d sizeOut)
{
    cv::Mat ocv_imageOut;

    /*CV_EXPORTS_W Mat findHomography( InputArray srcPoints, InputArray dstPoints,
                                     int method = 0, double ransacReprojThreshold = 3,
                                     OutputArray mask=noArray(), const int maxIters = 2000,
                                     const double confidence = 0.995);*/
    if (points1.size() != points2.size() or points1.empty() or points2.empty()
        or points1.size() < 4 or points2.size() < 4)
        return {};

    if (DEBUG) {
        std::cout << "Points1: "/* << points1*/ << std::endl;
        for (int i = 0; i < points1.size(); ++i)
            std::cout << points1[i] << " ";
    }
    if (DEBUG) {
        std::cout << std::endl << "Points2: " /*<< points2 */<< std::endl;
        std::cout << "Size = " << points2.size() << std::endl;
        for (int i = 0; i < points2.size(); ++i)
            std::cout << points2.at(i).x << " " << points2.at(i).y << " ";
    }

    // cv::imwrite("imageIn1.png", imageIn);

    try {
        matrix = cv::findHomography(points1, points2, 0);
    } catch (const std::exception& ex) {
        std::cout << "calc_projection_4points: findHomography: exception" << ex.what() << std::endl;
        return {};
    } catch (const std::string& ex) {
        std::cout << "calc_projection_4points: findHomography: exception: " << ex << std::endl;
        return {};
    } catch (...) {
        std::cout << "calc_projection_4points: findHomography: exception" << std::endl;
        return {};
    }

    if (DEBUG) std::cout << std::endl << "matrix: " << matrix << std::endl;
    if (DEBUG) std::cout << std::endl << "sizeOut: " << sizeOut << std::endl;

    // cv::warpPerspective(ocv_imageIn, ocv_imageOut, M, cv::Size2d(ocv_imageIn.rows, ocv_imageIn.cols));
    if (DEBUG) cv::imwrite("imageIn2.png", imageIn);
    try {
        cv::warpPerspective(imageIn, ocv_imageOut, matrix, sizeOut);
    } catch (const std::exception& ex) {
        std::cout << "calc_projection_4points: warpPerspective: exception" << ex.what() << std::endl;
        return {};
    } catch (const std::string& ex) {
        std::cout << "calc_projection_4points: warpPerspective: exception: " << ex << std::endl;
        return {};
    } catch (...) {
        std::cout << "calc_projection_4points: warpPerspective: exception" << std::endl;
        return {};
    }
    /*for (int i = 0; i < M.rows; ++i) {
        for (int j = 0; j < M.cols; ++j) {
            matrix(i, j) = M.at<double>(j, i);
            if (DEBUG) std::cout << M.at<double>(j, i) << " ";
        }
        if (DEBUG) std::cout << '\n';
    }*/

    // Convert images back to Qt

    return imageConverter(ocv_imageOut);
}

QImage applyBWThreshold(const QImage &qimg, int threshold, bool isInverse)
{
    cv::Mat mat = imageConverter(qimg);

    return imageConverter(applyBWThreshold(mat, threshold, isInverse));
}

cv::Mat applyBWThreshold(const cv::Mat &mat, int threshold, bool isInverse)
{
    cv::Mat outMat;

    // CV_EXPORTS_W double threshold( InputArray src, OutputArray dst,
    // double thresh, double maxval, int type );
    if (DEBUG) cv::imwrite("tmp/mat_before_threshold.png", mat);
    cv::threshold(mat, outMat, threshold, 255, cv::THRESH_BINARY + isInverse);

    if (DEBUG) cv::imwrite("tmp/outMat_threshold.png", outMat);
    // std::cout << outMat << std::endl;

    return outMat;
}

#endif // OPENCV_PROCESSOR_H
