// main.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-03 15:24:47 +0800
// Version: $Id: main.cpp 576 2010-11-18 03:40:16Z drswinghead $
// 


#include <QtCore>

#include "vomcall.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    VomCall c(argc, argv);
    c.login();
    
    return a.exec();
}
