#-------------------------------------------------
#
# Project created by QtCreator 2018-08-01T09:23:48
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui

TARGET = rpigpiobrightness
TEMPLATE = lib

DEFINES += RPIGPIOBRIGHTNESS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += rpigpiobrightness.cpp

HEADERS += rpigpiobrightness.hpp\
        rpigpiobrightness_global.hpp

DISTFILES += \
    rpigpiobrightness.json
