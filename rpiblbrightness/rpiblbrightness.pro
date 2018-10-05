#-------------------------------------------------
#
# Project created by QtCreator 2018-07-31T12:31:53
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui

TARGET = rpiblbrightness
TEMPLATE = lib

DEFINES += RPIBLBRIGHTNESS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += rpiblbrightness.cpp

HEADERS += rpiblbrightness.hpp\
        rpiblbrightness_global.hpp

DISTFILES += \
    rpiblbrightness.json
