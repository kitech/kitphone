// configs.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-25 15:53:52 +0800
// Version: $Id: configs.h 822 2011-04-11 02:44:20Z drswinghead $
// 

#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <QtCore>

class Configs : public QObject
{
    Q_OBJECT;
public:
    explicit Configs(QObject *parent = 0);
    virtual ~Configs();

public:
    QHash<QString, QString> getDatabaseInfo();
    QHash<QString, int> getSipServers();
    // QStringList getSkypeRouters();
    QVector<QPair<QString,QString> > getSkypeRouters();

    QString getMultiCallSkypePath();
    QString getSingleCallSkypePath();

    unsigned short getRouterPort();
    int getSessionTimeout();

    //            user name, password
    QVector<QPair<QString, QString> > getSwitchers();

    QString get_storage_path();
    QString get_sk_path();
    QString get_rt_name();
    QString get_app_token();

    QString get_self_app_path();

    QString get_log_path();

    QString _conf_file_path();
private:

    QString m_conf_path;
};


#endif /* _CONFIGS_H_ */
