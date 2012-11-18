# krtman.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2011-01-06 21:09:35 +0800
# Version: $Id: krtman.pro 672 2011-01-07 01:20:27Z drswinghead $
# 

QT     += core
QT     -=  gui
TARGET = krtman
TEMPLATE = app
CONFIG += debug
UI_HEADERS_DIR = GeneratedFiles
MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp

VERSION = 0.0.5

#########################
INCLUDEPATH += ../skyserv/

RESOURCES = 
SOURCES += krtman.cpp ../skyserv/configs.cpp
HEADERS += krtman.h ../skyserv/configs.h
FORMS += 
