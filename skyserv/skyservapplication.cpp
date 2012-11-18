// skyservapplication.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-04-17 23:14:21 +0800
// Version: $Id: skyservapplication.cpp 614 2010-12-08 08:33:24Z drswinghead $
// 

#include <QtCore>

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#endif

#include "skyservapplication.h"

SkyServApplication::SkyServApplication(int & argc, char ** argv)
	: QApplication(argc, argv)
{

}

SkyServApplication::~SkyServApplication()
{

}

#if defined(Q_OS_WIN)
bool SkyServApplication::winEventFilter ( MSG * msg, long * result )
{
	//qDebug()<<__FUNCTION__<<__LINE__<<rand();

	//qDebug()<<msg->message ;

	return QApplication::winEventFilter(msg,result);

}
#elif defined(Q_OS_MAC)
bool SkyServApplication::macEventFilter(EventHandlerCallRef caller, EventRef event )
{
    return QApplication::macEventFilter(caller, event);
}
#else
#include <X11/Xlib.h>
#include "xmessages.h"
bool SkyServApplication::x11EventFilter(XEvent *event)
{
    switch(event->type) {
    case ClientMessage:
        XMessages::processXMessages(event);
        break;
    }
    // return FALSE;
	return QApplication::x11EventFilter(event);
}
#endif

void SkyServApplication::handleMessage(const QString &msg)
{
    // qDebug()<<"I am running, you say:"<<msg;
}
