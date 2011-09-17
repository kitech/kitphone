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

//// sql request object
class SqlRequest : public QObject
{
    Q_OBJECT;
public:
    explicit SqlRequest() {
        this->mReqno = -1;
        this->mRet = false;
        this->mErrCnt = 0;
        this->mCbObject = nullptr;
        this->mCbSlot = nullptr;

        this->mCbData = nullptr;
    }
    virtual ~SqlRequest() {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    }
    
    int mReqno;
    QString mSql;
    QStringList mSqls;

    bool mRet;
    int mErrCnt;
    QString mErrorString;
    QVariant mExtraValue; // 一向用于last insert id
    QList<QSqlRecord> mResults;
    
    // functor, boost type
    boost::function<bool(boost::shared_ptr<SqlRequest>)> mCbFunctor;

    // call back of qt slot
    QObject *mCbObject;
    const char *mCbSlot;

    int mCbId;
    void *mCbData;
};

///////////////////////////
class AsyncDatabase : public QThread
{
    Q_OBJECT;
public:
    explicit AsyncDatabase(QObject *parent = 0);
    virtual ~AsyncDatabase();

    bool isConnected() { return this->m_connected; }
    int execute(const QString &query); // 返回一个执行号码
    int execute(const QStringList &querys); // 返回一个执行号码
    int syncExecute(const QString &query, QList<QSqlRecord> &records);

    // utils
    QString escapseString(const QString &str);

public slots:
    void onConnected();
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
    void executefwd(const QStringList &querys, int reqno);

private:
    DatabaseWorker *m_worker;
    // static int m_reqno;
    static QAtomicInt m_reqno;

    // std::atomic<bool> m_connected;
    bool m_connected;

};


#endif /* _ASYNCDATABASE_H_ */
