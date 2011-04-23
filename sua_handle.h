// sua_handle.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-30 09:54:28 +0800
// Version: $Id: sua_handle.h 592 2010-11-30 02:12:58Z drswinghead $
// 

#ifndef _SUA_HANDLE_H_
#define _SUA_HANDLE_H_

#include <QtCore>

extern "C" {
	#include <pjlib.h>
	#include <pjlib-util.h>
	#include <pjmedia.h>
	#include <pjmedia-codec.h>
	#include <pjsip.h>
	#include <pjsip_simple.h>
	#include <pjsip_ua.h>
	#include <pjsua-lib/pjsua.h>
}

class PjsuaHandleThread : public QThread
{
    Q_OBJECT;
public:
    explicit PjsuaHandleThread(QObject *parent = 0);
    virtual ~PjsuaHandleThread();

    void run();

    void dump_info(pj_thread_t *thread);
private:
    bool quit_loop;
};

#endif /* _SUA_HANDLE_H_ */
