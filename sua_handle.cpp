// sua_handle.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-30 09:54:43 +0800
// Version: $Id: sua_handle.cpp 601 2010-12-03 02:45:47Z drswinghead $
// 

#include "sua_handle.h"

////////////////////////////////
PjsuaHandleThread::PjsuaHandleThread(QObject *parent)
    : QThread(parent)
{
    this->quit_loop = false;
}

PjsuaHandleThread::~PjsuaHandleThread()
{
}

void PjsuaHandleThread::run()
{
    pj_thread_desc initdec;
    pj_thread_t* thread = 0;
    pj_status_t status;
    int evt_cnt = 0;

    qDebug()<<"ready register pjsip thread by Qt";
    if (!pj_thread_is_registered()) {
        status = pj_thread_register("PjsipPollThread_run", initdec, &thread);
        if (status != PJ_SUCCESS) {
            qDebug()<<"pj_thread_register faild:"<<status;
            return;
        }
    }
    PJ_CHECK_STACK();
    qDebug()<<"registerred pjsip thread:"<<thread;

    this->dump_info(thread);
    // while(isStackInit()) {
    //     pjsua_handle_events(10);
    // }
    qDebug()<<"enter pjsua thread loop supported by Qt";
    while (!quit_loop) {
        evt_cnt = pjsua_handle_events(20);
        if (qrand() % 150 == 1 && evt_cnt > 0) {
            qDebug()<<"pjsua pool event in thread supported by Qt, proccess count:"<<evt_cnt;
        }
    }
}

void PjsuaHandleThread::dump_info(pj_thread_t *thread)
{
    Q_ASSERT(thread != NULL);

    qDebug()<<"pj_thread_is_registered:"<<pj_thread_is_registered();
    qDebug()<<"pj_thread_get_prio:"<<pj_thread_get_prio(thread);
    qDebug()<<"pj_thread_get_prio_min:"<<pj_thread_get_prio_min(thread);
    qDebug()<<"pj_thread_get_prio_max:"<<pj_thread_get_prio_max(thread);
    qDebug()<<"pj_thread_get_name:"<<pj_thread_get_name(thread);
    qDebug()<<"pj_getpid:"<<pj_getpid();
}

