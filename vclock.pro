TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += vtools \
    vmode \
    yahooweather \
    api_ipify_org \
    ip_api_com \
    shoutcast

contains(QMAKE_HOST.arch, armv7l):{
SUBDIRS += ds18b20 \
    rpigpiobrightness \
    rpiblbrightness \
}

contains(QMAKE_HOST.os, Windows):{
SUBDIRS += winbrightness
}

contains(QMAKE_HOST.os, Linux):{
SUBDIRS += xrandrbrightness
}

SUBDIRS += nixie

TRANSLATIONS = $$PWD/languages/nixie_fr.ts

message (Build nixie for $$QMAKE_HOST.os ($$QMAKE_HOST.arch))


