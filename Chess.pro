#-------------------------------------------------
#
# Project created by QtCreator 2017-05-26T17:44:09
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chess
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    recthread.cpp

HEADERS  += mainwindow.h \
    recthread.h

FORMS    += mainwindow.ui
