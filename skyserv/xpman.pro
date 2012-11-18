# krtman.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2011-01-06 21:09:35 +0800
# Version: $Id: krtman.pro 672 2011-01-07 01:20:27Z drswinghead $
# 

QT     += core network
QT     -=  gui
TARGET = xpman
TEMPLATE = app
CONFIG += debug
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

VERSION = 0.0.8

#########################
QMAKE_CFLAGS += -std=c1x
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -g
QMAKE_CFLAGS += -g

INCLUDEPATH += .. ./../../boost/include/
QMAKE_LIBDIR +=./../../boost/lib/
LIBS +=-lboost_system

RESOURCES = 
SOURCES += xpman.cpp xpapp.cpp ../skyserv/configs.cpp database.cpp
HEADERS += xpman.h xpapp.h ../skyserv/configs.h database.h
FORMS += 

SOURCES += nbtimer.cpp stucxxox.cpp
HEADERS += nbtimer.h stucxxox.h

include(../qtsingleapplication/qtsinglecoreapplication.pri)

HOME=$$system("echo $HOME")
INCLUDEPATH += $$HOME/xfsdev/include $$HOME/skypegw/include
LIBS += -L$$HOME/xfsdev/lib -L$$HOME/skypegw/lib
INCLUDEPATH += /usr/include/postgresql/    # for ubuntu, the include header not in standard /usr/include
INCLUDEPATH += $$HOME/local/postgresql-9.x/include
LIBS += -L$$HOME/local/postgresql-9.x/lib -lpq


