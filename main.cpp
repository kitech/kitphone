// main.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-20 17:23:34 +0800
// Version: $Id$
// 

#include <QtGui/QApplication>
#include "kitphone.h"

#include "kitapplication.h"

int main(int argc, char *argv[])
{
    SkyServApplication a(argc, argv);
    KitPhone w;
    w.show();

    return a.exec();
}
