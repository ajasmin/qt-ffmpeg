#-------------------------------------------------
#
# Project created by QtCreator 2013-11-21T14:10:35
#
#-------------------------------------------------


QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH = /usr/local/include /usr/include

LIBS += -lboost_filesystem -lboost_system -pthread -L/usr/local/lib -lavfilter -lswresample -lswscale -lavformat -lavcodec -ldl -lz -lavutil -lm

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-ffmpeg
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
