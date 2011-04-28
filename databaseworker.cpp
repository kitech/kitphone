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
#define DATABASE_NAME "kitsession.edb"
#define DATABASE_HOST ""
#define DATABASE_DRIVER "QSQLITE"
#define SAMPLE_RECORDS 100000

#define TABLE_CONTACTS "kp_contacts"
#define TABLE_HISTORIES "kp_histories"
#define TABLE_OPTIONS "kp_options"
#define TABLE_GROUPS "kp_groups"
#define TABLE_ACCOUNTS "kp_accounts"

//
DatabaseWorker::DatabaseWorker( QObject* parent )
    : QObject( parent )
{

}

bool DatabaseWorker::connectDatabase()
{
    // thread-specific connection, see db.h
#ifdef WIN32
    QString db_file_path = qApp->applicationDirPath() + "/" + DATABASE_NAME;
#else
    QString db_file_path = QDir::homePath() + "/.kitphone/" + DATABASE_NAME;
    if (!QDir().exists(QDir::homePath() + "/.kitphone/")) {
        QDir().mkdir(QDir::homePath() + "/.kitphone/");
    }
    // for test
    db_file_path = qApp->applicationDirPath() + "/" + DATABASE_NAME;
#endif
    m_database = QSqlDatabase::addDatabase( DATABASE_DRIVER, 
                                            "WorkerDatabase" ); // named connection
    // m_database.setDatabaseName( DATABASE_NAME );
    m_database.setDatabaseName(db_file_path);
    m_database.setHostName( DATABASE_HOST );
    m_database.setUserName( DATABASE_USER );
    m_database.setPassword( DATABASE_PASS );
    if ( !m_database.open() ) {
        qWarning() << "Unable to connect to database, giving up:" << m_database.lastError().text();
        emit this->connect_error();
        return false;
    }

    // static std::vector<QString> tbls = {TABLE_OPTIONS, TABLE_CONTACTS, TABLE_HISTORIES};
    
    bool bok;
    QSqlQuery q;
    if (!m_database.tables().contains(TABLE_OPTIONS)) {
        // some data
        QString sql = QString("CREATE TABLE %1 (key VARCHAR(20) PRIMARY KEY, value VARCHAR(32));").arg(TABLE_OPTIONS);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_OPTIONS<<q.lastQuery()<<q.lastError();
    }

    /*
      CREATE TABLE TABLE_GROUPS (
      gid INTERGER PRIMARY KEY AUTOINCREMENT,
      group_name VARCHAR(100) UNIQUE
      );
     */
    if (!m_database.tables().contains(TABLE_GROUPS)) {
        // some data
        QString sql = QString("CREATE TABLE %1 (gid INTEGER PRIMARY KEY AUTOINCREMENT, group_name VARCHAR(100) UNIQUE);").arg(TABLE_GROUPS);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_GROUPS<<q.lastQuery()<<q.lastError();

        QSqlQuery query(m_database);
        query.prepare(QString("INSERT INTO %1 (gid,group_name) VALUES (?,?)").arg(TABLE_GROUPS));

        query.addBindValue(1);
        query.addBindValue("Family");
        query.exec();

        query.addBindValue(2);
        query.addBindValue("Friends");
        query.exec();

        query.addBindValue(3);
        query.addBindValue("Others");
        query.exec();

        m_database.commit();

        qDebug()<<TABLE_GROUPS<<query.lastQuery()<<query.lastError();

        // query.exec(QString("INSERT INTO %1 (group_name) VALUES ('vvvvvvvvvvvvvv')").arg(TABLE_GROUPS));
        // qDebug()<<TABLE_GROUPS<<query.lastQuery()<<query.lastError();
    }
    /*
      CREATE TABLE TABLE_CONTACTS (
      cid INTEGER PRIMARY KEY ,
      group_id INTEGER NOT NULL,
      display_name VARCHAR(100) UNIQUE,
      phone_number VARCHAR(100) UNIQUE
      );
     */
    if (!m_database.tables().contains(TABLE_CONTACTS)) {
        // some data
        QString sql = QString("CREATE TABLE %1 (cid INTEGER PRIMARY KEY AUTOINCREMENT, group_id INTERGER NOT NULL, display_name VARCHAR(100) UNIQUE, phone_number VARCHAR(100) UNIQUE);").arg(TABLE_CONTACTS);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_CONTACTS<<q.lastQuery()<<q.lastError();
    }

    /*
      Basically, create a column of type INTEGER PRIMARY KEY or a column called ROWID, then don't specify the value when inserting a row.
      CREATE TABLE TABLE_HISTORIES (
      hid INTEGER PRIMARY KEY ,
      contact_id INTEGER NOT NULL,
      call_status INTEGER,
      call_ctime VARCHAR(100),
      call_etime VARCHAR(100)
      );
     */

    if (!m_database.tables().contains(TABLE_HISTORIES)) {
        // some data
        QString sql = QString("CREATE TABLE %1 (hid INTEGER PRIMARY KEY AUTOINCREMENT, contact_id INTEGER NOT NULL, call_status INTEGER, call_ctime VARCHAR(100), call_etime VARCHAR(100));").arg(TABLE_HISTORIES);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_HISTORIES<<q.lastQuery()<<q.lastError();
    }
 
    // for account manager
    /*
      CREATE TABLE TABLE_ACCOUNTS (
      aid INTEGER PRIMARY KEY ,
      account_name VARCHAR(100),
      account_password VARCHAR(100),
      display_name VARCHAR(100),
      serv_addr VARCHAR(100),
      account_status INTEGER,
      account_ctime VARCHAR(100),
      account_etime VARCHAR(100)
      );
     */

    if (!m_database.tables().contains(TABLE_ACCOUNTS)) {
        // some data
        QString sql = QString("CREATE TABLE %1 (aid INTEGER PRIMARY KEY AUTOINCREMENT, account_name VARCHAR(100), account_password VARCHAR(100), display_name VARCHAR(100), serv_addr VARCHAR(100), account_status INTEGER, account_ctime VARCHAR(100), account_etime VARCHAR(100));").arg(TABLE_ACCOUNTS);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_HISTORIES<<q.lastQuery()<<q.lastError();
    }

    emit this->connected();

    return true;
    // std::for_each(tbls.begin(), tbls.end(),
    //               [&tbls,&m_database] (const QString &tbl) {
    //               });
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
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    this->m_database.close();

    // 如果不用这句，则会有警告。
    // QSqlDatabasePrivate::removeDatabase: connection 'WorkerDatabase' is still in use, all queries will cease to work.
    this->m_database = QSqlDatabase::addDatabase(DATABASE_DRIVER, "database_driver_destructor");
    QSqlDatabase::removeDatabase("WorkerDatabase");
}

void DatabaseWorker::slotExecute(const QString& query, int reqno)
{
    bool eret = false;
    QString estr;
    QVariant eval;
    QSqlError edb;
    QList<QSqlRecord> recs;
    QSqlQuery dbq(m_database);

    eret = dbq.exec(query);
    if (!eret) {
        edb = dbq.lastError();
        estr = QString("ENO:%1, %2").arg(edb.type()).arg(edb.text());
    } else {
        eval = dbq.lastInsertId();
        while ( dbq.next() ) {
            recs.push_back(dbq.record() );
        }
    }
    emit results(recs, reqno, eret, estr, eval);
}

