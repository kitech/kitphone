# skypekit.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-12-01 17:12:34 +0800
# Version: $Id: skypekit.pro 780 2011-03-20 15:08:48Z drswinghead $
# 

# simple check qt version
QMAKEVERSION = $$[QMAKE_VERSION]
ISQT4 = $$find(QMAKEVERSION, ^[2-9])

isEmpty( ISQT4 ) {
    error("Use the qmake include with Qt4.4 or greater, on Debian that is qmake-qt4");
}

TEMPLATE = subdirs
CONFIG = ordered
DESTDIR = .

SUBDIRS += cppwrapper cyassl client 