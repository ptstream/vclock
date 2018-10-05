#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T14:08:38
#
#-------------------------------------------------

include(../libvtools.pri)
include(../targetdir.pri)

QT       -= gui
QT       += network

TARGET = api_ipify_org
TEMPLATE = lib
CONFIG += plugin

DEFINES += API_IPIFY_ORG_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += api_ipify_org.cpp

HEADERS += api_ipify_org.hpp\
        api_ipify_org_global.hpp

DISTFILES += \
    api_ipify_org.json

