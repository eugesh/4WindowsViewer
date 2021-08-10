#include <QAction>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QHBoxLayout>
#include <QPushButton>
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

static const bool DEBUG1 = true;

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

    // ToolBars
    createControlPtsToolBar();

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
    connect(ui->actionPointInput, &QAction::triggered, this, &MainWindow::onActionPointInput);
}

MainWindow::~MainWindow()
{
    removeAdditionalWindows();
    delete ui;
}

cv::Mat MainWindow::readMtxFileStorage(const QString &path, const QString mtxName) // ToDo: move to lib
{
    cv::Mat mat;
    cv::FileStorage fs;

    fs.open(path.toStdString(), cv::FileStorage::READ);

    fs[mtxName.toStdString()] >> mat;

    fs.release();

    return mat;
}

void MainWindow::createControlPtsToolBar()
{
    m_controlPtToolBar = new QToolBar("Control Points", this);
    m_controlPtToolBar->setToolTip("Control Points Toolbar");

    QPushButton *loadLeftBtn = new QPushButton("Left");
    QPushButton *loadRightBtn = new QPushButton("Right");

    m_controlPtToolBar->addWidget(loadLeftBtn);
    m_controlPtToolBar->addWidget(loadRightBtn);
    loadLeftBtn->setToolTip("Load left image");
    loadLeftBtn->setToolTip("Load right image");

    connect(loadLeftBtn, &QAbstractButton::pressed, [this]() {
        if (m_vpImageItems.size() != 2) return;
        static QString fullFilePath = "/home";
        fullFilePath = QFileDialog::getOpenFileName(this, tr("Choose image file"), fullFilePath,
                      tr("Images (*.png *.bmp *.tif *.tiff *.gif *.xpm *.jpg *.jpeg *.JPG)"));

        m_vpImageItems[0]->setImage(QImage(fullFilePath));

        m_vpImageView[0]->view()->scene()->update();
    });

    connect(loadRightBtn, &QAbstractButton::pressed, [this]() {
        if (m_vpImageItems.size() != 2) return;

        static QString fullFilePath = "/home";
        fullFilePath = QFileDialog::getOpenFileName(this, tr("Choose image file"), fullFilePath,
                      tr("Images (*.png *.bmp *.tif *.tiff *.gif *.xpm *.jpg *.jpeg *.JPG)"));

        m_vpImageItems[1]->setImage(QImage(fullFilePath));

        m_vpImageView[1]->view()->scene()->update();
    });

    QPushButton *loadLeftCamMtxBtn = new QPushButton("CamML");
    QPushButton *loadRightCamMtxBtn = new QPushButton("CamMR");

    m_controlPtToolBar->addWidget(loadLeftCamMtxBtn);
    m_controlPtToolBar->addWidget(loadRightCamMtxBtn);
    loadLeftCamMtxBtn->setToolTip("Load Camera matrix and distrotion coefficients for the left camera");
    loadLeftCamMtxBtn->setToolTip("Load Camera matrix and distrotion coefficients for the right camera");

    connect(loadLeftCamMtxBtn, &QAbstractButton::pressed, [this]() {
        static QString fullFilePath = "/home";
        fullFilePath = QFileDialog::getOpenFileName(this, tr("Choose YAML file with Camera Matrix and distortion coefficients for the left image"), fullFilePath,
                      tr("File Storage YAML (*.yml *.yaml)"));

        if (fullFilePath.isEmpty()) return;

        m_cameraMtx[0] = readMtxFileStorage(fullFilePath, "CameraMatrix");
        m_distCoeffs[0] = readMtxFileStorage(fullFilePath, "DistCoeffs");

        if (DEBUG1) {
            std::cout << "m_cameraMtx[0] = " << m_cameraMtx[0] << std::endl;
            std::cout << "m_distCoeffs[0] = " << m_distCoeffs[0] << std::endl;
        }
    });

    connect(loadRightCamMtxBtn, &QAbstractButton::pressed, [this]() {
        static QString fullFilePath = "/home";
        fullFilePath = QFileDialog::getOpenFileName(this, tr("Choose YAML file with Camera Matrix and distortion coefficients for the right image"), fullFilePath,
                      tr("File Storage YAML (*.yml *.yaml)"));

        if (fullFilePath.isEmpty()) return;

        m_cameraMtx[1] = readMtxFileStorage(fullFilePath, "CameraMatrix");
        m_distCoeffs[1] = readMtxFileStorage(fullFilePath, "DistCoeffs");

        if (DEBUG1) {
            qDebug() << "m_cameraMtx[1] = " << m_cameraMtx[1].data;
            std::cout << "m_distCoeffs[1] = " << m_distCoeffs[1] << std::endl;
        }
    });

    // Save/Load Control Points
    QPushButton *saveBtn = new QPushButton("Save");
    m_controlPtToolBar->addWidget(saveBtn);
    saveBtn->setToolTip("Save Control Points");
    connect(saveBtn, &QAbstractButton::pressed, [this]() {
        static QString fullFilePath = tr("/home");
        fullFilePath = QFileDialog::getSaveFileName(nullptr, tr("Enter filename or select file"),
                                                    fullFilePath, tr("CSV (*.csv)"));
        saveControlPoints(fullFilePath);
    });

    QPushButton *loadBtn = new QPushButton("Load");
    m_controlPtToolBar->addWidget(loadBtn);
    loadBtn->setToolTip("Load Control Points");
    connect(loadBtn, &QAbstractButton::pressed, [this]() {
        static QString fullFilePath = "/home";
        fullFilePath = QFileDialog::getOpenFileName(this, tr("Enter filename or select file"), fullFilePath,
                      tr("CSV (*.csv)"));

        loadControlPoints(fullFilePath);
    });

    // Matching with Control Points
    QPushButton *regBtn = new QPushButton("reg");
    m_controlPtToolBar->addWidget(regBtn);
    regBtn->setToolTip("Calculate Homography and Registrate images pair");
    connect(regBtn, &QAbstractButton::pressed, [this]() {
        registerLeftRight();
    });

    // ToDo: Feature points matching
    QPushButton *matchBtn = new QPushButton("match");
    m_controlPtToolBar->addWidget(matchBtn);
    matchBtn->setToolTip("Calculate Homography and Registrate images pair");
    connect(matchBtn, &QAbstractButton::pressed, []() {

    });

    addToolBar(Qt::TopToolBarArea, m_controlPtToolBar);
    m_controlPtToolBar->hide();
}

