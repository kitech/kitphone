// networkdetect.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-27 21:27:40 +0800
// Version: $Id$
// 

#include "log.h"

#include "networkdetect.h"

boost::shared_ptr<NetworkChecker> NetworkChecker::mInst = boost::shared_ptr<NetworkChecker>();
NetworkChecker::NetworkChecker(QObject *parent)
    : QThread(parent)
{
    this->m_network_line_type = NLT_MIN;
    this->m_connected_web_serv = false;
    QObject::connect(this, SIGNAL(whatToDoNext()),
                     this, SLOT(onDoSomething()));

}

NetworkChecker::~NetworkChecker()
{
}

boost::shared_ptr<NetworkChecker> NetworkChecker::instance()
{
    if (NetworkChecker::mInst == boost::shared_ptr<NetworkChecker>()) {
        NetworkChecker::mInst = boost::shared_ptr<NetworkChecker>(new NetworkChecker());
    }

    return NetworkChecker::mInst;
}

void NetworkChecker::run()
{
    emit this->whatToDoNext();
    this->exec();
}

bool NetworkChecker::isInternalNetwork()
{
    return true;
}

bool NetworkChecker::isSupportIPV6()
{

    return false;
}

bool NetworkChecker::isSupportPing()
{
    
    return true;
}

int NetworkChecker::networkType() // wt,dx,tt,3g,2g..
{
    if (this->m_gateway_web_serv_ipaddr.isEmpty()) {
        return NLT_MIN;
    }

    return this->m_network_line_type;

    return NLT_MIN;
}

void NetworkChecker::onDoSomething()
{
    // 一群算法，智能产生更多的想法？？？？？
    qLogx()<<"hehe";

    if (this->m_gateway_web_serv_ipaddr.isEmpty()) {
        QHostInfo::lookupHost(GATEWAY_WEB_HOSTNAME, this, SLOT(onResolveWebServerDone(QHostInfo)));
    } else if (!this->m_connected_web_serv) {
        this->m_connect_web_serv_start_time = QDateTime::currentDateTime();
        this->m_connect_web_serv_sock = boost::shared_ptr<QTcpSocket>(new QTcpSocket());
        QObject::connect(this->m_connect_web_serv_sock.get(), SIGNAL(connected()),
                         this, SLOT(onConnectedWebServer()));
        QObject::connect(this->m_connect_web_serv_sock.get(), SIGNAL(error(QAbstractSocket::SocketError)),
                         this, SLOT(onConnectWebServerError(QAbstractSocket::SocketError)));
        this->m_connect_web_serv_sock->connectToHost(GATEWAY_WEB_HOSTNAME, 80);
    } else if (!this->m_connected_ws_serv) {
        qLogx()<<"Connect ws serv test:"<<this->m_gateway_router_serv_ipaddr;
        this->m_connect_ws_serv_start_time = QDateTime::currentDateTime();
        this->m_connect_ws_serv_sock = boost::shared_ptr<QTcpSocket>(new QTcpSocket());
        QObject::connect(this->m_connect_ws_serv_sock.get(), SIGNAL(connected()),
                         this, SLOT(onConnectedWebServer()));
        QObject::connect(this->m_connect_ws_serv_sock.get(), SIGNAL(error(QAbstractSocket::SocketError)),
                         this, SLOT(onConnectWebServerError(QAbstractSocket::SocketError)));
        this->m_connect_ws_serv_sock->connectToHost(this->m_gateway_router_serv_ipaddr, 80);
    } else {
        QTimer::singleShot((qrand()%30+5)*1000, this, SLOT(onDoSomething()));
    }
}


void NetworkChecker::onResolveWebServerDone(QHostInfo hi)
{
    QList<QHostAddress> addrs = hi.addresses();
    qLogx()<<addrs;

    if (addrs.count() > 0) {
        this->m_gateway_web_serv_ipaddr = addrs.at(0).toString();

        QString ws_serv_ipaddr;
        if (this->m_gateway_web_serv_ipaddr == "202.108.15.80") {
            ws_serv_ipaddr = "202.108.15.81";
            this->m_gateway_router_serv_ipaddr = ws_serv_ipaddr;
            this->m_network_line_type = NLT_WANGTONG;
        } else if (this->m_gateway_web_serv_ipaddr == "211.100.41.6") {
            ws_serv_ipaddr = "211.100.41.7";
            this->m_gateway_router_serv_ipaddr = ws_serv_ipaddr;
            this->m_network_line_type = NLT_DIANXING;
        } else {
            qLogx()<<"You network is strange enought.";
            Q_ASSERT(1==2);
        }
    }

    emit whatToDoNext();
}

void NetworkChecker::onConnectedWebServer()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    
    if (sock == this->m_connect_web_serv_sock.get()) {
        this->m_connect_web_serv_end_time = QDateTime::currentDateTime();
        this->m_connected_web_serv = true;

        qLogx()<<"connect web/ws server ok"<<sock->peerAddress()
               <<"Using time(ms):"
               <<this->m_connect_web_serv_start_time.msecsTo(this->m_connect_web_serv_end_time);

    } else if (sock == this->m_connect_ws_serv_sock.get()) {
        this->m_connect_ws_serv_end_time = QDateTime::currentDateTime();
        this->m_connected_ws_serv = true;

        qLogx()<<"connect web/ws server ok"<<sock->peerAddress()
               <<"Using time(ms):"
               <<this->m_connect_ws_serv_start_time.msecsTo(this->m_connect_ws_serv_end_time);
    }

    this->m_connect_web_serv_sock->close();

    emit whatToDoNext();
}

void NetworkChecker::onConnectWebServerError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());

    if (sock == this->m_connect_web_serv_sock.get()) {
        this->m_connect_web_serv_end_time = QDateTime::currentDateTime();

        qLogx()<<"connect web/ws server error"<<sock->peerAddress()<<socketError<<sock->errorString()
               <<"Using time(ms):"
               <<this->m_connect_web_serv_start_time.msecsTo(this->m_connect_web_serv_end_time);

    } else if (sock == this->m_connect_ws_serv_sock.get()) {
        this->m_connect_ws_serv_end_time = QDateTime::currentDateTime();

        qLogx()<<"connect web/ws server error"<<sock->peerAddress()<<socketError<<sock->errorString()
               <<"Using time(ms):"
               <<this->m_connect_ws_serv_start_time.msecsTo(this->m_connect_ws_serv_end_time);
    }

    emit whatToDoNext();
}


