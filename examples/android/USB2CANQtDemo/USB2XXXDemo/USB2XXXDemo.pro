#-------------------------------------------------
#
# Project created by QtCreator 2018-05-09T15:53:57
#
#-------------------------------------------------

QT      += core gui
QT      += core-private


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USB2XXXDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    LIBS += -L$$PWD/libs/win32/ -lUSB2XXX
}

unix{
    QT      += androidextras
    DEFINES += OS_ANDROID
    DEFINES += OS_UNIX
    LIBS += -L$$PWD/libs/armeabi-v7a/ -lUSB2XXX -lusb
}




SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialogcansetting.cpp

HEADERS += \
        mainwindow.h \
    usb_device.h \
    dialogcansetting.h

FORMS += \
        mainwindow.ui \
    dialogcansetting.ui

CONFIG += mobility
MOBILITY = 

unix{
    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$PWD/libs/armeabi-v7a/libusb.so \
            $$PWD/libs/armeabi-v7a/libUSB2XXX.so
    }

    DISTFILES += \
        java_source/java/com/usbxyz/USBDeviceActivity.java \
        java_source/AndroidManifest.xml \
        java_source/gradle/wrapper/gradle-wrapper.jar \
        java_source/gradlew \
        java_source/res/values/libs.xml \
        java_source/build.gradle \
        java_source/gradle/wrapper/gradle-wrapper.properties \
        java_source/gradlew.bat

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/java_source
}
