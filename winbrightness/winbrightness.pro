#-------------------------------------------------
#
# Project created by QtCreator 2018-07-31T12:31:53
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui

TARGET = winbrightness
TEMPLATE = lib

DEFINES += WINBRIGHTNESS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += winbrightness.cpp

HEADERS += winbrightness.hpp\
        winbrightness_global.hpp

DISTFILES += \
    winbrightness.json
