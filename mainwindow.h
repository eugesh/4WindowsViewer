#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGenericMatrix>
#include <QGraphicsScene>
#include "image-item.h"
#include "image-processing-common.h"
#include "pointitem.h"


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QSplitter;
class QDockWidget;
class ImageView;
class PixelRuler;
class RubberRect;
class QSettings;
QT_END_NAMESPACE

using namespace geom;

namespace Ui {
class MainWindow;
}

enum WindowsSplitters {
    HSplitter1, // First (top) horizontal splitter
    HSplitter2, // Second (bottom) horizontal splitter
    VSplitter // Vertical splitter
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void applyProjectiveTransform(QSize outSize);
    void create4Windows();
    void create3Windows();
    // 2 Windows: projective transform tool
    void create2Windows();
    // 3 Windows: Corresponding points input: left, right, match
    void openImages3Windows();
    void show3Windows();
    void hide3Windows();
    void initSettings();
    // 4 Windows: Color image + H,S,V or R,G,B
    void openImages4Windows();
    void show4Windows();
    void hide4Windows();
    void removeAdditionalWindows();
    // void changeColorSpace(QColor::Spec s);
    void changeColorSpace(ColorSpace s);
    void createControlPtsToolBar();
    bool loadControlPoints(const QString &fp);
    bool saveControlPoints(const QString &fp);
    // Apply Homography with Control Points and show on the first View
    void registerLeftRight();
    // Universal File Storage YAML parameter loader // ToDo: move to lib
    cv::Mat readMtxFileStorage(const QString &path, const QString mtxName);
    void addPointPair(const QString &pointName, const QPointF pts[2]);

signals:
    void scaleChanged(double);
    void angleChanged(double);
    void imagePlaceChanged(double, double);

private slots:
    int openImage();
    int saveImageAs(const QString &path);
    void onAddVSplitter();
    void onAddHSplitter();
    void onAddDockWidget();
    void on4WindowsCheck(int check);
    void on3WindowsCheck(int check);
    void on2WindowsCheck(int check);
    void onActionRGB();
    void onActionHSV();
    void onActionHSI();
    void onAction4PointsRubberRect();
    void onAction4PointsWithLines();
    void onActionProjectiveTransform();
    void onActionPixelRuler();
    void onActionSavePerspectiveProjectionMatrix();
    void onActionLoadPerspectiveProjectionMatrix();
    void onTuneSliderChanged(int val);
    void onSaveChannel();
    // Input corresponding points by hand
    void onActionPointInput();
    void onPointAdded(const QPoint&);
    void onRemoveControlPoints(const std::vector<quint32> &ids);
    //void onSimultaneousScrollCheck(int check);

private:
    Ui::MainWindow *ui;
    // Main Scene
    QSharedPointer<QGraphicsScene> m_scene;
    QSharedPointer<ImageView> m_view;
    QImage          m_image;
    QString         m_img_path;
    QString         m_saveMatrixPath = "./projMatrix.txt";
    QMatrix3x3      m_projMatrix;
    QSharedPointer<ImageItem> m_item;
    // 3 Additional image viewers. Indexed clockwise
    // main window 0
    //           1 2
    QVector<QSharedPointer<QSplitter> > m_vpSplitters;
    // Scenes created after splitters
    // QVector<QSharedPointer<QGraphicsScene> > m_vpScene;
    QVector<QSharedPointer<ImageView> > m_vpImageView;
    QVector<QSharedPointer<ImageItem> > m_vpImageItems;
    bool m_isSimultaneousScroll = true;
    ColorSpace m_colorSpace = RGB;
    QString m_lastPath = "/home";
    // Projective transform tool
    QSharedPointer<RubberRect> m_RR;
    QSharedPointer<PixelRuler> m_PixelRuler;

    QSettings *m_settings = nullptr;

    // Corresponding points
    QVector<quint32> m_pointsIds;
    QToolBar *m_controlPtToolBar;
    cv::Mat m_cameraMtx[2], m_distCoeffs[2];
    QString m_leftImagePairPath;
    QString m_rightImagePairPath;
    QString m_leftCalibMatrixPath;
    QString m_rightCalibMatrixPath;
    QString m_controlPointsPath;
};

#endif // MAINWINDOW_H
