// sipaccount.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-21 16:55:53 +0800
// Version: $Id: sipaccount.h 956 2011-08-24 08:53:52Z drswinghead $
// 

#ifndef _SIPACCOUNT_H_
#define _SIPACCOUNT_H_

#include <QtCore>
#include <QtSql>

#include "sipaccount.h"

class SipAccount
{
public:
    explicit SipAccount();
    virtual ~SipAccount();

    /*
    bool save(SipAccount &acc);
    bool remove(QString key);
    SipAccount getAccount(QString key);

    QVector<SipAccount> listAccounts();
    */
    static SipAccount fromSqlRecord(const QSqlRecord &rec);
    void dump();
private:
    // QString _getAccountFile();

public:
    int    enabled;
    int uid;
    QString userName;
    QString password;
    QString domain;
    QString displayName;
    QString ctime;
    QString mtime;
    int status;

private:
    
};


#endif /* _SIPACCOUNT_H_ */
