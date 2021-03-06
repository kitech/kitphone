// skypetunnel.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-09-08 14:11:53 +0800
// Version: $Id: skypetunnel.h 998 2011-09-17 11:03:58Z drswinghead $
// 

#ifndef _SKYPETUNNEL_H_
#define _SKYPETUNNEL_H_

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class SkypePackage;
class Skycit;

class SkypeAsyncConnector : public QThread
{
    Q_OBJECT;
public:
    explicit SkypeAsyncConnector(QObject *parent = 0);
    virtual ~SkypeAsyncConnector();

    void run();

private slots:
    
public:
    Skycit *mSkype;
};

class SkypeTunnel : public QObject
{
    Q_OBJECT;
public:
    SkypeTunnel(QObject *parent = 0);
    virtual ~SkypeTunnel();

    void setSkype(Skycit *skype);
    void setStreamPeer(QString streamPeerName);
    void setPhoneNumber(QString phoneNumber);

public slots:
    void onSkypeAsyncConnectFinished();
    void onSkypeError(int errNo, QString msg, QString cmd);
    void onSkypeConnected(QString skypeName);
    void onSkypeDisconnected(QString skypeName);
    void onNewStreamCreated(QString contactName, int stream);
    void onStreamClosed();
    void onSkypePackageArrived(QString contactName, int stream, QString data);
    void onDtmfSended(int callID, QString dtmf);
    void onNewCallArrived(QString callerName, QString calleeName, int callID);
    void onSkypeCallAnswered(int callID, QString callerName, QString calleeName);
    void onCallHangup(QString contactName, QString callerName, int callID);

    void processRequest(QString contactName, int stream, SkypePackage *sp);

    void skypeCallFriend(QString router, QString num);

private slots:
    void reconnectStream();
    void restoreVolume();

    void onPresendFinished();
    void onPresendReadyRead();

private:
    Skycit *mSkype;
    QString mStreamPeerName;
    QTimer *mStreamReconnectTimer;
    int mStreamReconnectRetry;
    QString mPhoneNumber;
    unsigned int mVolume;
    bool mycall;
    QNetworkAccessManager *mNetAM;
    QHash<QNetworkReply*, QByteArray> mPresendResult;
};

#endif /* _SKYPETUNNEL_H_ */
