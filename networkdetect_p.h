// networkdetect_p.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-02 18:05:02 +0800
// Version: $Id$
// 


#ifndef _NETWORKDETECT_P_H_
#define _NETWORKDETECT_P_H_

#include <QtCore>
#include <QtNetwork>

#include "boost/function.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/tuple/tuple.hpp"

class NetworkCheckData
{
public:
    NetworkCheckData() {
        m_check_ok = false;
        m_retry_times = 0;
        m_check_obj = NULL;
        m_check_slot_prototype = NULL;
        m_check_slot_name = NULL;
    }
    bool m_check_ok;
    int m_retry_times;
    QDateTime m_start_time;
    QDateTime m_end_time;
    QString m_conn_serv;
    unsigned short m_conn_port;
    boost::shared_ptr<QTcpSocket> m_conn_tsock;
    boost::shared_ptr<QUdpSocket> m_conn_usock;
    boost::function<bool(int)> m_check_fun0;
    boost::function<bool(int, NetworkCheckData*)> m_check_fun1;
    QObject *m_check_obj;
    const char *m_check_slot_prototype;
    char *m_check_slot_name;
};


#endif /* _NETWORKDETECT_P_H_ */
