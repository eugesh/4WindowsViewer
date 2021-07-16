#include <QAction>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QSlider>
#include <QSplitter>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image-view.h"
#include "image-item.h"
#include "imageprocessor.h"
#include "opencv_processor.h"
#include "RubberRect.h"
#include "pixelruler.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_scene = QSharedPointer<QGraphicsScene>(new QGraphicsScene(this));
    // ui->graphicsView->setScene(m_scene);


    m_item = QSharedPointer<ImageItem>(new ImageItem());
    m_scene->addItem(m_item.get());

    m_view = QSharedPointer<ImageView>(new ImageView("Main view"));
    m_view->view()->setScene(m_scene.get());

    setCentralWidget(m_view.get());

    connect(ui->actionOpen_image, &QAction::triggered, this, &MainWindow::openImage);
    connect(ui->actionSavePerspectiveProjectionMatrix, &QAction::triggered,
            this, &MainWindow::onActionSavePerspectiveProjectionMatrix);

    //  ToDo: create shortcuts
    connect(ui->actionHorizontal, &QAction::triggered, this, &MainWindow::onAddHSplitter);
    connect(ui->actionVertical, &QAction::triggered, this, &MainWindow::onAddVSplitter);
    connect(ui->actionAddDockWidget, &QAction::triggered, this, &MainWindow::onAddDockWidget);
    // connect(ui->actionSimultaneousScroll, &QAction::chan, this, &MainWindow::onSimultaneousScrollCheck);
    connect(ui->action4Windows, &QAction::triggered, this, &MainWindow::on4WindowsCheck);
    connect(ui->action2Windows, &QAction::triggered, this, &MainWindow::on2WindowsCheck);

    connect(ui->actionRGB, &QAction::triggered, this, &MainWindow::onActionRGB);
    connect(ui->actionHSV, &QAction::triggered, this, &MainWindow::onActionHSV);
    connect(ui->actionHSI, &QAction::triggered, this, &MainWindow::onActionHSI);

    connect(m_view.get(), &ImageView::scaleChanged, this, &MainWindow::scaleChanged);
    connect(m_view.get(), &ImageView::angleChanged, this, &MainWindow::angleChanged);
    connect(this, &MainWindow::scaleChanged, m_view.get(), &ImageView::zoomIn);
    connect(this, &MainWindow::angleChanged, m_view.get(), &ImageView::rotate);

    connect(ui->action4PointsRubberRect, &QAction::triggered, this, &MainWindow::onAction4PointsRubberRect);
    connect(ui->action4PointsWithLines, &QAction::triggered, this, &MainWindow::onAction4PointsWithLines);
    connect(ui->actionProjectiveTransform, &QAction::triggered, this, &MainWindow::onActionProjectiveTransform);
    connect(ui->actionLoadPerspectiveProjectionMatrix, &QAction::triggered,
            this, &MainWindow::onActionLoadPerspectiveProjectionMatrix);
    connect(ui->actionPixelRuler, &QAction::triggered, this, &MainWindow::onActionPixelRuler);
}

MainWindow::~MainWindow()
{
    removeAdditionalWindows();
    delete ui;
}

void MainWindow::on4WindowsCheck(int check)
{
    if (check) {
        removeAdditionalWindows();
        ui->action2Windows->setChecked(false);
        if (m_vpSplitters.empty())
            create4Windows();
        else
            show4Windows();
        if (m_vpImageItems.empty())
            openImages4Windows();
    } else {
        hide4Windows();
    }
}

void MainWindow::on2WindowsCheck(int check)
{
    if (check) {
        removeAdditionalWindows();
        ui->action4Windows->setChecked(false);
        create2Windows();
        if (m_vpImageItems.empty())
            openImages4Windows();
    } else {

    }
}

void MainWindow::onActionRGB()
{
    changeColorSpace(RGB);
    ui->actionHSV->setChecked(false);
    ui->actionHSI->setChecked(false);
}

