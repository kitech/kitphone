# tests.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-11-16 22:21:27 +0800
# Version: $Id: tvosky.pro 633 2010-12-14 03:28:39Z drswinghead $
# 

QT     += core network
QT     -=  gui
TARGET = 
TEMPLATE = app
CONFIG += debug
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

VERSION = 0.0.8

#########################
INCLUDEPATH += . ../skynet .. ../tests/
DEFINES += HAVE_CONFIG_H


#SOURCES += tutorial_1.cpp
RESOURCES = 
SOURCES += main.cpp \
        kitserv.cpp \
        websocket_proc.cpp \
        sip_entry.cpp \
        sxs_switcher.cpp \
        ringbuffer.c pa_ringbuffer.c \
#        PCMLoopback.cpp \
#        PCMWavPlayer.cpp \
        PCMSxsHost.cpp \
        KitPCMHostMain.cpp \
        ./ewebsocket.c ./emd5.c \
        ../skyserv/wryip.c ../libng/bencode.c ../libng/qtmd5.cpp \
        ../skyserv/database.cpp ../skyserv/configs.cpp

HEADERS += ../skyserv/database.h ../skyserv/configs.h

FORMS += 

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

#libskynet 
# include(../skynet/libskynet.pri)

# skypepath
include(../skykit/qmake/skypekit_path.pri)
# skypeclient
# include(./skypekitclient.pri)

VOICEPCM_SOURCES = $$SKBD/interfaces/voicepcm/sidg_pcmif_server.cpp \
                      $$SKBD/interfaces/voicepcm/sidg_pcmif_cb_client.cpp  \
                      $$SKBD/interfaces/voicepcm/sidg_msg_pcmif.cpp     \
                      $$SKBD/interfaces/voicepcm/sidg_msg_pcmif_cb.cpp    \
                      $$SKBD/ipc/cpp/SidApi.cpp  \
                      $$SKBD/ipc/cpp/SidAVDummyHelpers.cpp \
                      $$SKBD/ipc/cpp/SidAVTransportWrapper.cpp \
                      $$SKBD/ipc/cpp/SidAVServer.cpp  \
                      $$SKBD/ipc/cpp/SidField.cpp \
                      $$SKBD/ipc/cpp/SidProtocolBinClient.cpp \
                      $$SKBD/ipc/cpp/SidProtocolBinCommon.cpp \
                      $$SKBD/ipc/cpp/SidProtocolBinServer.cpp \
                      $$SKBD/ipc/cpp/SidProtocolFactory.cpp \
                      $$SKBD/ipc/cpp/AVTransport/SocketTransport.cpp \
                      $$SKBD/ipc/cpp/AVTransport/UnixSocket.cpp
INCLUDEPATH += $$SKBD/ipc/cpp \
            $$SKBD/ipc/cpp/AVTransport \
            $$SKBD/interfaces/voicepcm

SOURCES += $$VOICEPCM_SOURCES

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
DEFINES += NO_FILESYSTEM
QMAKE_CXXFLAGS +=  -DSSL_LIB_CYASSL
QMAKE_CFLAGS = $$QMAKE_CXXFLAGS
QMAKE_CFLAGS -= -fno-rtti

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
    LIBS += -lpthread -lresolv
}

###### for pjsip
HOME=$$system("echo $HOME")
INCLUDEPATH += $$HOME/xfsdev/include
LIBS += -L$$HOME/xfsdev/lib                                                                                     
INCLUDEPATH += /usr/include/postgresql/    # for ubuntu, the include header not in standard /usr/include        
INCLUDEPATH += $$HOME/local/postgresql-9.x/include
LIBS += -L$$HOME/local/postgresql-9.x/lib -lpq
PJLIB_PC_PATH=$$HOME/xfsdev/lib/pkgconfig:/serv/stow/pjsip/lib/pkgconfig                                        
# message($$PJLIB_PC_PATH)                                                                                      
PJSIP_LIBS=$$system("PKG_CONFIG_PATH=$$PJLIB_PC_PATH pkg-config --libs libpjproject")                           
PJSIP_CFLAGS=$$system("PKG_CONFIG_PATH=$$PJLIB_PC_PATH pkg-config --cflags libpjproject")                       
# message($$PJSIP_LIBS +++++++ $$PJSIP_CFLAGS)                                                                  
LIBS += $$PJSIP_LIBS                                                                                            
QMAKE_CXXFLAGS += $$PJSIP_CFLAGS

