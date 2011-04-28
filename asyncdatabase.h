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
#include "boost/signals2.hpp"
#include "boost/function.hpp"

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
    void results(const QList<QSqlRecord> & records, int seqno, bool eret, const QString &estr, const QVariant &eval);
    void connected();

protected:
    void run();

signals:
    void executefwd(const QString &query, int reqno);

private:
    DatabaseWorker *m_worker;
    // static int m_reqno;
    static QAtomicInt m_reqno;

    // std::atomic<bool> m_connected;
    bool m_connected;
};

//// sql request object

class SqlRequest : public QObject
{
    Q_OBJECT;
public:
    explicit SqlRequest() {
        this->mReqno = -1;
        this->mRet = false;
        this->mCbObject = nullptr;
        this->mCbSlot = nullptr;
    }
    virtual ~SqlRequest() {}
    
    int mReqno;
    QString mSql;

    bool mRet;
    QString mErrorString;
    QVariant mExtraValue;
    QList<QSqlRecord> mResults;
    
    // functor, boost type
    boost::function<bool(boost::shared_ptr<SqlRequest>)> mCbFunctor;

    // call back of qt slot
    QObject *mCbObject;
    const char *mCbSlot;
};

#endif /* _ASYNCDATABASE_H_ */