void MainWindow::registerLeftRight()
{
    if (m_vpImageView.size() != 2)
        return;

    // Find Homography with Control Points
    QMatrix3x3 H = estimateHomographyProjection(m_vpImageView.first()->getControlPointsSorted(), m_vpImageView.last()->getControlPointsSorted());

    if (DEBUG1)
        std::cout << "H: " << H.constData() << std::endl;

    // Apply Homography
    QMatrix3x3 matrix;
    QImage img_warp = calc_projection_4points(matrix,
                                              m_vpImageView.first()->getControlPointsSorted(),
                                              m_vpImageView.last()->getControlPointsSorted(),
                                              m_vpImageItems.first()->getImage(),
                                              m_vpImageItems.first()->getImage().size());

    if (DEBUG1)
        std::cout << "matrix: " << matrix.constData() << std::endl;

    // Show on the first Image View ()
    QImage mixed = mixChannels(img_warp, m_vpImageItems.last()->getImage());
    m_item->setImage(mixed);

}

bool MainWindow::loadControlPoints(const QString &fp)
{
    if (m_vpImageView.size() != 2) {
        return false;
    }
    QFile qFile(fp);
    if (!qFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
            qCritical() << tr("Control points file %1 wasn't opened for read").arg(fp);
    }

    QTextStream saveStream(&qFile);

    while (!saveStream.atEnd()) {
        QPointF pts[2];
        auto line = saveStream.readLine();
        auto words = line.split(";");
        pts[0] = QPointF(words[1].toDouble(), words[2].toDouble());
        pts[1] = QPointF(words[3].toDouble(), words[4].toDouble());
        addPointPair(words[0], pts);
    }

    return true;
}

