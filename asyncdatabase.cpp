// asyncdatabase.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-24 20:17:39 +0800
// Version: $Id$
// 

#include "databaseworker.h"
#include "asyncdatabase.h"

// #include "querythread.h"
// #include "db.h"

#include <QDebug>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>



// TODO 还有一个问题，就是怎么区分请求与返回值的问题。
// 因为不同的返回结果需要不同的处理方式。
////

QAtomicInt AsyncDatabase::m_reqno = 1;
AsyncDatabase::AsyncDatabase(QObject *parent)
    : QThread(parent)
{
    this->m_worker = nullptr;
    this->m_connected = false;
}

AsyncDatabase::~AsyncDatabase()
{
    delete m_worker;
}

void AsyncDatabase::onConnected() { 
    this->m_connected = true; 
    emit this->connected();
}

int AsyncDatabase::execute(const QString& query)
{
    // int reqno = this->m_reqno++;
    int reqno = this->m_reqno.fetchAndAddOrdered(1);
    emit executefwd(query, reqno); // forwards to the worker
    return reqno;
}

int AsyncDatabase::execute(const QStringList& querys)
{
    // int reqno = this->m_reqno++;
    int reqno = this->m_reqno.fetchAndAddOrdered(1);
    emit executefwd(querys, reqno); // forwards to the worker
    return reqno;
}

int AsyncDatabase::syncExecute(const QString &query, QList<QSqlRecord> &records)
{
    int iret;

    iret = this->m_worker->syncExecute(query, records);

    return 0;
}

QString AsyncDatabase::escapseString(const QString &str)
{
    return this->m_worker->escapseString(str);
}

void AsyncDatabase::run()
{
    emit ready(false);
    emit progress( "AsyncDatabase starting, one moment please..." );

    // Create worker object within the context of the new thread
    m_worker = new DatabaseWorker();
    QObject::connect(m_worker, SIGNAL(connected()),
                     this, SLOT(onConnected()));

    QObject::connect(this, SIGNAL(executefwd(const QString&, int)),
            m_worker, SLOT(slotExecute(const QString&,int)));
    QObject::connect(this, SIGNAL(executefwd(const QStringList&, int)),
            m_worker, SLOT(slotExecute(const QStringList&,int)));
    
    m_worker->connectDatabase();

    // Critical: register new type so that this signal can be
    // dispatched across thread boundaries by Qt using the event
    // system
    qRegisterMetaType< QList<QSqlRecord> >( "QList<QSqlRecord>" );

    // forward final signal
    QObject::connect(m_worker, SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)),
                     this, SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)));

    emit progress( "Press 'Go' to run a query." );
    emit ready(true);

    this->exec();  // our event loop
}

// #include "querythread.moc"
