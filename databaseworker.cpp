// databaseworker.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-24 20:19:15 +0800
// Version: $Id$
// 

#include <algorithm>

#include "databaseworker.h"


// Configure runtime parameters here 
#define DATABASE_USER "magdalena"
#define DATABASE_PASS "deedee"
// #define DATABASE_NAME "asynchdbtest.db"
#define DATABASE_NAME "kitphone.edb"
#define DATABASE_HOST ""
#define DATABASE_DRIVER "QSQLITE"
#define SAMPLE_RECORDS 100000

#define TABLE_CONTACTS "kp_contacts"
#define TABLE_HISTORIES "kp_histories"
#define TABLE_OPTIONS "kp_options"

//
DatabaseWorker::DatabaseWorker( QObject* parent )
    : QObject( parent )
{
    // thread-specific connection, see db.h
    m_database = QSqlDatabase::addDatabase( DATABASE_DRIVER, 
                                            "WorkerDatabase" ); // named connection
    m_database.setDatabaseName( DATABASE_NAME );
    m_database.setHostName( DATABASE_HOST );
    m_database.setUserName( DATABASE_USER );
    m_database.setPassword( DATABASE_PASS );
    if ( !m_database.open() ) {
        qWarning() << "Unable to connect to database, giving up:" << m_database.lastError().text();
        emit this->connect_error();
        return;
    }

    static std::vector<QString> tbls = {TABLE_OPTIONS, TABLE_CONTACTS, TABLE_HISTORIES};
    
    bool bok;
    QSqlQuery q;
    if (!m_database.tables().contains(TABLE_OPTIONS)) {
        // some data
        QString sql = QString("CREATE TABLE %1 (key VARCHAR(20) NOT NULL, value VARCHAR(32));").arg(TABLE_OPTIONS);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
    }

    emit this->connected();

    std::for_each(tbls.begin(), tbls.end(),
                  [&tbls,&m_database] (const QString &tbl) {
                  });
    // initialize db
    // if (!m_database.tables().contains( "item" ) )
    // {
    //     // some data
    //     m_database.exec( "create table item(id int, name varchar);" );
    //     m_database.transaction();
    //     QSqlQuery query(m_database);
    //     query.prepare("INSERT INTO item (id, name) "
    //                   "VALUES (?,?)");
    //     for ( int i = 0; i < SAMPLE_RECORDS; ++i )
    //     {
    //         query.addBindValue(i);
    //         query.addBindValue(QString::number(i));
    //         query.exec();
    //     }
    //     m_database.commit();
    // }
}

DatabaseWorker::~DatabaseWorker()
{
}

void DatabaseWorker::slotExecute(const QString& query, int reqno)
{
    QList<QSqlRecord> recs;
    QSqlQuery sql( query, m_database );
    while ( sql.next() ) {
        recs.push_back( sql.record() );
    }
    emit results(recs, reqno);
}