bool MainWindow::saveControlPoints(const QString &fp)
{
    if (m_vpImageView.size() != 2) {
        return false;
    }

    QFile qFile(fp);
    if (!qFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
            qCritical() << tr("Controlo points file %1 wasn't opened for write").arg(fp);
    }

    QTextStream saveStream(&qFile);

    auto item_list1 = m_vpImageView[0]->view()->scene()->items();
    auto item_list2 = m_vpImageView[1]->view()->scene()->items();

    foreach (auto item1, item_list1) {
        PointItem *pi1 = nullptr;
        pi1 = qgraphicsitem_cast<PointItem*>(item1);
        if (pi1) {
            foreach (auto item2, item_list2) {
                PointItem *pi2 = nullptr;
                pi2 = qgraphicsitem_cast<PointItem*>(item2);
                if (pi2 && pi1->id() == pi2->id()) {
                    saveStream << pi1->text() << ";" // QString::number(pi1->id())
                               << pi1->coord().x() << ";" << pi1->coord().y() << ";"
                               << pi2->coord().x() << ";" << pi2->coord().y() << "\n";
                }
            }
        }
    }

    return true;
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

void MainWindow::on3WindowsCheck(int check)
{
    if (check) {
        removeAdditionalWindows();
        ui->action2Windows->setChecked(false);
        ui->action4Windows->setChecked(false);
        if (m_vpSplitters.empty())
            create3Windows();
        else
            show3Windows();
        if (m_vpImageItems.empty())
            openImages3Windows();
    } else {
        hide3Windows();
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

void MainWindow::create3Windows()
{
    QSharedPointer<QSplitter> h1Splitter = QSharedPointer<QSplitter>(new QSplitter(this));

    QSharedPointer<QSplitter> vSplitter = QSharedPointer<QSplitter> (new QSplitter(this));
    vSplitter->setOrientation(Qt::Vertical);

    vSplitter->addWidget(m_view.get());
    vSplitter->addWidget(h1Splitter.get());

    m_vpImageView.push_back(QSharedPointer<ImageView>(new ImageView("Bottom left view", this)));
    QGraphicsScene *scene = new QGraphicsScene(this);
    m_vpImageView.last()->view()->setScene(scene);
    h1Splitter->addWidget(m_vpImageView.last().get());

    m_vpImageView.push_back(QSharedPointer<ImageView>(new ImageView("Bottom right view", this)));
    QGraphicsScene *scene2 = new QGraphicsScene(this);
    m_vpImageView.last()->view()->setScene(scene2);
    h1Splitter->addWidget(m_vpImageView.last().get());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(vSplitter.get());
    setLayout(layout);
    setCentralWidget(vSplitter.get());

    for (int i = 0; i < m_vpImageView.size(); ++i) {
        connect(m_vpImageView[i].get(), &ImageView::scaleChanged, this, &MainWindow::scaleChanged);
        connect(m_vpImageView[i].get(), &ImageView::angleChanged, this, &MainWindow::angleChanged);
        connect(m_vpImageView[i].get(), &ImageView::pointAdded, this, &MainWindow::onPointAdded);
        connect(m_vpImageView[i].get(), &ImageView::removeControlPoints, this, &MainWindow::onRemoveControlPoints);
        connect(this, &MainWindow::scaleChanged, m_vpImageView[i].get(), &ImageView::zoomIn);
        connect(this, &MainWindow::angleChanged, m_vpImageView[i].get(), &ImageView::rotate);
    }

    m_vpSplitters.push_back(h1Splitter);
    m_vpSplitters.push_back(vSplitter);
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

void MainWindow::onRemoveControlPoints(const std::vector<quint32> &ids)
{
    foreach(auto view, m_vpImageView)
        view->removePoints(ids);
}

// ToDo: combine code of addPointPair and onPointAdded together
void MainWindow::addPointPair(const QString &pointName, const QPointF pts[2])
{
    if (m_vpImageView.size() != 2)
        return;

    quint32 newId;
    if (m_pointsIds.empty()) {
        newId = 0;
    } else {
        newId = m_pointsIds.last() + 1;
    }
    m_pointsIds.push_back(newId);

    for (int i = 0; i < m_vpImageView.size(); ++i) {
        PointItem *pi = new PointItem("ControlPoint", newId, this, m_vpImageItems[i].get());

        pi->setPos(pts[i]);
        pi->setZValue(0);
        pi->setText(pointName);
        m_vpImageView[i]->view()->scene()->addItem(pi);
    }

    // Change text on both points after mouse double click
    auto item_list1 = m_vpImageView[0]->view()->scene()->items();
    auto item_list2 = m_vpImageView[1]->view()->scene()->items();
    foreach (auto item1, item_list1) {
        PointItem *pi1 = nullptr;
        pi1 = qgraphicsitem_cast<PointItem*>(item1);
        if (pi1) {
            foreach (auto item2, item_list2) {
                PointItem *pi2 = nullptr;
                pi2 = qgraphicsitem_cast<PointItem*>(item2);
                if (pi2 && pi1->id() == pi2->id()) {
                    connect(pi1, &PointItem::textChanged, pi2, &PointItem::setText);
                    connect(pi2, &PointItem::textChanged, pi1, &PointItem::setText);
                }
            }
        }
    }
}

void MainWindow::onPointAdded(const QPoint &point)
{
    if (m_vpImageView.size() != 2)
        return;

    auto sender = QObject::sender();

    quint32 newId;
    if (m_pointsIds.empty()) {
        newId = 0;
    } else {
        newId = m_pointsIds.last() + 1;
    }
    m_pointsIds.push_back(newId);

    for (int i = 0; i < m_vpImageView.size(); ++i) {
        PointItem *pi = new PointItem("ControlPoint", newId, this, m_vpImageItems[i].get());

        QPoint origin = m_vpImageView[i]->view()->mapFromGlobal(QCursor::pos());

        if (qobject_cast<ImageView*>(sender))
            origin = qobject_cast<ImageView*>(sender)->view()->mapFromGlobal(QCursor::pos());

        QPointF relativeOrigin = m_vpImageView[i]->view()->mapToScene(origin) - m_vpImageItems[i]->pos();

        pi->setPos(relativeOrigin);
        pi->setZValue(0);
        m_vpImageView[i]->view()->scene()->addItem(pi);
    }

    // Change text on both points after mouse double click
    auto item_list1 = m_vpImageView[0]->view()->scene()->items();
    auto item_list2 = m_vpImageView[1]->view()->scene()->items();
    foreach (auto item1, item_list1) {
        PointItem *pi1 = nullptr;
        pi1 = qgraphicsitem_cast<PointItem*>(item1);
        if (pi1) {
            foreach (auto item2, item_list2) {
                PointItem *pi2 = nullptr;
                pi2 = qgraphicsitem_cast<PointItem*>(item2);
                if (pi2 && pi1->id() == pi2->id()) {
                    connect(pi1, &PointItem::textChanged, pi2, &PointItem::setText);
                    connect(pi2, &PointItem::textChanged, pi1, &PointItem::setText);
                }
            }
        }
    }
}

void MainWindow::onActionPointInput()
{
    m_controlPtToolBar->show();
    on3WindowsCheck(1);
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

void MainWindow::openImages3Windows()
{
    openImages4Windows();
}

void MainWindow::show3Windows()
{

}

void MainWindow::hide3Windows()
{

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

    foreach (auto t, m_vpImageItems)
        if (!t.isNull())
            t.clear();

    if (m_vpImageItems.count())
        m_vpImageItems.clear();

    foreach (auto t, m_vpImageView)
        if (!t.isNull())
            t.clear();

    if (m_vpImageView.count())
        m_vpImageView.clear();

    foreach (auto t, m_vpSplitters)
        if (!t.isNull())
            t.clear();

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
