#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T03:23:07
#
#-------------------------------------------------

CONFIG	 += c++11 Wall
QT       += widgets network gui

RESOURCES     = clipshare.qrc

TARGET	 = ClipShare

SOURCES  += main.cpp clipsharerunner.cpp tcpclient.cpp statuswindow.cpp \
    applicationsettings.cpp \
    clipboardjsonformatter.cpp
HEADERS  += clipsharerunner.h tcpclient.h statuswindow.h \
    applicationsettings.h \
    clipboardjsonformatter.h

DISTFILES += ../config.cfg

for(FILE, $$DISTFILES) {
    QMAKE_POST_LINK += $(COPY_DIR) $$PWD/FILE $$OUT_PWD
}

FORMS += statuswindow.ui


