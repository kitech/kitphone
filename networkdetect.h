// networkdetect.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-27 21:27:28 +0800
// Version: $Id$
// 

#ifndef _NETWORKDETECT_H_
#define _NETWORKDETECT_H_

/*
  检测目标：
  到服务器端口直连情况, UDP, TCP, flash, 常用的5060, 80-88,8001-8009,8080-8081,443,

  traceroute 路由情况，跳数

  内外网IP情况
  
  NAT类型

  upnp支持情况

  IPV6情况

  网速情况

  操作系统情况

  内网机器情况
 */

#include <QtCore>
#include <QtNetwork>

#include "boost/smart_ptr.hpp"

#define GATEWAY_WEB_HOSTNAME "gw.skype.tom.com"

class NetworkChecker : public QThread
{
    Q_OBJECT;
public:
    virtual ~NetworkChecker();
    static boost::shared_ptr<NetworkChecker> instance();
    virtual void run();

    bool isInternalNetwork();
    bool isSupportIPV6();
    bool isSupportPing();

    int networkType(); // wt,dx,tt,3g,2g...

private slots:
    void onDoSomething();
    void onResolveWebServerDone(QHostInfo hi);
    void onConnectedWebServer();
    void onConnectWebServerError(QAbstractSocket::SocketError socketError);

public:
    enum NetworkLineType {
        NLT_MIN = 0,
        NLT_WANGTONG,
        NLT_DIANXING,
        NLT_TITONG,
        NLT_3G,
        NLT_MAX
    };
protected:
    explicit NetworkChecker(QObject *parent = 0);

signals:
    void whatToDoNext();

private:
    static boost::shared_ptr<NetworkChecker> mInst;

    int m_network_line_type;
    QString m_gateway_web_serv_ipaddr;
    bool m_connected_web_serv;
    QDateTime m_connect_web_serv_start_time;
    QDateTime m_connect_web_serv_end_time;
    boost::shared_ptr<QTcpSocket> m_connect_web_serv_sock;

    QString m_gateway_router_serv_ipaddr;
    bool m_connected_ws_serv;
    QDateTime m_connect_ws_serv_start_time;
    QDateTime m_connect_ws_serv_end_time;
    boost::shared_ptr<QTcpSocket> m_connect_ws_serv_sock;

private:

};

#endif /* _NETWORKDETECT_H_ */
