#-------------------------------------------------
#
# Project created by QtCreator 2018-02-02T08:52:09
#
#-------------------------------------------------
include(../multimedia.pri)
include(../libvtools.pri)

QT += core gui opengl widgets network xml

TARGET = nixie
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    mesh.cpp \
    config.cpp \
    stopwatchslots.cpp \
    menu.cpp \
    slots.cpp \
    about.cpp \
    audioslots.cpp \
    weatherslots.cpp \
    alarmslots.cpp \
    timerslots.cpp \
    brightnessslots.cpp \
    viewslots.cpp \
    modeslots.cpp \
    optionslots.cpp \
    nixie.cpp \
    alarmmanager.cpp \
    localisation.cpp \
    timermanager.cpp \
    webradiosmanager.cpp \
    listwidgetitemmanager.cpp \
    painttext.cpp \
    paintnixietubes.cpp \
    displaymanager.cpp \
    paintlcd.cpp

HEADERS  += mainwindow.hpp \
    about.hpp \
    nixie.hpp \
    alarmmanager.hpp \
    localisation.hpp \
    timermanager.hpp \
    webradiosmanager.hpp \
    listwidgetitemmanager.hpp \
    displaymanager.hpp

FORMS    += mainwindow.ui \
    about.ui \
    alarmmanager.ui \
    localisation.ui \
    timermanager.ui \
    webradiosmanager.ui \
    displaymanager.ui

DISTFILES += \
    config/nixie.ini \
    dep.sh \
    dep.cmd \
    nixie.rc

RC_FILE += \
    nixie.rc

RESOURCES += \
    shader.qrc \
    icons.qrc

win32-g++:CONFIG(release, debug|release): LIBS += ../vmode/release/libvmode.a
else:win32-g++:CONFIG(debug, debug|release): LIBS += ../vmode/debug/libvmode.a
else:win32:!win32-g++:CONFIG(release, debug|release): LIBS += ../vmode/release/vmode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += ../vmode/debug/vmode.lib
else:unix: LIBS += ../vmode/libvmode.a

# Copy files needed.
win32 {
CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release/
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug/
DESTDIR=$$replace(DESTDIR, /, \\)
_WIN_PRO_FILE_PWD_=$$replace(_PRO_FILE_PWD_, /, \\)
QMAKE_POST_LINK = $$_PRO_FILE_PWD_\dep.cmd $$_WIN_PRO_FILE_PWD_ $$DESTDIR
}

#Copy files needed.
unix {
QMAKE_POST_LINK = $$_PRO_FILE_PWD_/dep.sh $$_PRO_FILE_PWD_ $$OUT_PWD
}

#Copy files needed.
macx {
QMAKE_POST_LINK=$$_PRO_FILE_PWD_/dep.sh $$_PRO_FILE_PWD_ $$OUT_PWD/nixie.app/Contents/MacOS
}

macx:ICON=$$PWD/icons/nixie64.icns
