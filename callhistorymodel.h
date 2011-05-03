// callhistorymodel.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-01 13:27:25 +0800
// Version: $Id$
// 

#ifndef _CALLHISTORYMODEL_H_
#define _CALLHISTORYMODEL_H_

#include <QtCore>
#include <QtSql>

#include "boost/smart_ptr.hpp"
#include "boost/smart_ptr/enable_shared_from_this.hpp"

class AsyncDatabase;
class SqlRequest;

class CallHistoryNode : public boost::enable_shared_from_this<CallHistoryNode>
{
public:
    CallHistoryNode() {
        mrow = mcol = lazy_flag = mstatus = -1;
    }
    ~CallHistoryNode() {

    }

    int mrow;
    int mcol;
    int lazy_flag;

    int mstatus;
    int mcontact_id;
    QString mphone_number;
    QString ctime;
    QString mtime;
    
    QVector<boost::shared_ptr<CallHistoryNode> > childs;
};

class CallHistoryModel : public QAbstractItemModel
{
    Q_OBJECT;
public:
    explicit CallHistoryModel(boost::shared_ptr<AsyncDatabase> adb, QObject *parent = 0);
    virtual ~CallHistoryModel();

    ////model 函数
    QVariant data(const QModelIndex &index, int role) const;
    // Qt::ItemFlags flags(const QModelIndex &index) const;
    // QVariant headerData(int section, Qt::Orientation orientation,
    // int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
        
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    void fetchMore ( const QModelIndex & parent );
    bool canFetchMore ( const QModelIndex & parent ) const;

    bool insertRows(int row, int count, const QModelIndex &parent);
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

public slots:
    void onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval);
    bool onGetDbCallHistoryDone(boost::shared_ptr<SqlRequest> & req);

private:
    boost::shared_ptr<CallHistoryNode> mroot;

    boost::shared_ptr<AsyncDatabase> m_adb;
    //// sql reqno <---> sql reqclass
    QHash<int, boost::shared_ptr<SqlRequest> > mRequests;
};

#endif /* _CALLHISTORYMODEL_H_ */