void MainWindow::onActionHSV()
{
    changeColorSpace(HSV);
    ui->actionRGB->setChecked(false);
    ui->actionHSI->setChecked(false);

    /*for (int i = 0; i < m_vpImageView.size(); ++i) {
        m_vpImageView[i]->changeColorSpace(m_colorSpace);
        m_vpImageView[i]->changeChannelNumber(i);
    }*/
}

void MainWindow::onActionHSI()
{
    changeColorSpace(HSI);
    ui->actionHSV->setChecked(false);
    ui->actionRGB->setChecked(false);
}

/*void MainWindow::changeColorSpace(QColor::Spec s)
{
    m_colorSpace ;
}*/

void MainWindow::changeColorSpace(ColorSpace s)
{
    m_colorSpace = s;

    openImages4Windows();

    for (int i = 0; i < m_vpImageView.count(); ++i) {
        m_vpImageView[i].get()->view()->scene()->update();
        m_vpImageView[i]->changeColorSpace(m_colorSpace);
        m_vpImageView[i]->changeChannelNumber(i);
    }
}

void MainWindow::create4Windows()
{
    QSharedPointer<QSplitter> h1Splitter = QSharedPointer<QSplitter>(new QSplitter(this));
    QSharedPointer<QSplitter> h2Splitter = QSharedPointer<QSplitter>(new QSplitter(this));

    QSharedPointer<QSplitter> vSplitter = QSharedPointer<QSplitter> (new QSplitter(this));
    vSplitter->setOrientation(Qt::Vertical);
    vSplitter->addWidget(h1Splitter.get());
    vSplitter->addWidget(h2Splitter.get());

    h1Splitter->addWidget(m_view.get());

    m_vpImageView.push_back(QSharedPointer<ImageView>(new ImageView("Top right view", this)));
    QGraphicsScene *scene = new QGraphicsScene(this);
    m_vpImageView.last()->view()->setScene(scene);
    h1Splitter->addWidget(m_vpImageView.last().get());

    m_vpImageView.push_back(QSharedPointer<ImageView>(new ImageView("Bottom left view", this)));
    QGraphicsScene *scene2 = new QGraphicsScene(this);
    m_vpImageView.last()->view()->setScene(scene2);
    h2Splitter->addWidget(m_vpImageView.last().get());

    m_vpImageView.push_back(QSharedPointer<ImageView>(new ImageView("Bottom right view", this)));
    QGraphicsScene *scene3 = new QGraphicsScene(this);
    m_vpImageView.last()->view()->setScene(scene3);
    h2Splitter->addWidget(m_vpImageView.last().get());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(vSplitter.get());
    setLayout(layout);
    setCentralWidget(vSplitter.get());

    for (int i = 0; i < m_vpImageView.size(); ++i) {
        connect(m_vpImageView[i].get(), &ImageView::scaleChanged, this, &MainWindow::scaleChanged);
        connect(m_vpImageView[i].get(), &ImageView::angleChanged, this, &MainWindow::angleChanged);
        connect(m_vpImageView[i].get(), &ImageView::tuneSliderChanged, this, &MainWindow::onTuneSliderChanged);
        connect(m_vpImageView[i].get(), &ImageView::saveChannelImage, this, &MainWindow::onSaveChannel);
        connect(this, &MainWindow::scaleChanged, m_vpImageView[i].get(), &ImageView::zoomIn);
        connect(this, &MainWindow::angleChanged, m_vpImageView[i].get(), &ImageView::rotate);
    }

    m_vpSplitters.push_back(h1Splitter);
    m_vpSplitters.push_back(h2Splitter);
    m_vpSplitters.push_back(vSplitter);
}

