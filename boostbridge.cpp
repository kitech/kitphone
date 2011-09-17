// boostbridge.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-03 20:34:28 +0000
// Version: $Id$
// 

#include "simplelog.h"

#include "boostbridge.h"

/*
  对于从boost来的信息，无论来自什么线程，使用这个工具类发送到qt，则信息接收端的函数在qt端线程执行
  对于从Qt发送来的信号，是否能实现boost接收端的函数调用在boost线程中执行呢？
 */

BoostBridge::BoostBridge(QObject *parent)
    : QObject(parent)
{
}

BoostBridge::~BoostBridge()
{
}

void BoostBridge::on_sip_engine_error(int eno)
{
    // qLogx()<<eno;
    emit this->sip_engine_error(eno);
}

void BoostBridge::on_sip_engine_started()
{
    // qLogx()<<"";
    emit this->sip_engine_started();
}

void BoostBridge::onon_test_q2b_signal_run_thread()
{
    qLogx()<<"";
    this->my_test_q2b_rt();
}

