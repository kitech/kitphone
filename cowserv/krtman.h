// krtman.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-05 22:44:57 +0800
// Version: $Id: krtman.h 672 2011-01-07 01:20:27Z drswinghead $
// 

#ifndef _KRTMAN_H_
#define _KRTMAN_H_

#include <QtCore>
#include <QThread>
#include <QString>
#include <QHash>
#include <QProcess>

class KitRuntimeManager : public QThread
{
    Q_OBJECT;
public:
    KitRuntimeManager(int count, QObject *parent = 0);
    ~KitRuntimeManager();

    void run();

private:
    int manual_start(QProcess *proc, int seq);

private slots:
    void on_error(QProcess::ProcessError error);
    void on_finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void on_readyReadStandardError ();
    void on_readyReadStandardOutput ();
    void on_started ();
    void on_stateChanged ( QProcess::ProcessState newState );

private:
    QString rt_path;
    QString key_path_base;
    QString sk_dbpath_base;
    int rt_count;
    unsigned short port_base;
    QHash<QProcess*, int> procs; // proc, idx
};

#endif /* _KRTMAN_H_ */
