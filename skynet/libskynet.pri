QT *= core gui

SKYNET_HEADERS = $$PWD/xmessages.h $$PWD/skypecommon.h $$PWD/skypecommand.h \
               $$PWD/skype.h $$PWD/skypeobject.h
SKYNET_SOURCES = $$PWD/xmessages.cpp $$PWD/skypecommon_X11.cpp $$PWD/skypecommand.cpp \
               $$PWD/skype.cpp $$PWD/skypeobject.cpp $$PWD/skypecommon_win.cpp \
               $$PWD/skypecommon_com.cpp $$PWD/skypecommon_rawapi.cpp 

HEADERS += $$SKYNET_HEADERS
SOURCES += $$SKYNET_SOURCES