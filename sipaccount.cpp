// sipaccount.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-21 16:56:01 +0800
// Version: $Id: sipaccount.cpp 876 2011-05-11 14:25:21Z drswinghead $
// 

#include "sipaccount.h"

SipAccount::SipAccount()
{
    this->enabled = 0;
    this->uid = -1;
}

SipAccount::~SipAccount()
{
}

// QString SipAccount::_getAccountFile()
// {
//     QString acc_db_path;
//     acc_db_path = QCoreApplication::applicationDirPath() + "/accounts.db";
//     return acc_db_path;
// }
// bool SipAccount::save(SipAccount &acc)
// {
//     QString acc_db_path;

//     acc_db_path = this->_getAccountFile();

//     QSettings sets(acc_db_path, QSettings::IniFormat);

//     sets.beginGroup(acc.userName);
//     sets.setValue("user_name", acc.userName);
//     sets.setValue("password", acc.password);
//     sets.setValue("domain", acc.domain);
//     sets.setValue("enabled", acc.enabled);
//     sets.endGroup();

//     sets.sync();

//     return true;
// }

// bool SipAccount::remove(QString key)
// {
//     QString acc_db_path;

//     acc_db_path = this->_getAccountFile();

//     QSettings sets(acc_db_path, QSettings::IniFormat);
//     QStringList names = sets.childGroups();

//     if (names.contains(key)) {
//         sets.remove(key);
//         sets.sync();
//         return true;
//     }
//     return false;
// }

// SipAccount SipAccount::getAccount(QString key)
// {
//     QString acc_db_path;

//     acc_db_path = this->_getAccountFile();

//     QSettings sets(acc_db_path, QSettings::IniFormat);
//     QStringList names = sets.childGroups();

//     SipAccount acc;
//     if (names.contains(key)) {
//         sets.beginGroup(key);
//         acc.userName = key;
//         acc.password = sets.value("password").toString();
//         acc.domain = sets.value("domain").toString();
//         acc.enabled = sets.value("enabled").toInt();
//         sets.endGroup();
//     }

//     return acc;
// }

// QVector<SipAccount> SipAccount::listAccounts()
// {
//     QVector<SipAccount> accs;
//     QString acc_db_path;

//     acc_db_path = this->_getAccountFile();

//     QSettings sets(acc_db_path, QSettings::IniFormat);
//     QStringList names = sets.childGroups();
    
//     // qDebug()<<"names:"<<names<<sets.status();

//     SipAccount acc;
//     for (int i = 0 ; i < names.count(); i++) {
//         sets.beginGroup(names.at(i));
//         acc.userName = names.at(i);
//         acc.password = sets.value("password").toString();
//         acc.domain = sets.value("domain").toString();
//         acc.enabled = sets.value("enabled").toInt();
//         sets.endGroup();

//         accs.append(acc);
//     }
    
//     return accs;
// }

SipAccount SipAccount::fromSqlRecord(QSqlRecord &rec)
{
    SipAccount acc;

    acc.uid = rec.value("aid").toInt();
    acc.displayName = acc.userName = rec.value("account_name").toString();
    acc.password = rec.value("account_password").toString();
    acc.domain = rec.value("serv_addr").toString();
    acc.ctime = rec.value("account_ctime").toString();
    acc.ctime = rec.value("account_mtime").toString();
    acc.status = rec.value("account_status").toInt();

    return acc;
}

void SipAccount::dump()
{
    qDebug()<<"Sip account:";
    qDebug()<<"user name:"<<this->userName;
    qDebug()<<"domain::"<<this->domain;
}

////////////////////////

// SipAccountList *SipAccountList::mInstance = NULL;
// SipAccountList::SipAccountList(QObject *parent)
//     : QObject(parent)
// {}
// SipAccountList::~SipAccountList()
// {
// }

// SipAccountList *SipAccountList::instance()
// {
//     if (SipAccountList::mInstance == NULL) {
//         SipAccountList::mInstance  = new SipAccountList();
//     }

//     return SipAccountList::mInstance;
// }

// QVector<SipAccount> SipAccountList::loadAccounts()
// {
//     QVector<SipAccount> accs;

//     accs = SipAccount().listAccounts();

//     this->maccs2 = accs;

//     return accs;
// }
