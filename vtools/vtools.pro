#-------------------------------------------------
#
# Project created by QtCreator 2018-06-13T15:04:49
#
#-------------------------------------------------
include(../multimedia.pri)

QT += core gui widgets network xml

TARGET = vtools
TEMPLATE = lib
DEFINES += VTOOLS_LIBRARY

unix{
  CONFIG += staticlib
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    helper.cpp \
    mediaplayer.cpp \
    webradio.cpp \
    nam.cpp \
    alarm.cpp \
    diaporama.cpp \
    lighting.cpp \
    light.cpp \
    gpio.cpp \
    brightness.cpp \
    forecast.cpp \
    genre.cpp \
    station.cpp \
    webradiosinterface.cpp \
    version.cpp \
    tempsensorinterface.cpp \
    weatherinterface.cpp \
    externalipinterface.cpp \
    iplocationinterface.cpp \
    brightnessinterface.cpp \
    brightnesssensorinterface.cpp

HEADERS += \
    helper.hpp \
    mediaplayer.hpp \
    webradio.hpp \
    nam.hpp \
    alarm.hpp \
    diaporama.hpp \
    lighting.hpp \
    light.hpp \
    gpio.hpp \
    brightness.hpp \
    tempsensorinterface.hpp \
    weatherinterface.hpp \
    forecast.hpp \
    vtools_globals.h \
    iplocationinterface.hpp \
    iplocationinterface.hpp \
    externalipinterface.hpp \
    iplocationinterface.hpp \
    webradiosinterface.hpp \
    genre.hpp \
    station.hpp \
    version.hpp \
    brightnessinterface.hpp \
    brightnesssensorinterface.hpp

win32:CONFIG(release, debug|release): DESTDIR += ../nixie/release/
else:win32:CONFIG(debug, debug|release): DESTDIR = ../nixie/debug/
else:unix:DESTDIR = ../nixie/
