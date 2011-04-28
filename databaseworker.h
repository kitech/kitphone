// databaseworker.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-24 20:19:04 +0800
// Version: $Id$
// 

#ifndef _DATABASEWORKER_H_
#define _DATABASEWORKER_H_

#include <QtSql>

// The class that does all the work with the database. This class will
// be instantiated in the thread object's run() method.
class DatabaseWorker : public QObject
{
    Q_OBJECT;
public:
    explicit DatabaseWorker( QObject* parent = 0);
    virtual ~DatabaseWorker();

    bool connectDatabase();                             

public slots:
    void slotExecute( const QString& query, int reqno);
 
signals:
    void connected();
    void connect_error();
    // eval is lastInsertId
    void results( const QList<QSqlRecord>& records, int reqno, bool eret, 
                  const QString &estr, const QVariant &eval);

private:
    QSqlDatabase m_database;
};


#endif /* _DATABASEWORKER_H_ */
