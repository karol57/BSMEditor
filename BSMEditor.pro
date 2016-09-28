#-------------------------------------------------
#
# Project created by QtCreator 2016-04-25T10:46:24
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SDNW_BSMEditor
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    qbsmmodel.cpp \
    AddLanguageDialog.cpp \
    BSMFile.det.cpp \
    qbsmfile.cpp

HEADERS  += MainWindow.hpp \
    File.hpp \
    BSMFile.det.hpp \
    qbsmmodel.hpp \
    AddLanguageDialog.hpp \
    qbsmfile.hpp

FORMS    += MainWindow.ui \
    AddLanguageDialog.ui

RESOURCES += \
    resources.qrc
