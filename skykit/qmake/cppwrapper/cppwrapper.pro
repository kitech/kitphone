# cppwrapper.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-12-01 17:12:26 +0800
# Version: $Id: cppwrapper.pro 780 2011-03-20 15:08:48Z drswinghead $
# 

TEMPLATE = lib
QT -= core gui
CONFIG += staticlib console
TARGET = skypekit-cppwrapper_2_lib

SOURCES += 
HEADERS += 

include(../skypekit_path.pri)
win32 {
    SOURCES += $$SKCED/platform/threading/win/skype-thread-win.cpp
} else {
    SOURCES += $$SKCED/platform/threading/pthread/skype-thread-pthread.cpp
    QMAKE_CXXFLAGS += -g
    QMAKE_CXXFLAGS += -fno-rtti   # must used by skypekit lib, -fno-exceptions
}
QMAKE_CXXFLAGS += -MMD -MP  -DSSL_LIB_CYASSL -DNO_FILESYSTEM
QMAKE_CFLAGS = $$QMAKE_CXXFLAGS

SOURCES += $$SKCED/types/skype-int-list.cpp \
        $$SKCED/types/skype-uint64-list.cpp \
        $$SKCED/types/skype-ref-list.cpp \
        $$SKCED/types/skype-ptrint-dict.cpp \
        $$SKCED/types/skype-filename.cpp \
        $$SKCED/types/skype-binary.cpp \
        $$SKCED/types/skype-filename-list.cpp \
        $$SKCED/types/skype-ptr-dict.cpp \
        $$SKCED/types/skype-obj-dict.cpp \
        $$SKCED/types/skype-string-list.cpp \
        $$SKCED/types/skype-string.cpp \
        $$SKCED/types/skype-ptr-vector.cpp \
        $$SKCED/types/skype-string-dict.cpp \
        $$SKCED/types/skype-int-dict.cpp

SOURCES += $$SKCED/ipc/skype-clientsession.cpp \  
        $$SKCED/ipc/skype-object.cpp

SOURCES += $$SKCED/api/sidg_skylibevent_dispatch.cpp \
        $$SKCED/api/sidg_msgs_skylib.cpp  \
        $$SKCED/api/sidg_skylibaction_call.cpp    \
        $$SKCED/api/skype-embedded_2.cpp    \
        $$SKCED/api/sidg_skylibproperty_get_call.cpp      \
        $$SKCED/api/sidg_skylibrefs.cpp

SOURCES += $$SKBD/ipc/cpp/platform/se/SidPlatform.cpp \
        $$SKBD/ipc/cpp/SidSession.cpp \
        $$SKBD/ipc/cpp/SidClientSession.cpp \
        $$SKBD/ipc/cpp/SidCommandProcessor.cpp \
        $$SKBD/ipc/cpp/SidField.cpp \
        $$SKBD/ipc/cpp/SidProtocolBinCommon.cpp \
        $$SKBD/ipc/cpp/SidProtocolBinClient.cpp \
        $$SKBD/ipc/cpp/SidConnection.cpp \
        $$SKBD/ipc/cpp/SidTransportLog.cpp \
#        $$SKBD/ipc/cpp/SidSocketConnection.cpp \
        $$SKBD/ipc/cpp/SidClientConnection.cpp \
        $$SKBD/ipc/cpp/SkypekitVideoTransportBase.cpp \
        $$SKBD/ipc/cpp/SkypekitVideoTransportClient.cpp \
        $$SKBD/ipc/cpp/SidTLSEncryption.cpp

        