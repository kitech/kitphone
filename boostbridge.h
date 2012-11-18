// boostbridge.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-03 20:34:18 +0000
// Version: $Id$
// 

#ifndef _BOOSTBRIDGE_H_
#define _BOOSTBRIDGE_H_

#include <QtCore>
#include <boost/signals2.hpp>

class BoostBridge : public QObject
{
    Q_OBJECT;
public:
    explicit BoostBridge(QObject *parent = 0);
    virtual ~BoostBridge();

    // boost slot
    void on_sip_engine_error(int eno);
    void on_sip_engine_started();

    // boost signal
    boost::signals2::signal<void(void)> my_test_q2b_rt;

public slots:
    void onon_test_q2b_signal_run_thread();

signals:
    void sip_engine_error(int eno);
    void sip_engine_started();
};

#endif /* _BOOSTBRIDGE_H_ */
