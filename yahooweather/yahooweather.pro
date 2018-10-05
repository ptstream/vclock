
include(../libvtools.pri)
include(../targetdir.pri)

QT       += core gui network

TARGET = yahooweather
TEMPLATE = lib
CONFIG += plugin

DEFINES += YAHOOWEATHER_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += yahooweather.cpp

HEADERS += yahooweather.hpp\
        yahooweather_global.hpp

RESOURCES += \
    yahooicons.qrc

DISTFILES += \
    yahooweather.json
