// sipaccount.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-21 16:55:53 +0800
// Version: $Id: sipaccount.h 876 2011-05-11 14:25:21Z drswinghead $
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
    static SipAccount fromSqlRecord(QSqlRecord &rec);
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

// class SipAccountList : public QObject
// {
//     Q_OBJECT;
// public:
//     static SipAccountList *instance();
//     virtual ~SipAccountList();
    
//     QVector<SipAccount> loadAccounts();

// protected:
//     SipAccountList(QObject *parent = 0);

// private:
//     static SipAccountList *mInstance;

// private:
//     QHash<int, SipAccount> maccs;
//     QVector<SipAccount> maccs2;
// };


#endif /* _SIPACCOUNT_H_ */
