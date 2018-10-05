#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T15:39:23
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui
QT       += network

TARGET = ip_api_com
TEMPLATE = lib
CONFIG += plugin

DEFINES += IP_API_COM_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += ip_api_com.cpp

HEADERS += ip_api_com.hpp\
        ip_api_com_global.hpp

DISTFILES += \
    ip_api_com.json
