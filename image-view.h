#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QWheelEvent>
#include "image-processing-common.h"

using namespace geom;

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
class QPushButton;
QT_END_NAMESPACE

class ImageView;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(ImageView *v) : QGraphicsView(), view(v) { }
    //void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);

protected:
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *) override;
#endif

private:
    ImageView *view;
};


class ImageView : public QFrame
{
    Q_OBJECT
public:
    explicit ImageView(const QString &name, QWidget *parent = nullptr);

    QGraphicsView *view() const;
    ColorSpace colorSpace() { return m_colorSpace; }
    int channelNumber() { return m_channelNumber; }
    void changeColorSpace(ColorSpace cs) { m_colorSpace = cs; }
    void changeChannelNumber(int cn);
    QSlider* tuneSlider() { return m_tuneSlider; }

signals:
    void scaleChanged(double value);
    void angleChanged(double value);
    void tuneSliderChanged(int value);
    void saveChannelImage();
    void pointAdded(const QPoint&);

public slots:
    void zoomIn(int level = 1);
    void zoomOut(int level = 1);
    void rotate(double angleStep);

private slots:
    void resetView();
    void setResetButtonEnabled();
    void setupMatrix();
    void togglePointerMode();
    void toggleOpenGL();
    void toggleAntialiasing();
    void print();
    void rotateLeft();
    void rotateRight();
    void onSaveImageButton();
    void onTuneSliderValueChanged(int value);
    void showContextMenu(const QPoint&);

private:
    GraphicsView *graphicsView;
    QLabel *label;
    QLabel *label2;
    QToolButton *selectModeButton;
    QToolButton *dragModeButton;
    QToolButton *openGlButton;
    QToolButton *antialiasButton;
    QToolButton *printButton;
    QToolButton *resetButton;
    QSlider *zoomSlider;
    QSlider *rotateSlider;
    ColorSpace m_colorSpace;
    int m_channelNumber;
    QSlider *m_tuneSlider;
    QPushButton *m_saveImageButton;
};

#endif // IMAGEVIEW_H
