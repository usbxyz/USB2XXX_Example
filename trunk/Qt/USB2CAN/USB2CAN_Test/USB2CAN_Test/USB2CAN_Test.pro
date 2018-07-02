#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T16:17:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USB2CAN_Test
TEMPLATE = app

win32{
    LIBS += -L$$PWD/lib/msvc_x86/ -lUSB2XXX
}else{
    unix:contains(QMAKE_HOST.arch, x86_64){
        LIBS += -L$$PWD/lib/Linux_x64/ -lUSB2XXX
    }else{
        LIBS += -L$$PWD/lib/Linux_x86/ -lUSB2XXX
    }
}
!win32: DEFINES += OS_UNIX


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += \
    mainwindow.h \
    usb_device.h \
    usb2can.h

FORMS    += mainwindow.ui