/*void MainWindow::onTuneSliderChanged(int val)
{
    if (ImageView* view = qobject_cast<ImageView*> (sender())) {
        foreach (auto *item, view->view()->scene()->items()) {
            if (auto imageItem = dynamic_cast<ImageItem*> (item)) {
                // imageItem->setFiltered(geom::getChannel(m_item->getImage(), m_colorSpace, i));
                QImage img = imageItem->getImage();
                if (DEBUG) img.save("tmp/img.png");
                QImage imgBW(img.size(), QImage::Format_Indexed8);
                static QVector<QRgb> sColorTable;
                if (sColorTable.isEmpty()) {
                    sColorTable.resize(256);
                    for (int i = 0; i < 256; ++i) {
                        sColorTable[i] = qRgb(i, i, i);
                    }
                }
                imgBW.setColorTable(sColorTable);
                imgBW = (applyBWThreshold(img, val));
                cv::Mat applyBWThreshold(const cv::Mat &mat, int threshold, bool isInverse = false);
                if (DEBUG) imgBW.save("tmp/BW.png");
                imageItem->setFiltered(imgBW);
                view->view()->scene()->update();
            }
        }
        // m_vpImageItems[i]->setFiltered(geom::getChannel(m_item->getImage(), m_colorSpace, i));
    }
}*/

void MainWindow::onTuneSliderChanged(int val)
{
    if (ImageView* view = qobject_cast<ImageView*> (sender())) {
        foreach (auto *item, view->view()->scene()->items()) {
            if (auto imageItem = dynamic_cast<ImageItem*> (item)) {
                view->tuneSlider()->setToolTip(QString("%1").arg(view->tuneSlider()->value()));
                // imageItem->setFiltered(geom::getChannel(m_item->getImage(), m_colorSpace, i));
                QImage img = imageItem->getImage();
                if (DEBUG) img.save("tmp/img.png");
                QImage imgBW(img.size(), QImage::Format_Indexed8);
                static QVector<QRgb> sColorTable;
                if (sColorTable.isEmpty()) {
                    sColorTable.resize(256);
                    for (int i = 0; i < 256; ++i) {
                        sColorTable[i] = qRgb(i, i, i);
                    }
                }
                imgBW.setColorTable(sColorTable);
                imgBW = (applyBWThreshold(img, val));
                if (DEBUG) imgBW.save("tmp/BW.png");
                imageItem->setFiltered(imgBW);
                view->view()->scene()->update();
            }
        }
        // m_vpImageItems[i]->setFiltered(geom::getChannel(m_item->getImage(), m_colorSpace, i));
    }
}

void MainWindow::onSaveChannel()
{
    if (ImageView* view = qobject_cast<ImageView*> (sender())) {
        foreach (auto *item, view->view()->scene()->items()) {
            if (auto imageItem = dynamic_cast<ImageItem*> (item)) {
                QString fullFilePath =
                        QFileDialog::getSaveFileName(nullptr, tr("Enter filename or select file"),
                                                     tr("/home"), tr("Images (*.png *.bmp *.tif *.xpm *.jpg *.jpeg *.JPG)"));

                imageItem->getFiltered().save(fullFilePath);
            }
        }
    }
}

void MainWindow::create2Windows()
{
    QSharedPointer<QSplitter> hSplitter = QSharedPointer<QSplitter>(new QSplitter(this));

    hSplitter->addWidget(m_view.get());

    m_vpImageView.push_back(QSharedPointer<ImageView>(new ImageView("Top right view", this)));
    QGraphicsScene *scene = new QGraphicsScene(this);
    m_vpImageView.last()->view()->setScene(scene);
    hSplitter->addWidget(m_vpImageView.last().get());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(hSplitter.get());
    setLayout(layout);
    setCentralWidget(hSplitter.get());

    for (int i = 0; i < m_vpImageView.size(); ++i) {
        connect(m_vpImageView[i].get(), &ImageView::scaleChanged, this, &MainWindow::scaleChanged);
        connect(m_vpImageView[i].get(), &ImageView::angleChanged, this, &MainWindow::angleChanged);
        connect(m_vpImageView[i].get(), &ImageView::tuneSliderChanged, this, &MainWindow::onTuneSliderChanged);
        connect(this, &MainWindow::scaleChanged, m_vpImageView[i].get(), &ImageView::zoomIn);
        connect(this, &MainWindow::angleChanged, m_vpImageView[i].get(), &ImageView::rotate);
    }

    m_vpSplitters.push_back(hSplitter);
}

