
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += ../vmode/release/libvmode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += ../vmode/debug/libvmode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += ../vmode/release/vmode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += ../vmode/debug/vmode.lib
else:unix: PRE_TARGETDEPS += ../vmode/libvmode.a

win32-g++:CONFIG(release, debug|release): LIBS += ../vmode/release/libvmode.a
else:win32-g++:CONFIG(debug, debug|release): LIBS += ../vmode/debug/libvmode.a
else:win32:!win32-g++:CONFIG(release, debug|release): LIBS += ../vmode/release/vmode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += ../vmode/debug/vmode.lib
else:unix: LIBS += ../vmode/libvmode.a
