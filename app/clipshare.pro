#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T03:23:07
#
#-------------------------------------------------

CONFIG	 += c++11
QT       += widgets network gui sql

RESOURCES     = clipshare.qrc

TARGET	 = clipshare

SOURCES  += main.cpp \
    clipshare.cpp \
    networkio.cpp \
    aggregator.cpp \
    settings.cpp \
    window.cpp \
    status.cpp

HEADERS  += \
    miniz.c \
    clipshare.h \
    settings.h \
    aggregator.h \
    networkio.h \
    window.h \
    status.h

#LIBS += -L/usr/local/lib -lsqlite3

FORMS += statuswindow.ui


