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

#include "boost/smart_ptr.hpp"

class PhoneContact;
class AsyncDatabase;


struct ContactGroupNode;
struct ContactInfoNode
{
    boost::shared_ptr<PhoneContact> pc;
    ContactGroupNode *gn;
};

struct ContactGroupNode
{
    int gid;
    QString group_name;
    int lazy_flag;

    QVector<boost::shared_ptr<ContactInfoNode> > childs;
};

class ContactModel : public QAbstractItemModel
{
    Q_OBJECT;
public:
    explicit ContactModel(boost::shared_ptr<AsyncDatabase> adb, QObject *parent = 0);
    virtual ~ContactModel();

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

public slots:
    
private:
    boost::shared_ptr<AsyncDatabase> m_adb;
    QVector<boost::shared_ptr<ContactGroupNode> > mGroups;

    QHash<int, QVector<boost::shared_ptr<PhoneContact> > > mContacts;
};

#endif /* _CONTACTMODEL_H_ */
