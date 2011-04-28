// contactmodel.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-28 16:47:47 +0800
// Version: $Id$
// 

#include "phonecontact.h"
#include "contactmodel.h"

ContactModel::ContactModel(boost::shared_ptr<AsyncDatabase> adb, QObject *parent)
    : QAbstractItemModel(parent)
    ,m_adb(adb)
{
}

ContactModel::~ContactModel()
{

}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    QVariant v;

    return v;
}

// Qt::ItemFlags flags(const QModelIndex &index) const;
// QVariant headerData(int section, Qt::Orientation orientation,
// int role = Qt::DisplayRole) const

QModelIndex ContactModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex midx;
    QModelIndex idx;
    int gid = 0;

    if (!parent.isValid()) {
        gid = this->mGroups.at(row)->gid;
        idx = this->createIndex(row, column, &gid);
        // qDebug()<<__FUNCTION__<<__LINE__<<"row :"<<row<<" column:" <<column<<parent<<idx
        //         <<child_item->fileName()<<child_item
        //         <<"0";
        Q_ASSERT(!(idx.row() == -1 && idx.column() == 0));
        return idx;
    } else {
        idx = createIndex(row, column, 0);
        // qDebug()<<__FUNCTION__<<__LINE__<<"row :"<<row<<" column:" <<column<<parent<<idx
        //         <<child_item->fileName()
        //         <<parent_item->fileName()
        //         <<child_item;
        Q_ASSERT(!(idx.row() == -1 && idx.column() == 0));
        return idx;
    }

    qDebug()<<__FUNCTION__<<__LINE__<<"invalid QModelIndex returnted";
    return QModelIndex();

    return midx;
}

QModelIndex ContactModel::parent(const QModelIndex &child) const
{
    QModelIndex midx;

    if (!child.isValid()) {
        return midx;
    } else {
        if (child.internalPointer() == 0) {
            
        }
    }

    return midx;
}

int ContactModel::rowCount(const QModelIndex &parent) const
{
    int cnt = 0;
    int row;

    if (!parent.isValid()) {
        cnt = this->mGroups.count();
    } else {
        row = parent.row();
        
    }

    return cnt;
}

int ContactModel::columnCount(const QModelIndex &parent) const
{
    int cnt = 0;

    if (!parent.isValid()) {
        cnt = 2;
    } else {
        cnt = 4;
    }

    return cnt;
}

bool ContactModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    } else {
        return true;
    }
}
