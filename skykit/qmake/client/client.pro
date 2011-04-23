# client.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-12-01 17:12:21 +0800
# Version: $Id: client.pro 780 2011-03-20 15:08:48Z drswinghead $
# 

TEMPLATE = app
QT -= core gui
TARGET = skypekitclient

SOURCES += 
HEADERS += 

include(../skypekit_path.pri)
win32 {
    CONFIG += console warn_on
    CONFIG -= embed_manifest_exe
    CONFIG -= embed_manifest_dll
    CONFIG -= incremental
} else {
     QMAKE_CXXFLAGS += -g
     QMAKE_CXXFLAGS += -fno-rtti   # must used by skypekit lib, -fno-exceptions
}

# LIBPATH += ../cppwrapper
QMAKE_LIBDIR += ../cppwrapper ../cyassl
LIBS +=  -lskypekit-cppwrapper_2_lib -lskypekit-cyassl_lib
win32 {
    LIBS += -lws2_32
} else {
    LIBS += -lpthread
    POST_TARGETDEPS += ../cppwrapper/libskypekit-cppwrapper_2_lib.a ../cyassl/libskypekit-cyassl_lib.a
}
QMAKE_CXXFLAGS +=  -DSSL_LIB_CYASSL -DNO_FILESYSTEM

SOURCES += $$SKCED/client/command-conversationconference.cpp \
        $$SKCED/client/cconversation.cpp   \
        $$SKCED/client/command-invoker.cpp   \
        $$SKCED/client/command-greeting.cpp   \
        $$SKCED/client/command-conversationchat.cpp   \
        $$SKCED/client/cmessage.cpp   \
        $$SKCED/client/cvideowindow.cpp   \
        $$SKCED/client/csms.cpp   \
        $$SKCED/client/main.cpp   \
        $$SKCED/client/command-privacy.cpp   \
        $$SKCED/client/command-sms.cpp   \
        $$SKCED/client/ccontact.cpp   \
        $$SKCED/client/cvideo.cpp   \
        $$SKCED/client/command-voicemail.cpp   \
        $$SKCED/client/cskype.cpp   \
        $$SKCED/client/command-filetransfer.cpp   \
        $$SKCED/client/x11window.cpp   \
        $$SKCED/client/cvoicemail.cpp   \
        $$SKCED/client/cparticipant.cpp   \
#        $$SKCED/client/posixshmtransport.cpp   \
        $$SKCED/client/command-conversation.cpp   \
        $$SKCED/client/glwindow.cpp   \
        $$SKCED/client/command-app2app.cpp   \
#        $$SKCED/client/sysvshmtransport.cpp   \
        $$SKCED/client/command-setup.cpp   \
#        $$SKCED/client/iosurfacetransport.cpp   \
        $$SKCED/client/command-account.cpp   \
        $$SKCED/client/ctransfer.cpp   \
        $$SKCED/client/helper-functions.cpp   \
        $$SKCED/client/command-conversationcall.cpp   \
        $$SKCED/client/ccontactsearch.cpp   \
        $$SKCED/client/caccount.cpp   \
        $$SKCED/client/command-contactlist.cpp   \
        $$SKCED/client/command-contactsearch.cpp   \
        $$SKCED/client/ccontactgroup.cpp

