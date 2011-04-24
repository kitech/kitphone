// asyncdatabase.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-24 20:17:22 +0800
// Version: $Id$
// 
#ifndef _ASYNCDATABASE_H_
#define _ASYNCDATABASE_H_

// #include <atomic>

#include <QtSql>

class DatabaseWorker;

class AsyncDatabase : public QThread
{
    Q_OBJECT;
public:
    explicit AsyncDatabase(QObject *parent = 0);
    virtual ~AsyncDatabase();

    bool isConnected() { return this->m_connected; }
    int execute(const QString &query); // 返回一个执行号码

public slots:
    void onConnected() { this->m_connected = true; }
    void onConnectError() { this->m_connected = false; }

signals:
    void progress(const QString &msg);
    void ready(bool);
    void results(const QList<QSqlRecord> & records);

protected:
    void run();

signals:
    void executefwd(const QString &query, int reqno);

private:
    DatabaseWorker *m_worker;
    static int m_reqno;

    // std::atomic<bool> m_connected;
    bool m_connected;
};

#endif /* _ASYNCDATABASE_H_ */
