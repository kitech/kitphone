// database.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-07 21:26:00 +0800
// Version: $Id: database.h 170 2010-10-12 12:55:58Z drswinghead $
// 

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <QtCore>

#include "libpq-fe.h"

class Database : public QObject
{
    Q_OBJECT;
public:
    Database(QObject *parent = 0);
    virtual ~Database();

    bool connectdb();
    bool isConnected();
    bool disconnectdb();
    bool reconnectdb();

    PGconn *connection() {
        return this->conn;
    }

    /*
      ipaddr:如果不为空，则优先查找同一网段的可用语音网关，查找方法：ipv4的前三段相同即可。
     */
    int acquireGateway(const QString &caller_name, const QString &callee_phone, QString &gateway,
                       unsigned short &port, QString &ipaddr);
    int releaseGateway(const QString &caller_name, const QString &gateway);
    QString getForwardPhone(QString caller_name, QString gateway);

    int setCallPair(const QString &caller_name, const QString &callee_phone, const QString &remote_ipaddr, QStringList &routers);
    QString getCallPeer(const QString &caller_name, QString &caller_ipaddr);
    bool removeCallPair(const QString &caller_name);

    bool setForwardPort(const QString &gateway, unsigned short port, const QString &ipaddr);
    unsigned short getForwardPort(const QString &caller_name, QString &ipaddr);

    bool setLineState(QString gateway, int state);
    bool setBatchLineState(QStringList &gateways, int state);

    enum CallLineState {
        CLS_FREE=0,
        CLS_INUSE,
        CLS_QUIT,
        CLS_CRASH
    };

private:
    PGconn *conn;
};

#endif /* _DATABASE_H_ */
