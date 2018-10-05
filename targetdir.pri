win32:CONFIG(release, debug|release): DESTDIR += ../nixie/release/plugins
else:win32:CONFIG(debug, debug|release): DESTDIR = ../nixie/debug/plugins
else: DESTDIR += ../nixie/plugins