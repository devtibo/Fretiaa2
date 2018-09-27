#-------------------------------------------------
#
# Project created by QtCreator 2018-07-23T19:23:37
#
#-------------------------------------------------

APP_NAME = \"\\\"Version 2.5.1\\\"\"
# ------------- VERSION --------------
#Application version
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

DEFINES += "APP_NAME=$$APP_NAME"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}
#INCLUDEPATH += ../QFPlot


QT       += core gui multimedia concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Fretiaa2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the follotarget.path = /usr/local/bin/
INSTALLS += targetwing line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        aboutdialog.cpp \
        qcustomplot/qcustomplot.cpp \
    QFPlot/qfgraph.cpp \
    QFPlot/xyrangedialog.cpp \
    QFPlot/qcpitemtracerbars.cpp \
    QFPlot/qcpitemtracercolormap.cpp \
    QFPlot/qfoscillogram.cpp \
    QFPlot/qflevelmeter.cpp \
    QFPlot/qfmultimeter.cpp \
    inputaudiodevicedialog.cpp \
    QFPlot/qfoscillogramroirectangle.cpp \
    gainswidget.cpp

HEADERS += \
        mainwindow.h \
        aboutdialog.h \
        qcustomplot/qcustomplot.h \
        QFPlot/qfplot.h \
    QFPlot/qfgraph.h \
    QFPlot/xyrangedialog.h \
    QFPlot/qcpitemtracerbars.h \
    QFPlot/qcpitemtracercolormap.h \
    QFPlot/qfoscillogram.h \
    QFPlot/qflevelmeter.h \
    QFPlot/qfmultimeter.h \
    inputaudiodevicedialog.h \
    QFPlot/qfoscillogramroirectangle.h \
    gainswidget.h

RESOURCES += \
    ressources.qrc
