#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T03:23:07
#
#-------------------------------------------------

CONFIG	 += c++11 Wall
QT       += widgets network

TARGET	 = ClipShare-Linux
TEMPLATE = app

SOURCES  += main.cpp clipboardcontent.cpp clipsharerunner.cpp tcpclient.cpp
HEADERS  += clipboardcontent.h clipsharerunner.h tcpclient.h

DISTFILES += ../config.cfg


for(FILE, $$DISTFILES) {
    QMAKE_POST_LINK += $(COPY_DIR) $$PWD/FILE $$OUT_PWD
}
