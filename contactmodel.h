// contactmodel.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-28 16:47:26 +0800
// Version: $Id$
// 

#ifndef _CONTACTMODEL_H_
#define _CONTACTMODEL_H_

#include <QtCore>
#include <QtSql>

#include "boost/smart_ptr.hpp"
#include "boost/function.hpp"

class PhoneContact;
class AsyncDatabase;
class SqlRequest;

class ContactDataRetriver : public QObject
{
    Q_OBJECT;
public:
    explicit ContactDataRetriver(boost::shared_ptr<AsyncDatabase> adb);
    virtual ~ContactDataRetriver();

    QAtomicInt lazy_flag;
    bool getGroupList();
    bool getContactsByGroupId(int gid);
    bool getContactById(int cid);

public slots:
    void onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval);
    bool onGetAllGroupsDone(boost::shared_ptr<SqlRequest> req);
    bool onGetContactsByIdDone(boost::shared_ptr<SqlRequest> req);
    bool onGetModifiedContactDone(boost::shared_ptr<SqlRequest> req);

signals:
    void groupsRetrived(const QList<QSqlRecord> & results);
    void contactsRetrived(int gid, const QList<QSqlRecord> & results);
    void modifiedContactRetrived(const QList<QSqlRecord> & results);

private:
    boost::shared_ptr<AsyncDatabase> m_adb;
    //// sql reqno <---> sql reqclass
    QHash<int, boost::shared_ptr<SqlRequest> > mRequests;
};

class ContactInfoNode
{
public:
    explicit ContactInfoNode() {
        mrow = mcol = ntype = gid = lazy_flag = -1;
        pnode = NULL;
        pc = NULL;
    }
    ~ContactInfoNode() {};

    int mrow;
    int mcol;
    int ntype; // 0 is cat, 1 is contact
    int gid;
    QString group_name;
    int lazy_flag; // 0,1,2 

    QVector<ContactInfoNode*> childs;

    ContactInfoNode *pnode;
    // boost::shared_ptr<PhoneContact> pc;
    PhoneContact *pc;
};

class ContactModel : public QAbstractItemModel
{
    Q_OBJECT;
public:
    explicit ContactModel(boost::shared_ptr<AsyncDatabase> adb, QObject *parent = 0);
    virtual ~ContactModel();

    ////model 函数
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // Qt::ItemFlags flags(const QModelIndex &index) const;
    // QVariant headerData(int section, Qt::Orientation orientation,
    // int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
        
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    bool insertRows(int row, int count, const QModelIndex &parent);
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

public slots:
    void onGroupsRetrived(const QList<QSqlRecord> & results);
    void onContactsRetrived(int gid, const QList<QSqlRecord> & results);
    void onContactModified(int cid);
    void onModifiedContactRetrived(const QList<QSqlRecord> & results);

signals:
    void retriveGroups();
    void retriveContacts(int gid);

private:
    // friend class ContactDataRetriver;
    boost::shared_ptr<AsyncDatabase> m_adb;
    ContactDataRetriver *m_dretr;
    QVector<ContactInfoNode*> mContacts;
};

#endif /* _CONTACTMODEL_H_ */
