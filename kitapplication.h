// nullgetapplication.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-04-09 00:16:45 +0000
// Version: $Id: kitapplication.h 842 2011-04-21 06:24:06Z drswinghead $
// 

#ifndef SKYSERVAPPLICATION_H
#define SKYSERVAPPLICATION_H

#include <QApplication>
#include <QtSingleApplication>  // add include_once in this file

class SkyServApplication : public QtSingleApplication
{
	Q_OBJECT;
public:
    SkyServApplication(int & argc, char ** argv);
    virtual ~SkyServApplication();

#if defined(Q_OS_WIN)
    // for skype
    // static bool eventHandled;
    // static long eventResult;

    virtual bool winEventFilter ( MSG * msg, long * result );

signals:
    void winMessage( MSG *msg );

#elif defined(Q_OS_MAC)
    virtual bool macEventFilter(EventHandlerCallRef caller, EventRef event ) ;
#else
	virtual bool x11EventFilter ( XEvent * event ) ;
#endif

public slots:
    void handleMessage(const QString &msg);

private:
    
};

#endif // SKYSERVAPPLICATION_H


