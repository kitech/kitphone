// configs.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-25 15:54:57 +0800
// Version: $Id: configs.cpp 897 2011-05-28 14:25:04Z drswinghead $
// 

#include "configs.h"

#include <algorithm>

#define COMMENT_STRING "#"

Configs::Configs(QObject *parent)
    : QObject(parent)
{
}

Configs::~Configs()
{
}

QString Configs::_conf_file_path()
{
    QString path;
    pid_t my_pid = 0;
    QString link;

    if (!this->m_conf_path.isEmpty()) {
        return this->m_conf_path;
    }

    my_pid = getpid();
    link = QString("/proc/%1/exe").arg(my_pid);  // linux only

    path = QFileInfo(link).symLinkTarget();
    Q_ASSERT(!path.isEmpty());

    path = QFileInfo(path).absolutePath();
    path = path + QString("/skyserv.ini");
    // path = QString(getenv("HOME")) + QString("/skyserv.ini"); // "/home/gzleo/skyserv.ini";
    // path = QCoreApplication::applicationDirPath() + QString("/skyserv.ini");

    this->m_conf_path = path;

    // qDebug()<<"conf ini path:"<<path;

    return path;
}

QHash<QString, QString> Configs::getDatabaseInfo()
{
    QHash<QString, QString> db_info;
    QStringList keys;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("database");
    keys = sets.childKeys();
    
    QString vstr;
    for (int i = 0 ; i < keys.count(); ++i) {
        vstr = sets.value(keys.at(i)).toString();
        if (!vstr.trimmed().startsWith(COMMENT_STRING)) {
            db_info.insert(keys.at(i), vstr);
        }
    }

    return db_info;
}

QHash<QString, int> Configs::getSipServers()
{
    QHash<QString, int> servs;
    QStringList keys;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("sip_server");
    keys = sets.childKeys();
    
    int vint;
    for (int i = 0 ; i < keys.count(); ++i) {
        vint = sets.value(keys.at(i)).toInt();
        if (!keys.at(i).trimmed().startsWith(COMMENT_STRING)) {
            servs.insert(keys.at(i).trimmed(), vint);
        }
    }

    return servs;
}

QString Configs::getMultiCallSkypePath()
{
    QString path;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("common");

    path = sets.value("skype_20_path").toString();
    
    return path;
}

QString Configs::getSingleCallSkypePath()
{
    QString path;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("common");

    path = sets.value("skype_21_path").toString();

    return path;
}


QVector<QPair<QString,QString> > Configs::getSkypeRouters()
{
    QVector<QPair<QString,QString> > routers;
    QStringList keys;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("ctrl_switcher");
    keys = sets.childKeys();

    std::for_each(keys.begin(), keys.end(),
                   [&] (QString elm) {
                      if (!elm.trimmed().startsWith(COMMENT_STRING)) {
                          routers.append(QPair<QString, QString>(elm, sets.value(elm).toString()));
                      }
                   });

    // routers = keys;
    return routers;
}

// QStringList Configs::getSkypeRouters()
// {
//     QStringList routers;
//     QStringList keys;

//     QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
//     sets.beginGroup("ctrl_switcher");
//     keys = sets.childKeys();

//     routers = keys;
//     return routers;
// }

unsigned short Configs::getRouterPort()
{
    unsigned short router_port = 8000;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("skyserv");

    router_port = sets.value("router_ws_port").toString().toUShort();

    return router_port;;
    
}

int Configs::getSessionTimeout()
{
    int sess_timeout = 10;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("skyserv");

    sess_timeout = sets.value("sess_timeout").toInt();

    return sess_timeout;
}

QVector<QPair<QString, QString> > Configs::getSwitchers()
{
    QVector<QPair<QString, QString> > switchers;

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("audio_switcher");

    QString users = sets.value("AUDIO_SWITCHER_USERS").toString();
    QString passes = sets.value("AUDIO_SWITCHER_PASSWORDS").toString();

    QStringList user_list = users.split(' ');
    QStringList pass_list = passes.split(' ');

    if (user_list.count() != pass_list.count()) {
        qDebug()<<"user and pass count not match."
                <<user_list<<pass_list;
        Q_ASSERT(user_list.count() == pass_list.count());
    }

    for (int i = 1 ; i < user_list.count(); i++) {
        switchers.append(QPair<QString,QString>(user_list.at(i), pass_list.at(i)));
    }

    return switchers;
}

QString Configs::get_storage_path()
{
    QString path;
    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("common");

    path = sets.value("storage_path_base").toString();

    return path;
}

QString Configs::get_sk_path()
{
    QString path;
    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("common");

    path = sets.value("sk_path_base").toString();

    return path;
}

QString Configs::get_rt_name()
{
    QString path;
    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("common");

    path = sets.value("sk_rt_name").toString();

    return path;
}

QString Configs::get_app_token()
{
    QString path;
    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("common");

    path = sets.value("sk_app_token").toString();

    return path;
}

QString Configs::get_self_app_path()
{
    QString path;
    pid_t my_pid = 0;
    QString link;

    if (!this->m_conf_path.isEmpty()) {
        return this->m_conf_path;
    }

    my_pid = getpid();
    link = QString("/proc/%1/exe").arg(my_pid);  // linux only

    path = QFileInfo(link).symLinkTarget();
    Q_ASSERT(!path.isEmpty());

    path = QFileInfo(path).absolutePath();
    // path = path + QString("/skyserv.ini");
    // path = QString(getenv("HOME")) + QString("/skyserv.ini"); // "/home/gzleo/skyserv.ini";
    // path = QCoreApplication::applicationDirPath() + QString("/skyserv.ini");

    // this->m_conf_path = path;

    // qDebug()<<"conf ini path:"<<path;

    return path;

}

QString Configs::get_log_path() 
{
    QString path;
    QString storage_path_base = this->get_storage_path();

    QSettings sets(this->_conf_file_path(), QSettings::IniFormat);
    sets.beginGroup("skyserv");

    path = sets.value("log_dir").toString();

    path = QString("%1/%2/%3").arg(QString(getenv("HOME"))).arg(storage_path_base).arg(path);

    return path;
}

