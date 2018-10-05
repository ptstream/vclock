!contains(QMAKE_HOST.arch, armv7l):{
    QT += multimedia
    DEFINES += QTMULTIMEDIA_DEFINED
}
