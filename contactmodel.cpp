// contactmodel.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-28 16:47:47 +0800
// Version: $Id$
// 

#include "asyncdatabase.h"

#include "phonecontact.h"
#include "contactmodel.h"

ContactModel::ContactModel(boost::shared_ptr<AsyncDatabase> adb, QObject *parent)
    : QAbstractItemModel(parent)
    ,m_adb(adb)
{
    this->m_dretr = new ContactDataRetriver(adb);
    QObject::connect(this->m_dretr, SIGNAL(groupsRetrived(const QList<QSqlRecord> &)),
                     this, SLOT(onGroupsRetrived(const QList<QSqlRecord> &)));
    QObject::connect(this->m_dretr, SIGNAL(contactsRetrived(int, const QList<QSqlRecord> &)),
                     this, SLOT(onContactsRetrived(int, const QList<QSqlRecord> &)));
    QObject::connect(this->m_dretr, SIGNAL(modifiedContactRetrived(const QList<QSqlRecord> &)),
                     this, SLOT(onModifiedContactRetrived(const QList<QSqlRecord> &)));
}

ContactModel::~ContactModel()
{
    delete this->m_dretr;
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<index<<role;    
    QVariant v;
    ContactInfoNode *pnode = NULL;
    ContactInfoNode *cnode = NULL;

    if (role != Qt::DisplayRole) {
        return v;
    }

    if (!index.isValid()) {       
    } else {
        cnode = static_cast<ContactInfoNode*>(index.internalPointer());
        if (cnode->ntype != 0) {
            // contact leaf
            if (index.column() == 0) {
                v = cnode->pc->mDispName;
            } else if (index.column() == 1) {
                v = cnode->pc->mPhoneNumber;
            } else if (index.column() == 2) {
                v = cnode->pc->mContactId;
            }
        } else {
            // group name
            cnode = this->mContacts.at(index.row());
            if (index.column() == 0) {
                v = cnode->group_name;
            } else {
                v = cnode->gid;
            }
        }
    }
    // qDebug()<<v<<cnode;
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<index<<role<<v;

    return v;
}

// Qt::ItemFlags flags(const QModelIndex &index) const;
// QVariant headerData(int section, Qt::Orientation orientation,
// int role = Qt::DisplayRole) const

QModelIndex ContactModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex midx;
    QModelIndex idx;
    ContactInfoNode *pnode = NULL;
    ContactInfoNode *cnode = NULL;

    if (!parent.isValid()) {
        // group node
        pnode = this->mContacts.at(row);
        idx = this->createIndex(row, column, pnode);
        // qDebug()<<__FUNCTION__<<__LINE__<<"row :"<<row<<" column:" <<column<<parent<<idx
        //         <<child_item->fileName()<<child_item
        //         <<"0";
        Q_ASSERT(!(idx.row() == -1 && idx.column() == 0));
        return idx;
    } else {
        pnode = static_cast<ContactInfoNode*>(parent.internalPointer());
        // qDebug()<<parent<<pnode;
        cnode = pnode->childs.at(row);

        idx = createIndex(row, column, cnode);
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
    ContactInfoNode *pnode = NULL;
    ContactInfoNode *cnode = NULL;

    if (!child.isValid()) {
        return midx;
    } else {
        cnode = static_cast<ContactInfoNode*>(child.internalPointer());
        if (cnode->ntype == 0) {
            return midx;
        } else {
            pnode = cnode->pnode;
            midx = this->createIndex(pnode->mrow, 0, pnode);
            return midx;
        }
    }

    return midx;
}

int ContactModel::rowCount(const QModelIndex &parent) const
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<parent;
    int cnt = 0;
    int row;
    int gid;
    ContactInfoNode *pnode = NULL;
    ContactInfoNode *cnode = NULL;


    if (!parent.isValid()) {
        cnt = this->mContacts.count();
        if (cnt == 0 && this->m_dretr->lazy_flag == 0) {
            this->m_dretr->lazy_flag = 1;
            this->m_dretr->getGroupList();
        }
    } else {
        row = parent.row();
        pnode = static_cast<ContactInfoNode*>(parent.internalPointer());
        if (pnode->ntype == 0) {
            // pnode = this->mContacts.at(row);
            cnt = pnode->childs.count();

            //////////
            if (cnt == 0 && pnode->lazy_flag == 0) {
                pnode->lazy_flag = 1;
                this->m_dretr->getContactsByGroupId(pnode->gid);
            }
        } else {
            cnt = 0;
        }
    }
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<parent<<cnt;
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

    cnt = 3;
    return cnt;
}

