// vomcall.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-16 22:24:54 +0800
// Version: $Id: vomcall.h 576 2010-11-18 03:40:16Z drswinghead $
// 
#ifndef _VOMCALL_H_
#define _VOMCALL_H_

#include <QtCore>
#include <QtNetwork>

#include "skaccount.h"

class MySkype;

class VomCall : public QObject
{
    Q_OBJECT;
public:
    explicit VomCall(int pArgc, char **pArgv, QObject *parent = 0);
    virtual ~VomCall();

public:
    bool parseArgs();
    bool login();
    bool logout();

    bool callSkype(const char *pSkypeName);
public slots:
    void onLoginComplete();

public: // sip area

private slots:
    void onPresendFinished();
    void onPresendReadyRead();

private:
    MySkype *mSkykit;
    MyAccount::Ref acc;
    int mArgc;
    char **mArgv;
    uint mPort;
    std::string mLoginName;
    std::string mLoginPassword;
    std::string mPhoneNumber;
    std::string mRouter;

    bool mycall;
    QNetworkAccessManager *mNetAM;
    QHash<QNetworkReply*, QByteArray> mPresendResult;
};

#endif /* _VOMCALL_H_ */
