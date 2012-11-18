# tests.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-11-16 22:21:27 +0800
# Version: $Id: tests.pro 576 2010-11-18 03:40:16Z drswinghead $
# 

QT       += core network
QT       -= gui
TARGET = vomcall
TEMPLATE = app
CONFIG += debug
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

VERSION = 0.0.1

#########################
INCLUDEPATH += . ../skynet ..

#SOURCES += tutorial_1.cpp
SOURCES += main.cpp vomcall.cpp skypekit.cpp skaccount.cpp \
        skconversation.cpp skparticipant.cpp
HEADERS += vomcall.h skaccount.h

#libskynet 
# include(../skynet/libskynet.pri)

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
QMAKE_CXXFLAGS += -fno-rtti   # must used by skypekit lib, -fno-exceptions

#######skypekit
SKYPEKIT_PATH=$$HOME/skypekit/distro-skypekit-cb3.1_3.1.0.2665_124773
INCLUDEPATH += $$SKYPEKIT_PATH/ipc/cpp $$SKYPEKIT_PATH/ipc/cpp/platform/se \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/api \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/ipc \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/types \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/platform/threading \
            $$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/src/platform/threading/pthread
LIBS += -L$$SKYPEKIT_PATH/interfaces/skype/cpp_embedded/build -lskypekit-cppwrapper_2_lib -lpthread