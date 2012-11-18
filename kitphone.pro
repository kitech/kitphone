# kitphone.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-09-21 16:19:00 +0800
# Version: $Id: kitphone.pro 998 2011-09-17 11:03:58Z drswinghead $
# 

QT      += core gui network declarative sql
TARGET = kitphone
TEMPLATE = app
CONFIG += debug_and_release console qaxcontainer link_pkgconfig
#QTPLUGIN += qsqlite

UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
debug {
OBJECTS_DIR = tmpd
} else {
OBJECTS_DIR = tmp
}

VERSION = 0.6.6

win32 {
    CONFIG -= windows
    #QMAKE_LFLAGS_CONSOLE = -Wl,-subsyetem,console
    QMAKE_LFLAGS += $$QMAKE_LFLAGS_CONSOLE
}else{
    QMAKE_CXXFLAGS += -g -std=c++0x
}
DEFINES += KP_VERSION_STR=\"\\\"$$VERSION\\\"\" 

QTSAPP_HEADERS = ./qtsingleapplication/qtsingleapplication.h ./qtsingleapplication/qtlocalpeer.h
QTSAPP_SOURCES = ./qtsingleapplication/qtsingleapplication.cpp ./qtsingleapplication/qtlocalpeer.cpp

SOURCES += main.cpp kitapplication.cpp metauri.cpp \
        kitphone.cpp aboutdialog.cpp \
        sipaccountswindow.cpp sipaccountpropertieswindow.cpp \
        preferencesdialog.cpp preferences.cpp global.cpp \
        sipaccount.cpp \
        skypetracer.cpp skypetunnel.cpp volctl.cpp \
#        PjCallback.cpp \
        asyncdatabase.cpp \
        databaseworker.cpp \
        simplelog.cpp ldist.cpp

HEADERS  += kitphone.h kitapplication.h metauri.h aboutdialog.h \
         sipaccountswindow.h sipaccountpropertieswindow.h \
        preferencesdialog.h\
         sipaccount.h \
         skypetracer.h skypetunnel.h \
#         PjCallback.h \
        asyncdatabase.h \
        databaseworker.h \
        simplelog.h

FORMS   += kitphone.ui aboutdialog.ui skypetracer.ui \
        sipaccountswindow.ui sipaccountpropertieswindow.ui \
        preferencesdialog.ui 

#######
SOURCES +=  mosipphone.cpp #sipphone.cpp
HEADERS += mosipphone.h #sipphone.h
FORMS += sipphone.ui mosipphone.ui

#######
SOURCES += skypephone.cpp websocketclient.cpp websocketclient2.cpp phonecontact.cpp phonecontactproperty.cpp \
        groupinfodialog.cpp contactmodel.cpp callhistorymodel.cpp \
        networkdetect.cpp ystatusbar.cpp pjtypes.cpp sipengine.cpp boostbridge.cpp \
        websocketserver3.cpp libwebsockets_extra.cpp security.cpp intermessage.cpp \
        skycitengine.cpp
HEADERS += skypephone.h websocketclient.h websocketclient2.h phonecontact.h phonecontactproperty.h \
        groupinfodialog.h contactmodel.h callhistorymodel.h boostbridge.h \ 
        networkdetect.h networkdetect_p.h ystatusbar.h pjtypes.h \
        skycitengine.h
FORMS += skypephone.ui phonecontactproperty.ui groupinfodialog.ui

SOURCES += $$QTSAPP_SOURCES
HEADERS += $$QTSAPP_HEADERS

#############

SOURCES += $$ASIP_SOURCES
################

RESOURCES += kitphone.qrc
win32 {
    RC_FILE = kitphone.rc
}

# libwss
DEFINES += DEBUG
#QMAKE_CFLAGS += -include ./libwss/wss_config.h
INCLUDEPATH += ./libwss
win32 {
    INCLUDEPATH += ./libwss/win32helpers Z:/librarys/vc-zlib/include Z:/cross/boost_1_47_0
    INCLUDEPATH += Z:/librarys/vc-ssl-x86/include
    LIBS += -LZ:/cross/boost_1_47_0/stage/lib -LZ:/librarys/vc-ssl-x86/lib
    LIBS += -llibboost_thread-vc100-mt-s-1_47 -llibboost_serialization-vc100-mt-s-1_47
    LIBS += -lws2_32 # for libwss
} else {
    LIBS += -lboost_thread -lboost_serialization
}


SOURCES += ./libwss/base64-decode.c  \
            ./libwss/extension.c     \
            ./libwss/handshake.c     \
#            ./libwss/md5.c     \
#            ./libwss/sha-1.c         \
            ./libwss/client-handshake.c  \
            ./libwss/extension-deflate-stream.c  \
            ./libwss/extension-x-google-mux.c \
            ./libwss/libwebsockets.c  \
            ./libwss/parsers.c 
win32 {
    SOURCES +=  ./libwss/win32helpers/gettimeofday.c \
           ./libwss/win32helpers/poll.c
}
#DEFINES += LWS_EXT_GOOGLE_MUX
DEFINES += KP_LWS_CLIENT
# \"\\\"$$VERSION\\\"\" 
DEFINES += LWS_OPENSSL_SUPPORT LWS_OPENSSL_CLIENT_CERTS=\"\\\"/etc/pki/tls/certs/\\\"\" 

## libskynet
include(./skynet/libskynet.pri)

### jsoncpp
include(./jsoncpp/jsoncpp.pri)

### libjson vc10 compile error.
 # include(./libjson/libjson.pri)

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
    -llibg729codec-i386-win32-vc6-release \
    -llibsilkcodec-i386-win32-vc6-release \
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
