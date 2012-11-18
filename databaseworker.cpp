// databaseworker.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-25 20:19:15 +0800
// Version: $Id: databaseworker.cpp 998 2011-09-17 11:03:58Z drswinghead $
// 

#include <algorithm>

#include "simplelog.h"

#include "sqlite/sqlite3.h"
#include "databaseworker.h"

// TODO encrypt session database
// TODO switch to raw sqlite c api for encrypt

// Configure runtime parameters here 
#define DATABASE_USER "magdalena"
#define DATABASE_PASS "deedee"
// #define DATABASE_NAME "asynchdbtest.db"
#define SESSDB_CONN_NAME "AsyncWorkerDatatbase"
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

DatabaseWorker::~DatabaseWorker()
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    // this->m_database.close();

    // 如果不用这句，则会有警告。
    // 现在用不用都会出这个问题。
    // QSqlDatabasePrivate::removeDatabase: connection 'WorkerDatabase' is still in use, all queries will cease to work.
    // this->m_database = QSqlDatabase::addDatabase(DATABASE_DRIVER, "database_driver_destructor");

    QSqlDatabase::removeDatabase("WorkerDatabase");
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

    QStringList drivers = QSqlDatabase::drivers();
    if (!drivers.contains(DATABASE_DRIVER)) {
        qLogx()<<"Warning no "<<DATABASE_DRIVER<<" in this Qt";
        Q_ASSERT(1==2);
        return false;
    }

    QSqlDatabase m_database = QSqlDatabase::addDatabase( DATABASE_DRIVER, 
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

    bool has_new_created_table = false;
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
      group_name VARCHAR(100) UNIQUE NOT NULL
      );
     */
    if (!m_database.tables().contains(TABLE_GROUPS)) {
        has_new_created_table = true;
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
        has_new_created_table = true;
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
      contact_id INTEGER NOT NULL, // dep???
      phone_number VARCHAR(100),
      call_status INTEGER,
      call_ctime VARCHAR(100),
      call_etime VARCHAR(100)
      );
     */

    if (!m_database.tables().contains(TABLE_HISTORIES)) {
        has_new_created_table = true;
        // some data
        QString sql = QString("CREATE TABLE %1 (hid INTEGER PRIMARY KEY AUTOINCREMENT, contact_id INTEGER NOT NULL, phone_number VARCHAR(100), call_status INTEGER, call_ctime VARCHAR(100), call_etime VARCHAR(100));").arg(TABLE_HISTORIES);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_HISTORIES<<q.lastQuery()<<q.lastError();
    }
 
    // for account manager
    /*
      CREATE TABLE TABLE_ACCOUNTS (
      aid INTEGER PRIMARY KEY ,
      account_name VARCHAR(100) NOT NULL,
      account_password VARCHAR(100),
      display_name VARCHAR(100) NOT NULL UNIQUE,
      serv_addr VARCHAR(100) NOT NULL,
      account_status INTEGER,
      account_ctime VARCHAR(100),
      account_mtime VARCHAR(100)
      );
     */

    if (!m_database.tables().contains(TABLE_ACCOUNTS)) {
        has_new_created_table = true;
        // some data
        QString sql = QString("CREATE TABLE %1 (aid INTEGER PRIMARY KEY AUTOINCREMENT, account_name VARCHAR(100) NOT NULL, account_password VARCHAR(100), display_name VARCHAR(100) NOT NULL UNIQUE, serv_addr VARCHAR(100) NOT NULL, account_status INTEGER, account_ctime VARCHAR(100), account_mtime VARCHAR(100));").arg(TABLE_ACCOUNTS);
        // m_database.exec( "create table item(id int, name varchar);" );
        q = m_database.exec(sql);
        qDebug()<<TABLE_HISTORIES<<q.lastQuery()<<q.lastError();
    }

    // 如果有新创建的表，则关闭再打开。否则可能调用端查询不到数据。
    if (has_new_created_table == true) {
        m_database.close();
        m_database.open();
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

QString DatabaseWorker::escapseString(const QString &str)
{
    QString estr;
    QSqlField fld("haha", QVariant::String);
    fld.setValue(str);

    QSqlDatabase m_database = QSqlDatabase::database("WorkerDatabase");
    estr = m_database.driver()->formatValue(fld);

    qLogx()<<"Fmt1:"<<estr<< (estr == str);

    QVariant v = m_database.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0) {
        // v.data() returns a pointer to the handle
        sqlite3 *handle = *static_cast<sqlite3 **>(v.data());
        if (handle != 0) { // check that it is not NULL
            // ...
            
        }
    }

    // retry next way
    if (1 || estr == str) {
        estr = QString();
        bool escapePercentage = false;
        std::string in = str.toStdString();
        for (std::string::const_iterator it = in.begin(); it != in.end(); it++) {
            if (*it == '\"') {
                // estr += "\\\"";
                estr += *it;
            } else if (*it == '\'') {
                // estr += "\\'";
                estr += "''";
            } else if (*it == '\\') {
                // estr += "\\\\";
                estr += *it;
            } else if (escapePercentage && (*it == '%') ) {
                estr += "\\%";
            } else {
                estr += *it;
            }
        }

        qLogx()<<"Fmt3:"<<estr<< (estr == str);

        if (estr.isEmpty()) {
            estr = str;
        }
    }

    return estr;
}

