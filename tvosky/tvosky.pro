# tests.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-11-16 22:21:27 +0800
# Version: $Id: tvosky.pro 780 2011-03-20 15:08:48Z drswinghead $
# 

QT     += core gui network
TARGET = tvosky
TEMPLATE = app
CONFIG += debug
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

VERSION = 0.0.3

#########################
INCLUDEPATH += . ../skynet .. ../tests/

#SOURCES += tutorial_1.cpp
RESOURCES = icons/tvosky.qrc
SOURCES += main.cpp tvosky.cpp \
        skypeutils.cpp utils.cpp \
        buddybox.cpp \
        buddyitem.cpp \

HEADERS += tvosky.h \
        utils.h \
        buddybox.h

FORMS += ui/tvosky.ui \
      ui/buddybox.ui

SKYKITCLIENT_HEADERS = \
                     caccount.h \
                     ccontact.h     \
                     ccontactgroup.h     \
                     cconversation.h     \
                     cparticipant.h     \
                     cskype.h 

SKYKITCLIENT_SOURCES =  \
                     caccount.cpp     \
                     ccontact.cpp     \
                     ccontactgroup.cpp     \
                     ccontactsearch.cpp     \
                     cconversation.cpp     \
                     cmessage.cpp     \
                     cparticipant.cpp     \
                     cskype.cpp     \
                     csms.cpp     \
                     ctransfer.cpp     \
                     cvideo.cpp     \
                     cvideowindow.cpp     \
                     cvoicemail.cpp     \
                     glwindow.cpp     \
                     helper-functions.cpp     \
                     iosurfacetransport.cpp     \
                     posixshmtransport.cpp     \
                     sysvshmtransport.cpp     \
                     x11window.cpp

HEADERS += $$SKYKITCLIENT_HEADERS
SOURCES += $$SKYKITCLIENT_SOURCES

# skypepath
include(../skykit/qmake/skypekit_path.pri)
# skypeclient
# include(./skypekitclient.pri)

#libskynet 
# include(../skynet/libskynet.pri)

HOME=$$system("echo $HOME")
INCLUDEPATH += $$HOME/xfsdev/include $$HOME/skypegw/include
LIBS += -L$$HOME/xfsdev/lib -L$$HOME/skypegw/lib
# INCLUDEPATH += /usr/include/postgresql/    # for ubuntu, the include header not in standard /usr/include        
# INCLUDEPATH += $$HOME/local/postgresql-9.x/include
# LIBS += -L$$HOME/local/postgresql-9.x/lib -lpq                                                      
PJLIB_PC_PATH=$$HOME/xfsdev/lib/pkgconfig:$$HOME/skypegw/lib/pkgconfig:/serv/stow/pjsip/lib/pkgconfig
# message($$PJLIB_PC_PATH)                                                                                      
PJSIP_LIBS=$$system("PKG_CONFIG_PATH=$$PJLIB_PC_PATH pkg-config --libs libpjproject")                           
PJSIP_CFLAGS=$$system("PKG_CONFIG_PATH=$$PJLIB_PC_PATH pkg-config --cflags libpjproject")                       
# message($$PJSIP_LIBS +++++++ $$PJSIP_CFLAGS)                                                                  
LIBS += $$PJSIP_LIBS                                                                                            
QMAKE_CXXFLAGS += $$PJSIP_CFLAGS
win32 {
    CONFIG -= debug
    CONFIG -= embed_manifest_exe
    CONFIG -= embed_manifest_dll
    CONFIG -= incremental
    QMAKE_LFALGS += /NODEFAULTLIB:msvcrt.lib
} else {
    QMAKE_CXXFLAGS += -fno-rtti   # must used by skypekit lib, -fno-exceptions
    QMAKE_CXXFLAGS += -g
}

#######skypekit
SKYPEKIT_PATH=$$SKBD
INCLUDEPATH += $$SKYPEKIT_PATH/ipc/cpp $$SKYPEKIT_PATH/ipc/cpp/platform/se \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/api \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/ipc \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/types \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/platform/threading \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/platform/threading/pthread
LIBS += -L../skykit/qmake/cppwrapper -L../skykit/qmake/cyassl \
     -L$$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/build \
     -lskypekit-cppwrapper_2_lib \
     -lskypekit-cyassl_lib

win32 {
    LIBS += -lws2_32
} else {
    LIBS += -lpthread
}
