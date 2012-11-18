// skyservapplication.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-04-17 23:14:21 +0800
// Version: $Id: kitapplication.cpp 842 2011-04-21 06:24:06Z drswinghead $
// 

#include <QtCore>

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#endif

#include "kitapplication.h"

#if defined(Q_OS_WIN)
// for skype
bool eventHandled = false;
long eventResult = -1;
#endif

SkyServApplication::SkyServApplication(int & argc, char ** argv)
	: QtSingleApplication(argc, argv)
{

}

SkyServApplication::~SkyServApplication()
{

}

#if defined(Q_OS_WIN)
// bool SkyServApplication::eventHandled=false;
// long SkyServApplication::eventResult=0;
bool SkyServApplication::winEventFilter ( MSG * msg, long * result )
{
	//qDebug()<<__FUNCTION__<<__LINE__<<rand();
	//qDebug()<<msg->message ;

    ::eventHandled=false;
    emit winMessage( msg );
    if ( ::eventHandled )
      *result = ::eventResult;
    return ::eventHandled;

	// return QApplication::winEventFilter(msg,result);
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
