#-------------------------------------------------
#
# Project created by QtCreator 2019-04-06T10:56:32
#
#-------------------------------------------------

QT += core gui widgets

qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl

TARGET = ImageViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


windows: INCLUDEPATH += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv
windows: INCLUDEPATH += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/include
#windows: release: LIBS += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/x64/vc15/bin/opencv_world454.dll
windows: release: INCLUDEPATH += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/x64/vc15/bin/
windows: release: DEPENDPATH += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/x64/vc15/bin/
windows: release: LIBS += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv/lib/Release/opencv_world454.lib
#windows: release: DEPENDPATH += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv/lib/Release/
#windows: debug: LIBS += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv/bin/Debug/opencv_world454d.dll
#windows: release: LIBS += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv/bin/Release/opencv_world454.dll
#windows: debug: LIBS += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/x64/vc15/bin/opencv_world454d.dll
windows: debug: INCLUDEPATH += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/x64/vc15/bin/
windows: debug: DEPENDPATH += D:/opencv/ocv_4_1/vc15/Install_vs15/opencv/x64/vc15/bin/
windows: debug: LIBS += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv/lib/Debug/opencv_world454d.lib
#windows: debug: DEPENDPATH += D:/opencv/ocv_4_1/vc15/Build_vs15/opencv/lib/Debug/

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        image-graphics-scene.cpp \
        image-item.cpp \
        imageprocessor.cpp \
        image-view.cpp \
        pixelruler.cpp \
        pointitem.cpp \
        RubberRect.cpp

HEADERS += \
        mainwindow.h \
        image-graphics-scene.h \
        image-item.h \
        imageprocessor.cpp \
        image-view.h \
        pixelruler.h \
        pointitem.h \
        RubberRect.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
