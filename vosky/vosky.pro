#-------------------------------------------------
#
# Project created by QtCreator 2010-09-21T16:19:00
#
#-------------------------------------------------

QT       += core gui network

TARGET = vosky
TEMPLATE = app
config += debug
QMAKE_CXXFLAGS += -g
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp
QT += network

CONFIG -= embed_manifest_exe
CONFIG -= embed_manifest_dll

QTSAPP_HEADERS = ../qtsingleapplication/qtsingleapplication.h ../qtsingleapplication/qtlocalpeer.h
QTSAPP_SOURCES = ../qtsingleapplication/qtsingleapplication.cpp ../qtsingleapplication/qtlocalpeer.cpp

SOURCES += main.cpp ../kitapplication.cpp ../metauri.cpp \
        vosky.cpp ./kitpstn.cpp ../volctl.cpp \
        ../skypetracer.cpp ../skypetunnel.cpp

HEADERS  += vosky.h ../kitapplication.h ../metauri.h \
         ../skypetracer.h ../skypetunnel.h

FORMS    += vosky.ui ../skypetracer.ui

SOURCES += $$QTSAPP_SOURCES
HEADERS += $$QTSAPP_HEADERS

## libskynet
include(../skynet/libskynet.pri)

INCLUDEPATH += ../ ../qtsingleapplication/ ../skynet/
# INCLUDEPATH += ./qtsingleapplication ./skynet /serv/stow/pjsip/include
# LIBS += -L/serv/stow/pjsip/lib

# LIBS += -lm \
#    -lpthread \
#    -lssl

# PJSIP_LIBS=$$system("cat /serv/stow/pjsip/lib/pkgconfig/libpjproject.pc | grep Libs | awk -F} '{print $2}'")
# message($$PJSIP_LIBS)

# LIBS += $$PJSIP_LIBS