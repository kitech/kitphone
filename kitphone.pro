# kitphone.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-09-21 16:19:00 +0800
# Version: $Id: kitphone.pro 858 2011-05-01 16:33:57Z drswinghead $
# 

QT       += core gui network declarative sql
TARGET = kitphone
TEMPLATE = app
config += debug console qaxcontainer link_pkgconfig
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

VERSION = 0.4

!win32 {
QMAKE_CXXFLAGS += -g -std=c++0x
}

QTSAPP_HEADERS = ./qtsingleapplication/qtsingleapplication.h ./qtsingleapplication/qtlocalpeer.h
QTSAPP_SOURCES = ./qtsingleapplication/qtsingleapplication.cpp ./qtsingleapplication/qtlocalpeer.cpp

SOURCES += main.cpp kitapplication.cpp metauri.cpp \
        kitphone.cpp \
        sipaccountswindow.cpp sipaccountpropertieswindow.cpp \
        sipaccount.cpp \
        skypetracer.cpp skypetunnel.cpp volctl.cpp \
        PjCallback.cpp \
        asyncdatabase.cpp \
        databaseworker.cpp \
        log.cpp

HEADERS  += kitphone.h kitapplication.h metauri.h \
         sipaccountswindow.h sipaccountpropertieswindow.h \
         sipaccount.h \
         skypetracer.h skypetunnel.h \
         PjCallback.h \
        asyncdatabase.h \
        databaseworker.h \
        log.h

FORMS    += kitphone.ui skypetracer.ui sipaccountswindow.ui sipaccountpropertieswindow.ui

#######
SOURCES += sipphone.cpp
HEADERS += sipphone.h
FORMS += sipphone.ui

#######
SOURCES += skypephone.cpp websocketclient.cpp phonecontact.cpp phonecontactproperty.cpp \
        groupinfodialog.cpp contactmodel.cpp callhistorymodel.cpp \
        networkdetect.cpp
HEADERS += skypephone.h websocketclient.h phonecontact.h phonecontactproperty.h \
        groupinfodialog.h contactmodel.h callhistorymodel.h \ 
        networkdetect.h
FORMS += skypephone.ui phonecontactproperty.ui groupinfodialog.ui

SOURCES += $$QTSAPP_SOURCES
HEADERS += $$QTSAPP_HEADERS
RESOURCES += kp.qrc

# libwss
DEFINES += DEBUG
#QMAKE_CFLAGS += -include ./libwss/wss_config.h
INCLUDEPATH += ./libwss
win32 {
INCLUDEPATH += ./libwss/win32helpers Z:/librarys/vc-zlib/include Z:/cross/boost_1_46_1
} else {

}

SOURCES += ./libwss/base64-decode.c  \
             ./libwss/extension.c     \
            ./libwss/handshake.c     \
            ./libwss/md5.c     \
            ./libwss/sha-1.c         \
            ./libwss/client-handshake.c  \
            ./libwss/extension-deflate-stream.c  \
            ./libwss/libwebsockets.c  \
            ./libwss/parsers.c 
win32 {
    SOURCES +=  ./libwss/win32helpers/gettimeofday.c
}

## libskynet
include(./skynet/libskynet.pri)

INCLUDEPATH += ./qtsingleapplication ./skynet /serv/stow/pjsip/include
LIBS += -L/serv/stow/pjsip/lib


!win32 {
   LIBS += -lssl
   #PJSIP_LIBS=$$system("cat /serv/stow/pjsip/lib/pkgconfig/libpjproject.pc | grep Libs | awk -F} '{print $2}'")
   #system("export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/serv/stow/pjsip/lib/pkgconfig")
   #PKGCONFIG += libpjproject
   PJSIP_LIBS=$$system("./pkg_config.sh")
   message($$PJSIP_LIBS)
   LIBS += $$PJSIP_LIBS -lg729codec-x86_64-unknown-linux-gnu -lsilkcodec-x86_64-unknown-linux-gnu
}

win32 {
    CONFIG -= embed_manifest_exe
    CONFIG -= embed_manifest_dll

    INCLUDEPATH += . Z:/librarys/pjsip/include
    LIBS += -LZ:/librarys/pjsip/lib

    LIBS += -lws2_32 -lole32 -luser32
    LIBS += -llibg7221codec-i386-win32-vc6-release \
    -llibgsmcodec-i386-win32-vc6-release \
    -llibilbccodec-i386-win32-vc6-release \
    -llibmilenage-i386-win32-vc6-release \
    -llibportaudio-i386-win32-vc6-release \
    -llibresample-i386-win32-vc6-release \
    -llibspeex-i386-win32-vc6-release \
    -llibsrtp-i386-win32-vc6-release \
    -lpjlib-i386-win32-vc6-release \
    -lpjlib-util-i386-win32-vc6-release \
    -lpjmedia-audiodev-i386-win32-vc6-release \
    -lpjmedia-codec-i386-win32-vc6-release \
    -lpjmedia-i386-win32-vc6-release \
    -lpjnath-i386-win32-vc6-release \
    -lpjsip-core-i386-win32-vc6-release \
    -lpjsip-simple-i386-win32-vc6-release \
    -lpjsip-ua-i386-win32-vc6-release \
    -lpjsua-lib-i386-win32-vc6-release

    LIBS += -lQAxContainer
}
