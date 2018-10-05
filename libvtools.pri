
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += ../nixie/release/libvtools.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += ../nixie/debug/libvtools.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += ../nixie/release/vtools.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += ../nixie/debug/vtools.lib
else:unix: PRE_TARGETDEPS += ../nixie/libvtools.a

win32-g++:CONFIG(release, debug|release): LIBS += ../nixie/release/libvtools.a
else:win32-g++:CONFIG(debug, debug|release): LIBS += ../nixie/debug/libvtools.a
else:win32:!win32-g++:CONFIG(release, debug|release): LIBS += ../nixie/release/vtools.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += ../nixie/debug/vtools.lib
else:unix: LIBS += ../nixie/libvtools.a
