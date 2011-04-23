# skyserv.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-07-03 14:03:38 +0800
# Version: $Id: skyserv.pro 847 2011-04-23 04:46:25Z drswinghead $
# 

QT       += core gui network
TARGET = skyserv
TEMPLATE = app
CONFIG += debug link_pkgconfig
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

# only test qmake pkgconfig support feature
PKGCONFIG += x11

VERSION = 0.7.80

#########################
#QMAKE_CFLAGS += -std=c1x  # newlisp compile faild
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -g
QMAKE_CFLAGS += -g

INCLUDEPATH += . .. ../skynet ../../boost/include

SOURCES += main.cpp skyserv.cpp skyservapplication.cpp \
        configs.cpp database.cpp \
        ../metauri.cpp \
        siproom.cpp \
        sxs_switcher.cpp \
        pa_ringbuffer.c ringbuffer.c \
        wryip.c websocket.cpp \
        md5.c \
        ../utils.cpp \
        limit_detect.cpp \
        lisp_bridge.cpp

HEADERS  += skyserv.h skyservapplication.h \
         configs.h database.h \
         sxs_switcher.h \
         ../metauri.h \
         websocket.h  \
         ../utils.h \
        limit_detect.h \
         lisp_bridge.h

#libskynet 
include(../skynet/libskynet.pri)

#boost exceed
SOURCES += nbtimer.cpp

# libwss
DEFINES += DEBUG
QMAKE_CFLAGS += -include ../libwss/wss_config.h
INCLUDEPATH += ../libwss
SOURCES += ../libwss/base64-decode.c  \
             ../libwss/extension.c     \
            ../libwss/handshake.c     \
        #    ../libwss/md5.c     \
         #   ../libwss/sha-1.c         \
            ../libwss/client-handshake.c  \
            ../libwss/extension-deflate-stream.c  \
            ../libwss/libwebsockets.c  \
            ../libwss/parsers.c


#newlisp
# newlisp.o nl-symbol.o nl-math.o nl-list.o nl-liststr.o nl-string.o nl-filesys.o \
#	nl-sock.o nl-import.o nl-xml.o nl-web.o nl-matrix.o nl-debug.o pcre.o unix-lib.o
# CFLAGS = -fPIC -m64 -Wall -pedantic -Wno-uninitialized -Wno-strict-aliasing -Wno-long-long -c -O2 -g -DLINUX -DNEWLISP64 -DLIBRARY
SOURCES += ../newlisp/newlisp.c \
        ../newlisp/nl-symbol.c \
        ../newlisp/nl-math.c \
        ../newlisp/nl-list.c \
        ../newlisp/nl-liststr.c \
        ../newlisp/nl-string.c \
        ../newlisp/nl-filesys.c \
        ../newlisp/nl-sock.c \
        ../newlisp/nl-import.c \
        ../newlisp/nl-xml.c \
        ../newlisp/nl-web.c \
        ../newlisp/nl-matrix.c \
        ../newlisp/nl-debug.c \
        ../newlisp/pcre.c \
        ../newlisp/unix-lib.c
#INCLUDEPATH += ../newlisp
DEFINES += LINUX LIBRARY
UNAMEA=$$system("uname -m|grep x86_64|grep -v grep")
message($$UNAMEA)
eval($$UNAMEA = x86_64) {
    DEFINES += NEWLISP64
    message(644bit $$UNAMEA)
} else {
    DEFINES += NEWLISP32
    message(32bit $$UNAMEA)
}

HOME=$$system("echo $HOME")
INCLUDEPATH += $$HOME/xfsdev/include $$HOME/skypegw/include
LIBS += -L$$HOME/xfsdev/lib -L$$HOME/skypegw/lib
INCLUDEPATH += /usr/include/postgresql/    # for ubuntu, the include header not in standard /usr/include
INCLUDEPATH += $$HOME/local/postgresql-9.x/include
LIBS += -L$$HOME/local/postgresql-9.x/lib -lpq

PJLIB_PC_PATH=$$HOME/xfsdev/lib/pkgconfig:$$HOME/skypegw/lib/pkgconfig:/serv/stow/pjsip/lib/pkgconfig
# message($$PJLIB_PC_PATH)
PJSIP_LIBS=$$system("PKG_CONFIG_PATH=$$PJLIB_PC_PATH pkg-config --libs libpjproject")
PJSIP_CFLAGS=$$system("PKG_CONFIG_PATH=$$PJLIB_PC_PATH pkg-config --cflags libpjproject")
# message($$PJSIP_LIBS +++++++ $$PJSIP_CFLAGS)
LIBS += $$PJSIP_LIBS
QMAKE_CXXFLAGS += $$PJSIP_CFLAGS

LIBS += -lresolv   # for websocket's b64_ntop, b64_pton
