#include "image-view.h"
#include "pointitem.h"

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <qmath.h>

constexpr double ZOOM_SCALE_STEP = 6.0;
constexpr double ANGLE_STEP = 1.0;

QString textFromColorSpace(geom::ColorSpace s) {
    switch (int(s)) {
    case geom::RGB: return "RGB";
    case geom::HSV: return "HSV";
    case geom::HSL: return "HSL";
    case geom::HSI: return "HSI";
    }
    return {};
}

QString textFromColorName(geom::ColorName c) {
    switch (int(c)) {
    case geom::gray: return "Gray";
    case geom::red: return "Red";
    case geom::green: return "Green";
    case geom::blue: return "Blue";
    case geom::hue: return "Hue";
    case geom::saturation: return "Saturation";
    case geom::value: return "Value";
    case geom::lightness: return "Lightness";
    case geom::intensity: return "Intensity";
    }
    return {};
}

#if QT_CONFIG(wheelevent)
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0) {
            // view->zoomIn(int(ZOOM_SCALE_STEP));
            emit view->scaleChanged(ZOOM_SCALE_STEP);
        } else {
            // view->zoomOut(int(ZOOM_SCALE_STEP));
            emit view->scaleChanged(-ZOOM_SCALE_STEP);
        }
        e->accept();
    } else if (e->modifiers() & Qt::AltModifier) {
        if (e->delta() > 0) {
            emit view->angleChanged(ANGLE_STEP);
        } else {
            emit view->angleChanged(-ANGLE_STEP);
        }
    } else {
        QGraphicsView::wheelEvent(e);
    }

}
#endif

/*void GraphicsView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode)
{
    QGraphicsView::fitInView(rect, aspectRadioMode);
}*/

ImageView::ImageView(const QString &name, QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(Sunken | StyledPanel);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    graphicsView = new GraphicsView(this);
    graphicsView->setRenderHint(QPainter::Antialiasing, false);
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // QGraphicsView::AnchorViewCenter

    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(size, size);

    QToolButton *zoomInIcon = new QToolButton;
    zoomInIcon->setAutoRepeat(true);
    zoomInIcon->setAutoRepeatInterval(33);
    zoomInIcon->setAutoRepeatDelay(0);
    zoomInIcon->setIcon(QPixmap(":/images/zoomin.png"));
    zoomInIcon->setIconSize(iconSize);
    QToolButton *zoomOutIcon = new QToolButton;
    zoomOutIcon->setAutoRepeat(true);
    zoomOutIcon->setAutoRepeatInterval(33);
    zoomOutIcon->setAutoRepeatDelay(0);
    zoomOutIcon->setIcon(QPixmap(":/images/zoomout.png"));
    zoomOutIcon->setIconSize(iconSize);
    zoomSlider = new QSlider;
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(500);
    zoomSlider->setValue(250);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    zoomSlider->setVisible(false);
    zoomOutIcon->setVisible(false);
    zoomInIcon->setVisible(false);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomInIcon);
    zoomSliderLayout->addWidget(zoomSlider);
    zoomSliderLayout->addWidget(zoomOutIcon);

    QToolButton *rotateLeftIcon = new QToolButton;
    rotateLeftIcon->setIcon(QPixmap(":/images/rotateleft.png"));
    rotateLeftIcon->setIconSize(iconSize);
    QToolButton *rotateRightIcon = new QToolButton;
    rotateRightIcon->setIcon(QPixmap(":/images/rotateright.png"));
    rotateRightIcon->setIconSize(iconSize);
    rotateSlider = new QSlider;
    rotateSlider->setOrientation(Qt::Horizontal);
    rotateSlider->setMinimum(-180);
    rotateSlider->setMaximum(180);
    rotateSlider->setValue(0);
    rotateSlider->setTickPosition(QSlider::TicksBelow);

    // Rotate slider layout
    QHBoxLayout *rotateSliderLayout = new QHBoxLayout;
    rotateSliderLayout->addWidget(rotateLeftIcon);
    rotateSliderLayout->addWidget(rotateSlider);
    rotateSliderLayout->addWidget(rotateRightIcon);

    resetButton = new QToolButton;
    resetButton->setText(tr("0"));
    resetButton->setEnabled(false);

    rotateSlider->setVisible(false);
    rotateLeftIcon->setVisible(false);
    rotateRightIcon->setVisible(false);
    resetButton->setVisible(false);

    // Label layout
    QHBoxLayout *labelLayout = new QHBoxLayout;
    label = new QLabel(name);
    label2 = new QLabel(tr("Pointer Mode"));
    selectModeButton = new QToolButton;
    selectModeButton->setText(tr("Select"));
    selectModeButton->setCheckable(true);
    selectModeButton->setChecked(true);
    dragModeButton = new QToolButton;
    dragModeButton->setText(tr("Drag"));
    dragModeButton->setCheckable(true);
    dragModeButton->setChecked(false);
    antialiasButton = new QToolButton;
    antialiasButton->setText(tr("Aliasing"));
    antialiasButton->setCheckable(true);
    antialiasButton->setChecked(false);
    m_tuneSlider = new QSlider(Qt::Orientation::Horizontal, this);
    m_tuneSlider->setMaximum(255);
    m_tuneSlider->setMinimum(0);
    m_saveImageButton = new QPushButton;
    m_saveImageButton->setText(tr("Save"));
    openGlButton = new QToolButton;
    openGlButton->setText(tr("OpenGL"));
    openGlButton->setCheckable(true);
#ifndef QT_NO_OPENGL
    openGlButton->setEnabled(QGLFormat::hasOpenGL());