bool ContactModel::hasChildren(const QModelIndex &parent) const
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<parent;
    ContactInfoNode *pnode = NULL;
    ContactInfoNode *cnode = NULL;

    if (parent.isValid()) {
        pnode = static_cast<ContactInfoNode*>(parent.internalPointer());
        if (pnode->ntype == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

void ContactModel::onGroupsRetrived(const QList<QSqlRecord> & results)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QSqlRecord rec;
    int gid;
    QString gname;
    ContactInfoNode *cin = NULL;
    QModelIndex idx;

    // qDebug()<<results;

    this->m_dretr->lazy_flag = 2;
    for (int i = 0; i < results.count(); i++) {
        rec = results.at(i);

        gid = rec.value("gid").toInt();
        gname = rec.value("group_name").toString();

        cin = new ContactInfoNode();
        cin->ntype = 0;
        cin->gid = gid;
        cin->group_name = gname;
        cin->pnode = NULL;
        cin->mrow = this->mContacts.count();
        cin->lazy_flag = 0;

        this->beginInsertRows(idx,  this->mContacts.count(), this->mContacts.count());
        this->mContacts.append(cin);
        this->endInsertRows();
    }

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<this->mContacts.count();    
}

void ContactModel::onContactsRetrived(int gid, const QList<QSqlRecord> & results)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<gid<<results.count();

    QSqlRecord rec;
    QString gname;
    ContactInfoNode *cin = NULL;
    ContactInfoNode *nin = NULL;
    QModelIndex idx;

    for (int i = this->mContacts.count()-1; i >= 0; i--) {
        cin = this->mContacts.at(i);
        if (cin->gid == gid) {

            for (int j = 0; j < results.count(); j++) {
                rec = results.at(j);
                nin = new ContactInfoNode();
                nin->gid = gid;
                nin->pnode = cin;
                nin->pc = boost::shared_ptr<PhoneContact>(new PhoneContact());
                nin->pc->mContactId = rec.value("cid").toInt();
                nin->pc->mGroupName = rec.value("group_name").toString();
                nin->pc->mUserName = nin->pc->mDispName = rec.value("display_name").toString();
                nin->pc->mPhoneNumber = rec.value("phone_number").toString();
                nin->mrow = cin->childs.count();

                idx = this->index(i, 0, QModelIndex());
                this->beginInsertRows(idx, cin->childs.count(), cin->childs.count());
                cin->childs.append(nin);
                this->endInsertRows();
            }

            break;
        }
    }
}


void ContactModel::onContactModified(int cid)
{
    this->m_dretr->getContactById(cid);
}

void ContactModel::onModifiedContactRetrived(const QList<QSqlRecord> & results)
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<results;

    QSqlRecord rec = results.at(0);
    int cid = rec.value("cid").toInt();
    int gid = rec.value("group_id").toInt();

    ContactInfoNode *pnode = NULL;
    ContactInfoNode *cnode = NULL;
    ContactInfoNode *tnode = NULL;
    int rmrow = -1;
    QModelIndex idx;

    // 如果组ID变了，让view的失效范围更大，否则，只需要失效一条记录

    for (int i = this->mContacts.count() -1; i >= 0; i--) {
        tnode = this->mContacts.at(i);
        for (int j = tnode->childs.count()-1; j >= 0; j--) {
            cnode = tnode->childs.at(j);
            if (cnode->pc->mContactId == cid) {
                // found
                rmrow = j;
                break;
            }
            cnode = NULL;
        }
        tnode = NULL;
    }
    // qDebug()<<cnode<<rmrow;
    if (cnode != NULL) {
        if (cnode->pc->mGroupId == gid) {
            // set data
        } else {
            // remove and add
        }
        rmrow = cnode->mrow;
        pnode = cnode->pnode;
        idx = this->index(pnode->mrow, 0, QModelIndex());
        this->beginRemoveRows(idx, cnode->mrow, cnode->mrow);
        pnode->childs.remove(rmrow);
        for (int i = rmrow; i < pnode->childs.count(); i++) {
            tnode = pnode->childs.at(i);
            tnode->mrow--;
        }
        this->endRemoveRows();
    }

    this->onContactsRetrived(gid, results);
}

///////////////////////////
ContactDataRetriver::ContactDataRetriver(boost::shared_ptr<AsyncDatabase> adb)
    : m_adb(adb)
{
    this->lazy_flag = 0;
    QObject::connect(this->m_adb.get(), SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)),
                     this, SLOT(onSqlExecuteDone(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)));

}

