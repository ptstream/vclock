#-------------------------------------------------
#
# Project created by QtCreator 2018-07-31T12:31:53
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui

TARGET = xrandrbrightness
TEMPLATE = lib

DEFINES += XRANDRBRIGHTNESS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += xrandrbrightness.cpp

HEADERS += xrandrbrightness.hpp\
        xrandrbrightness_global.hpp

DISTFILES += \
    xrandrbrightness.json
