#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T03:23:07
#
#-------------------------------------------------

CONFIG	 += c++11
QT       += widgets network gui

RESOURCES     = clipshare.qrc

TARGET	 = ClipShare

SOURCES  += main.cpp clipsharerunner.cpp statuswindow.cpp \
    applicationsettings.cpp \
    clipboardjsonformatter.cpp \
    networkmanager.cpp

HEADERS  += clipsharerunner.h statuswindow.h \
    applicationsettings.h \
    clipboardjsonformatter.h \
    networkmanager.h \
    miniz.c \
    messagetype.h

DISTFILES += ../config.cfg
QMAKE_LFLAGS += -static-libgcc -static-libstdc++

for(FILE, $$DISTFILES) {
    QMAKE_POST_LINK += $(COPY_DIR) $$PWD/FILE $$OUT_PWD
}

FORMS += statuswindow.ui


