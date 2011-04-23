// krtman.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-05 22:45:16 +0800
// Version: $Id: krtman.cpp 676 2011-01-08 16:05:54Z drswinghead $
// 

#include "configs.h"

#include "krtman.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    KitRuntimeManager manager(3);
    manager.run();

    return a.exec();
    return 0;
}

/////////////
////////////
KitRuntimeManager::KitRuntimeManager(int count, QObject *parent)
    : QThread(parent)
{
    this->port_base = 8950;
    this->rt_count = count;
    this->rt_path = "";

    // detect skrt dir
    QString home_dir = QString(getenv("HOME"));
    this->rt_path = home_dir + "/" + Configs().get_sk_path();
    this->rt_path += "/" + Configs().get_rt_name();
    // the path should get from .ini file
    // this->rt_path = home_dir + QString("/sxxxxxkxxxit/dis-kkkkkkkkk-path/bin/linux-x86");
    // this->rt_path += "/linux-x86-skypekit-voicepcm-novideo";

    // ~/skype_cluster/ipc_keys/ipc_key_0/
    this->key_path_base = home_dir + "/" + Configs().get_storage_path() + "/ipc_keys/ipc_key"; 
    // qDebug()<<"File path:"<<this->rt_path<<this->key_path_base;

    this->sk_dbpath_base = home_dir + "/" + Configs().get_storage_path() + "/gateways";
}

KitRuntimeManager::~KitRuntimeManager()
{

}

void KitRuntimeManager::run()
{
    QProcess *proc = NULL;
    for (int i = 0 ; i < this->rt_count ; i ++) {
        proc = new QProcess();
        QObject::connect(proc, SIGNAL(error(QProcess::ProcessError)),
                         this, SLOT(on_error(QProcess::ProcessError)));
        QObject::connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                         this, SLOT(on_finished(int, QProcess::ExitStatus)));
        QObject::connect(proc, SIGNAL(readyReadStandardError()),
                         this, SLOT(on_readyReadStandardError()));
        QObject::connect(proc, SIGNAL(readyReadStandardOutput()),
                         this, SLOT(on_readyReadStandardOutput()));
        QObject::connect(proc, SIGNAL(started()),
                         this, SLOT(on_started()));
        QObject::connect(proc, SIGNAL(stateChanged(QProcess::ProcessState)),
                         this, SLOT(on_stateChanged(QProcess::ProcessState)));

        this->procs.insert(proc, i);

        this->manual_start(proc, i);
    }
}

int KitRuntimeManager::manual_start(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;

    str = this->key_path_base + QString("_%1/").arg(seq);
    if (!QDir().exists(str)) {
        QDir().mkpath(str);
    }

    str2 = this->sk_dbpath_base + QString("/sd_%1").arg(seq);
    if (!QDir().exists(str2)) {
        QDir().mkpath(str2);
    }

    str3 = this->key_path_base + QString("_un_name_%1.sock").arg(seq);

    args.clear();
    args<<"-m";
    // args<<"-p" << QString::number(this->port_base + seq);
    args<<"-l" << str3;
    args<<"-s" << str;
    args<<"-f" << str2;
    args<<"-d" << str2;

    proc->start(this->rt_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);

    // qDebug()<<args;

    return 0;
}

void KitRuntimeManager::on_error(QProcess::ProcessError error)
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<error<<proc->errorString();
}

void KitRuntimeManager::on_finished ( int exitCode, QProcess::ExitStatus exitStatus )
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<exitCode<<proc->error()<<proc->errorString();

    int seq = -1;
    switch (exitStatus) {
    case QProcess::CrashExit:
        Q_ASSERT(this->procs.contains(proc));
        seq = this->procs.value(proc);
        Q_ASSERT(seq >= 0 && seq < 10000);
        this->manual_start(proc, seq);
        break;
    case QProcess::NormalExit:
        // maybe should exit
        break;
    default:
        break;
    };

    switch (proc->error()) {
    default:
        break;
    };
}

void KitRuntimeManager::on_readyReadStandardError ()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;

    QByteArray ba = proc->readAllStandardError();
    qDebug()<<"INFO:"<<this->procs.value(proc)<<ba.trimmed();
}

void KitRuntimeManager::on_readyReadStandardOutput ()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    Q_UNUSED(proc);
}

void KitRuntimeManager::on_started ()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    Q_UNUSED(proc);
}

void KitRuntimeManager::on_stateChanged ( QProcess::ProcessState newState )
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<newState<<proc->pid();
    Q_UNUSED(proc);
}


