
include(../libvtools.pri)
include(../targetdir.pri)

QT       += network xml

TARGET = shoutcast
TEMPLATE = lib
CONFIG += plugin

DEFINES += SHOUTCAST_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += shoutcast.cpp

HEADERS += shoutcast.hpp\
        shoutcast_global.hpp

RESOURCES += \
    shoutcasticons.qrc

DISTFILES += \
    shoutcast.json
