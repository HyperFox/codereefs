#-------------------------------------------------
#
# Project created by QtCreator 2014-06-13T23:02:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = code_reefs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    codeedit.cpp

HEADERS  += mainwindow.h \
    codeedit.h

FORMS    += mainwindow.ui

RESOURCES += \
    icon.qrc

OTHER_FILES += \
    readMe.txt