#else
    openGlButton->setEnabled(false);
#endif
    // printButton = new QToolButton;
    // printButton->setIcon(QIcon(QPixmap(":/images/fileprint.png")));

    QButtonGroup *pointerModeGroup = new QButtonGroup(this);
    pointerModeGroup->setExclusive(true);
    pointerModeGroup->addButton(selectModeButton);
    pointerModeGroup->addButton(dragModeButton);

    labelLayout->addWidget(label);
    labelLayout->addStretch();
    labelLayout->addWidget(label2);
    labelLayout->addWidget(selectModeButton);
    labelLayout->addWidget(dragModeButton);
    labelLayout->addStretch();
    labelLayout->addWidget(m_tuneSlider);
    labelLayout->addWidget(m_saveImageButton);
    labelLayout->addWidget(antialiasButton);
    labelLayout->addWidget(openGlButton);
    // labelLayout->addWidget(printButton);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addLayout(labelLayout, 0, 0);
    topLayout->addWidget(graphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->addLayout(rotateSliderLayout, 2, 0);
    topLayout->addWidget(resetButton, 2, 1);
    setLayout(topLayout);

    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(graphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(setResetButtonEnabled()));
    connect(graphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(setResetButtonEnabled()));
    connect(selectModeButton, SIGNAL(toggled(bool)), this, SLOT(togglePointerMode()));
    connect(dragModeButton, SIGNAL(toggled(bool)), this, SLOT(togglePointerMode()));
    connect(antialiasButton, SIGNAL(toggled(bool)), this, SLOT(toggleAntialiasing()));
    connect(openGlButton, SIGNAL(toggled(bool)), this, SLOT(toggleOpenGL()));
    connect(rotateLeftIcon, SIGNAL(clicked()), this, SLOT(rotateLeft()));
    connect(rotateRightIcon, SIGNAL(clicked()), this, SLOT(rotateRight()));
    connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));
    connect(m_saveImageButton, &QPushButton::clicked, this, &ImageView::onSaveImageButton);
    connect(m_tuneSlider, &QSlider::valueChanged, this, &ImageView::onTuneSliderValueChanged);
    connect(m_saveImageButton, &QPushButton::clicked, this, &ImageView::saveChannelImage);
    // connect(printButton, SIGNAL(clicked()), this, SLOT(print()));

    connect(this, &QWidget::customContextMenuRequested,
            this, &ImageView::showContextMenu);

    setupMatrix();
}

QGraphicsView *ImageView::view() const
{
    return static_cast<QGraphicsView *>(graphicsView);
}

void ImageView::onSaveImageButton()
{

}

void ImageView::onTuneSliderValueChanged(int value)
{
    emit tuneSliderChanged(value);
}

void ImageView::resetView()
{
    zoomSlider->setValue(250);
    rotateSlider->setValue(0);
    setupMatrix();
    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
    graphicsView->fitInView(graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);

    resetButton->setEnabled(false);
}

void ImageView::setResetButtonEnabled()
{
    resetButton->setEnabled(true);
}

void ImageView::setupMatrix()
{
    qreal scale = qPow(qreal(2), (zoomSlider->value() - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);
    matrix.rotate(rotateSlider->value());

    graphicsView->setMatrix(matrix);
    setResetButtonEnabled();
}

void ImageView::togglePointerMode()
{
    graphicsView->setDragMode(selectModeButton->isChecked()
                              ? QGraphicsView::RubberBandDrag
                              : QGraphicsView::ScrollHandDrag);
    graphicsView->setInteractive(selectModeButton->isChecked());
}

void ImageView::toggleOpenGL()
{
#ifndef QT_NO_OPENGL
    graphicsView->setViewport(openGlButton->isChecked() ? new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}

void ImageView::toggleAntialiasing()
{
    graphicsView->setRenderHint(QPainter::Antialiasing, antialiasButton->isChecked());
}

void ImageView::print()
{
//#if QT_CONFIG(printdialog)
#if defined(QT_PRINTSUPPORT_LIB)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        graphicsView->render(&painter);
    }
#endif
}

void ImageView::zoomIn(int level)
{
    zoomSlider->setValue(zoomSlider->value() + level);
}

void ImageView::zoomOut(int level)
{
    zoomSlider->setValue(zoomSlider->value() - level);
}

void ImageView::rotate(double angleStep)
{
    rotateSlider->setValue(rotateSlider->value() - int(angleStep));
}

void ImageView::rotateLeft()
{
    rotateSlider->setValue(rotateSlider->value() - 10);
}

void ImageView::rotateRight()
{
    rotateSlider->setValue(rotateSlider->value() + 10);
}

void ImageView::changeChannelNumber(int cn)
{
    m_channelNumber = cn;
    label->setText(textFromColorName(ColorName(cn + m_colorSpace * 3 + red)));
}

void ImageView::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Add control point", this);

    connect(&action1, &QAction::triggered, [&]() {
        emit pointAdded(pos);
    });

    contextMenu.addAction(&action1);

    contextMenu.exec(mapToGlobal(pos));
}

QMap<QString, QPointF> ImageView::getControlPoints() const
{
    QMap<QString, QPointF> map;

    auto item_list = view()->scene()->items();

    foreach (auto item, item_list) {
        PointItem *pi = nullptr;
        pi = qgraphicsitem_cast<PointItem*>(item);
        if (pi) {
            map.insert(pi->text(), pi->coord());
        }
    }

    return map;
}

std::vector<QPointF> ImageView::getControlPointsSorted() const
{
    return getControlPoints().values().toVector().toStdVector();
}