void MainWindow::openImages4Windows()
{
    for (int i = 0; i < m_vpImageView.size(); ++i) {
        m_vpImageView[i]->changeColorSpace(m_colorSpace);
        m_vpImageView[i]->changeChannelNumber(i);
        if (m_vpImageItems.count() <= i) {
            m_vpImageItems.push_back(QSharedPointer<ImageItem>(new ImageItem()));
            m_vpImageItems[i]->setImage(m_item->getImage());
            QGraphicsScene *scene = new QGraphicsScene(this);
            m_vpImageView[i]->view()->setScene(scene);
            scene->addItem(m_vpImageItems[i].get());
            scene->update();
        }
        m_vpImageItems[i]->setImage(geom::getChannel(m_item->getImage(), m_colorSpace, i));
        m_vpImageItems[i]->scene()->update();
    }
}

void MainWindow::show4Windows()
{

}

void MainWindow::hide4Windows()
{

}

void MainWindow::removeAdditionalWindows()
{
    if (! m_view.isNull())
        setCentralWidget(m_view.get());
    if (m_vpImageItems.count())
        m_vpImageItems.clear();
    if (m_vpImageView.count())
        m_vpImageView.clear();
    if (m_vpSplitters.count())
        m_vpSplitters.clear();
}

// ui->actionSimultaneousScroll->isChecked();
/*void MainWindow::onSimultaneousScrollCheck(int check) {
    m_isSimultaneousScroll = check;
}*/

int
MainWindow::openImage()
{
    QString fullFilePath =
            QFileDialog::getOpenFileName(this, tr("Choose image file"), m_lastPath,
                                         tr("Images (*.png *.bmp *.tif *.tiff *.gif *.xpm *.jpg *.jpeg *.JPG)"));
    // m_settings_dlg->ui->queue_lineEdit->setText(queuePath);

    if (fullFilePath.isEmpty())
        return -1;

    QFileInfo fi(fullFilePath);
    m_lastPath = fi.absoluteDir().path();

    m_image = QImage(fullFilePath);

    if (DEBUG) std::cout << "input image.format(): " << m_image.format() << std::endl;

    m_item->setImage(m_image);

    openImages4Windows();

    m_scene->update();

    return 0;
}

int MainWindow::saveImageAs(const QString &path)
{

    return 0;
}

void MainWindow::onAddVSplitter()
{
    QSplitter *vSplitter = new QSplitter;
    vSplitter->setOrientation(Qt::Vertical);

}

void MainWindow::onAddHSplitter()
{

}

void MainWindow::onAddDockWidget()
{
    QDockWidget *dock = new QDockWidget(tr(""), this);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::onAction4PointsRubberRect()
{
    QPointF center = QPointF(double(m_image.width()) / 2, double(m_image.height() / 2));
    QRectF rect = QRectF(m_image.rect());
    m_RR = QSharedPointer<RubberRect> (new RubberRect(rect, rect, center, 1, m_item.get()));
    m_RR->setEditable(true);

    m_scene->addItem(m_RR.get());
    m_RR->show();
    m_scene->update();
}

void MainWindow::onAction4PointsWithLines()
{
    if (m_RR.isNull()) {
        onAction4PointsRubberRect();
    }

    m_RR->setHasAuxLines(true);
    m_scene->update();
}

void MainWindow::onActionPixelRuler()
{
    if (m_vpSplitters.count() && m_vpImageItems.count()) {
        m_PixelRuler = QSharedPointer<PixelRuler> (new PixelRuler(QLineF(10, 10, 100, 100),
                       m_vpImageItems.first().get()->getImage().rect(), m_vpImageItems.first().get()));
        m_PixelRuler->setEditable(true);

        m_vpImageView.first()->view()->scene()->addItem(m_PixelRuler.get());
        m_PixelRuler->show();
        m_vpImageView.first()->view()->scene()->update();
    }
}

void MainWindow::onActionSavePerspectiveProjectionMatrix()
{
    m_saveMatrixPath = QFileDialog::getSaveFileName(nullptr, tr("Enter filename or select file"), m_saveMatrixPath, tr("*"));

    QFile qFile(m_saveMatrixPath);
    if (!qFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
            qCritical() << tr("Perspective transformation file %1 wasn't opened").arg(m_saveMatrixPath);
    }

    QTextStream saveStream(&qFile);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            saveStream << m_projMatrix(i, j) << ' ';
        }
        saveStream << '\n';
    }
    saveStream << m_vpImageItems.first()->getImage().width() << ' ';
    saveStream << m_vpImageItems.first()->getImage().height();
}