ContactDataRetriver::~ContactDataRetriver()
{

}

////////////////
void ContactDataRetriver::onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<reqno; 
    
    QObject *cb_obj = NULL;
    const char *cb_slot = NULL;
    boost::function<bool(boost::shared_ptr<SqlRequest>)> cb_functor;
    boost::shared_ptr<SqlRequest> req;
    bool bret = false;
    // QGenericReturnArgument qret;
    // QGenericArgument qarg;
    bool qret;
    QMetaMethod qmethod;
    char raw_method_name[32] = {0};

    if (this->mRequests.contains(reqno)) {
        req = this->mRequests[reqno];
        req->mRet = eret;
        req->mErrorString = estr;
        req->mExtraValue = eval;
        req->mResults = results;

        // 实现方法太多，还要随机使用一种方法，找麻烦
        if (qrand() % 2 == 1) {
            cb_functor = req->mCbFunctor;
            bret = cb_functor(req);
        } else {
            cb_obj = req->mCbObject;
            cb_slot = req->mCbSlot;

            qDebug()<<"qinvoke:"<<cb_obj<<cb_slot;
            // get method name from SLOT() signature: 1onAddContactDone(boost::shared_ptr<SqlRequest>)
            for (int i = 0, j = 0; i < strlen(cb_slot); ++i) {
                if (cb_slot[i] >= '0' && cb_slot[i] <= '9') {
                    continue;
                }
                if (cb_slot[i] == '(') break;
                Q_ASSERT(j < sizeof(raw_method_name));
                raw_method_name[j++] = cb_slot[i];
            }
            Q_ASSERT(strlen(raw_method_name) > 0);
            Q_ASSERT(cb_obj->metaObject()->indexOfSlot(raw_method_name) != -1);
            bret = QMetaObject::invokeMethod(cb_obj, raw_method_name,
                                             Q_RETURN_ARG(bool, qret),
                                             Q_ARG(boost::shared_ptr<SqlRequest>, req));
            // qmethod = cb_obj->metaObject()->method(cb_obj->metaObject()->indexOfSlot(SLOT(onAddContactDone(boost::shared_ptr<SqlRequest>))));
            // bret = qmethod.invoke(cb_obj, Q_RETURN_ARG(bool, qret),
            //                        Q_ARG(boost::shared_ptr<SqlRequest>, req));
            // qDebug()<<cb_obj->metaObject()->indexOfSlot(cb_slot);
        }
    }
}

bool ContactDataRetriver::onGetAllGroupsDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;

    this->mRequests.remove(req->mReqno);

    emit this->groupsRetrived(req->mResults);

    return true;
}

bool ContactDataRetriver::onGetContactsByIdDone(boost::shared_ptr<SqlRequest> req)
{

    this->mRequests.remove(req->mReqno);

    emit this->contactsRetrived(req->mCbId, req->mResults);
    
    return true;
}

bool ContactDataRetriver::getGroupList()
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());
    // get groups list
    req->mCbFunctor = boost::bind(&ContactDataRetriver::onGetAllGroupsDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onGetAllGroupsDone(boost::shared_ptr<SqlRequest>));
    req->mSql = QString("SELECT * FROM kp_groups");
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);

    return true;
}

bool ContactDataRetriver::getContactsByGroupId(int gid)
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());
    // get groups list
    req->mCbId = gid;
    req->mCbFunctor = boost::bind(&ContactDataRetriver::onGetContactsByIdDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onGetContactsByIdDone(boost::shared_ptr<SqlRequest>));
    req->mSql = QString("SELECT * FROM kp_contacts,kp_groups WHERE group_id=%1 AND kp_contacts.group_id=kp_groups.gid").arg(gid);
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);

    qDebug()<<req->mSql;

    return true;
}

bool ContactDataRetriver::getContactById(int cid)
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());
    // get groups list
    req->mCbId = cid;
    req->mCbFunctor = boost::bind(&ContactDataRetriver::onGetModifiedContactDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onGetModifiedContactDone(boost::shared_ptr<SqlRequest>));
    req->mSql = QString("SELECT * FROM kp_contacts,kp_groups WHERE cid=%1 AND kp_contacts.group_id=kp_groups.gid").arg(cid);
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);

    qDebug()<<req->mSql;

    return true;
}

bool ContactDataRetriver::onGetModifiedContactDone(boost::shared_ptr<SqlRequest> req)
{

    emit this->modifiedContactRetrived(req->mResults);
    return true;
}

