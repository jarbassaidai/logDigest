#-------------------------------------------------
#
# Project created by QtCreator 2016-12-14T09:24:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = logDigest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filebrowser.cpp \
    tablayout.cpp \
    highlighter.cpp

HEADERS  += mainwindow.h \
    filebrowser.h \
    tablayout.h \
    highlighter.h

FORMS    += mainwindow.ui