void MainWindow::onActionLoadPerspectiveProjectionMatrix()
{
    m_saveMatrixPath = QFileDialog::getOpenFileName(this, tr("Choose matrix file"), m_saveMatrixPath, tr("*"));

    QFile qFile(m_saveMatrixPath);
    if (!qFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
            qCritical() << tr("Perspective transformation file %1 wasn't opened").arg(m_saveMatrixPath);
    }

    QTextStream saveStream(&qFile);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            saveStream >> m_projMatrix(i, j);
        }
    }

    QSize outSize;
    int width, height;
    saveStream >> width;
    saveStream >> height;
    outSize.setWidth(width);
    outSize.setHeight(height);
    applyProjectiveTransform(outSize);
}

void MainWindow::applyProjectiveTransform(QSize outSize)
{
    if (!m_vpImageItems.isEmpty() && !m_vpImageItems.first().isNull()) {
        m_vpImageItems.first()->setImage(applyPerspectiveProjection(m_projMatrix, m_image, outSize));
        m_vpImageItems.first()->scene()->update();
    }
}

void MainWindow::onActionProjectiveTransform()
{
    if (m_vpSplitters.count() && !m_RR.isNull()) {
        std::vector<QPointF> outPoints;

        outPoints.push_back(m_RR->getPoints()[0]);

        QPointF Point2 = m_RR->getPoints()[1] - m_RR->getPoints()[0];
        // Point2.setX(Point2.manhattanLength() + m_RR->getPoints()[0].rx());
        Point2.setX(std::hypot(Point2.x(), Point2.y()) + m_RR->getPoints()[0].rx());
        Point2.setY(m_RR->getPoints()[0].ry());

        outPoints.push_back(Point2);

        QPointF Point4 = m_RR->getPoints()[3] - m_RR->getPoints()[0];
        // Point4.setY(Point4.manhattanLength() + m_RR->getPoints()[0].ry());
        Point4.setY(std::hypot(Point4.x(), Point4.y()) + m_RR->getPoints()[0].ry());
        Point4.setX(m_RR->getPoints()[0].rx());

        QPointF Point3 = m_RR->getPoints()[2] - m_RR->getPoints()[1];
        Point3.setX(Point2.rx());
        Point3.setY(Point4.ry());

        outPoints.push_back(Point3);

        // outPoints.push_back(m_RR->getPoints()[1]);
        // outPoints.push_back(m_RR->getPoints()[2]);

        outPoints.push_back(Point4);

        // int width = m_RR->getPoints()[1].rx() - m_RR->getPoints()[0].rx();
        // width
        double alpha = atan2(m_RR->getPoints()[2].ry() - m_RR->getPoints()[1].ry(),
                             m_RR->getPoints()[1].rx() - m_RR->getPoints()[0].rx());

        double sinAlpha = sin(alpha);

        QSize outSize(int(double(m_image.width()) / sinAlpha), m_image.height());
        QImage img = calc_projection_4points(m_projMatrix, m_RR->getPoints(), outPoints, m_image, outSize);
        if (img.isNull()) {
            qCritical() << "calc_projection_4points returned empty image!";
            return;
        }
        m_vpImageItems.first()->setImage(img);
        m_vpImageView.first()->view()->scene()->update();
    }
}
