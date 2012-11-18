// main.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-03 15:24:47 +0800
// Version: $Id: main.cpp 596 2010-12-01 02:42:27Z drswinghead $
// 


#include <QtCore>

#include "tvosky.h"
// #include "vomcall.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TvoSky vp;
    vp.show();
    // VomCall c(argc, argv);
    // c.login();

    return a.exec();
}
