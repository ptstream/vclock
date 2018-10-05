#-------------------------------------------------
#
# Project created by QtCreator 2018-07-07T13:05:53
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui

TARGET = ds18b20
TEMPLATE = lib
CONFIG += plugin

DEFINES += DS18B20_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += ds18b20.cpp

HEADERS += ds18b20.hpp\
        ds18b20_global.hpp

DISTFILES += \
    ds18b20.json