void DatabaseWorker::slotExecute(const QString& query, int reqno)
{
    bool eret = false;
    QString estr;
    QVariant eval;
    QSqlError edb;
    QList<QSqlRecord> recs;

    QSqlDatabase m_database = QSqlDatabase::database("WorkerDatabase");

    QSqlQuery dbq(m_database);
    QStringList qelms;
    QString sql;

    eret = dbq.exec(query);
    if (!eret) {
        edb = dbq.lastError();
        estr = QString("ENO:%1, %2").arg(edb.type()).arg(edb.text());
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<estr;
    } else {
        eval = dbq.lastInsertId();
        if (!eval.isValid()) {
            // not insert query
            while(dbq.next()) {
                recs.push_back(dbq.record());
            }
        } else {
            // insert query;
            qelms = query.trimmed().split(" ");
            if (qelms.at(2) == TABLE_GROUPS) {
                sql = QString("SELECT * FROM %1 WHERE gid=%2").arg(TABLE_GROUPS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_CONTACTS) {
                sql = QString("SELECT * FROM %1 WHERE cid=%2").arg(TABLE_CONTACTS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_HISTORIES) {
                sql = QString("SELECT * FROM %1 WHERE hid=%2").arg(TABLE_HISTORIES).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_ACCOUNTS) {
                sql = QString("SELECT * FROM %1 WHERE aid=%2").arg(TABLE_ACCOUNTS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());
                }
            }
        }
    }
    // qDebug()<<"QQQQ: "<<query<<recs.count();
    emit results(recs, reqno, eret, estr, eval);
}

void DatabaseWorker::slotExecute(const QStringList& querys, int reqno)
{
    bool eret = false;
    int errcnt = 0;
    QString estr;
    QVariant eval;
    QSqlError edb;
    QList<QSqlRecord> recs;

    QSqlDatabase m_database = QSqlDatabase::database("WorkerDatabase");

    QSqlQuery dbq(m_database);
    QStringList qelms;
    QString sql;

    eret = m_database.transaction();
    for (int i = 0; i < querys.count(); ++i) {
        sql = querys.at(i);
        qLogx()<<"Exec..."<<sql;
        eret = dbq.exec(sql);
        if (!eret) {
            edb = dbq.lastError();
            estr = QString("ENO:%1, %2").arg(edb.type()).arg(edb.text());
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<estr;
            ++ errcnt;
        }
    }
    eret = errcnt > 0 ? m_database.rollback() : m_database.commit();

    // eret = dbq.exec(query);
    if (!eret) {
        edb = dbq.lastError();
        estr = QString("ENO:%1, %2").arg(edb.type()).arg(edb.text());
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<estr;
    } else if (errcnt == 0) {
        eval = dbq.lastInsertId();
        if (!eval.isValid()) {
            // not insert query
            while(dbq.next()) {
                recs.push_back(dbq.record());
            }
        } else {
            // insert query;
            // qelms = query.trimmed().split(" ");
            qelms = sql.trimmed().split(" ");
            if (qelms.at(2) == TABLE_GROUPS) {
                sql = QString("SELECT * FROM %1 WHERE gid=%2").arg(TABLE_GROUPS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_CONTACTS) {
                sql = QString("SELECT * FROM %1 WHERE cid=%2").arg(TABLE_CONTACTS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_HISTORIES) {
                sql = QString("SELECT * FROM %1 WHERE hid=%2").arg(TABLE_HISTORIES).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_ACCOUNTS) {
                sql = QString("SELECT * FROM %1 WHERE aid=%2").arg(TABLE_ACCOUNTS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());
                }
            }
        }
    } else {
        qLogx()<<"Sql query error count: "<<errcnt;
    }
    // qDebug()<<"QQQQ: "<<query<<recs.count();
    emit results(recs, reqno, eret, estr, eval);
}

int DatabaseWorker::syncExecute(const QString &query, QList<QSqlRecord> &records)
{
    bool eret = false;
    QString estr;
    QVariant eval;
    QSqlError edb;
    QList<QSqlRecord> recs;

    QSqlDatabase m_database = QSqlDatabase::database("WorkerDatabase");

    QSqlQuery dbq(m_database);
    QStringList qelms;
    QString sql;

    eret = dbq.exec(query);
    if (!eret) {
        edb = dbq.lastError();
        estr = QString("ENO:%1, %2").arg(edb.type()).arg(edb.text());
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<estr;
    } else {
        eval = dbq.lastInsertId();
        if (!eval.isValid()) {
            // not insert query
            while(dbq.next()) {
                recs.push_back(dbq.record());
            }
        } else {
            // insert query;
            qelms = query.trimmed().split(" ");
            if (qelms.at(2) == TABLE_GROUPS) {
                sql = QString("SELECT * FROM %1 WHERE gid=%2").arg(TABLE_GROUPS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_CONTACTS) {
                sql = QString("SELECT * FROM %1 WHERE cid=%2").arg(TABLE_CONTACTS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_HISTORIES) {
                sql = QString("SELECT * FROM %1 WHERE hid=%2").arg(TABLE_HISTORIES).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());                    
                }
            } else if (qelms.at(2) == TABLE_ACCOUNTS) {
                sql = QString("SELECT * FROM %1 WHERE aid=%2").arg(TABLE_ACCOUNTS).arg(eval.toInt());
                eret = dbq.exec(sql);
                Q_ASSERT(eret);
                while(dbq.next()) {
                    recs.push_back(dbq.record());
                }
            }
        }
    }

    records = recs;
    
    return 0;
}
