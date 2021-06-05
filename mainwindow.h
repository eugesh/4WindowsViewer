#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGenericMatrix>
#include <QGraphicsScene>
#include "image-item.h"
#include "image-processing-common.h"


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QSplitter;
class QDockWidget;
class ImageView;
class PixelRuler;
class RubberRect;
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

private:
    void create4Windows();
    void create2Windows();
    void openImages4Windows();
    void show4Windows();
    void hide4Windows();
    void removeAdditionalWindows();
    // void changeColorSpace(QColor::Spec s);
    void changeColorSpace(ColorSpace s);

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
    void on2WindowsCheck(int check);
    void onActionRGB();
    void onActionHSV();
    void onActionHSI();
    void onAction4PointsRubberRect();
    void onAction4PointsWithLines();
    void onActionProjectiveTransform();
    void onActionPixelRuler();
    void onActionSavePerspectiveProjectionMatrix();
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
    // Projective trabsform tool
    QSharedPointer<RubberRect> m_RR;
    QSharedPointer<PixelRuler> m_PixelRuler;
};

#endif // MAINWINDOW_H
