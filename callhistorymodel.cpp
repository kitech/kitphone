// callhistorymodel.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-01 13:27:36 +0800
// Version: $Id$
// 

#include "asyncdatabase.h"
#include "simplelog.h"

#include "callhistorymodel.h"

CallHistoryModel::CallHistoryModel(boost::shared_ptr<AsyncDatabase> adb, QObject *parent)
    : QAbstractItemModel(parent)
    ,m_adb(adb)
{
    this->mroot = boost::shared_ptr<CallHistoryNode>(new CallHistoryNode());
    this->mroot->lazy_flag = 0;

    QObject::connect(this->m_adb.get(), SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)),
                     this, SLOT(onSqlExecuteDone(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)));

}

CallHistoryModel::~CallHistoryModel()
{
}

QVariant CallHistoryModel::data(const QModelIndex &index, int role) const
{
    boost::shared_ptr<CallHistoryNode> cnode;
    boost::shared_ptr<CallHistoryNode> pnode;

    QVariant v;

    if (role != Qt::DisplayRole) {
        return v;
    }

    if (!index.isValid()) {
        return v;
    }

    int row = index.row();
    cnode = this->mroot->childs[row];
    if (index.column() == 0) {
        v = cnode->mstatus;
    } else if (index.column() == 1) {
        v = cnode->mphone_number;
    } else if (index.column() == 2) {
        v = cnode->ctime;
    } else if (index.column() == 3) {
        v = cnode->mhid;
    } else {
        ////////////
    }


    return v;
}

// Qt::ItemFlags flags(const QModelIndex &index) const;
// QVariant headerData(int section, Qt::Orientation orientation,
// int role = Qt::DisplayRole) const;
QModelIndex CallHistoryModel::index(int row, int column,
                  const QModelIndex &parent) const
{
    // qLogx()<<row<<column<<parent;
    QModelIndex idx;
    boost::shared_ptr<CallHistoryNode> cnode;
    boost::shared_ptr<CallHistoryNode> pnode;

    if (!parent.isValid()) {
		if (row < this->mroot->childs.count()) {
        cnode = this->mroot->childs[row];
        idx = this->createIndex(row, column, cnode.get());
		} else {
		}
    } else {
        // should no model for this case
    }

    return idx;
}
        
QModelIndex CallHistoryModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int CallHistoryModel::rowCount(const QModelIndex &parent) const
{
    int cnt;

    if (!parent.isValid()) {
        cnt = this->mroot->childs.count();
    } else {
        cnt = 0;
    }

    return cnt;
}

int CallHistoryModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}


bool CallHistoryModel::hasChildren(const QModelIndex &parent) const
{
    // qLogx()<<parent;
    if (parent.isValid()) {
        return false;
    }
    return true;
}

void CallHistoryModel::fetchMore ( const QModelIndex & parent )
{
    // qLogx()<<parent;
    if (!parent.isValid()) {
        boost::shared_ptr<SqlRequest> req(new SqlRequest());
        // get groups list
        // req->mCbId = 0;
        req->mCbFunctor = boost::bind(&CallHistoryModel::onGetDbCallHistoryDone, this, _1);
        req->mCbObject = this;
        req->mCbSlot = SLOT(onGetDbCallHistoryDone(boost::shared_ptr<SqlRequest>));
        req->mSql = QString("SELECT * FROM kp_histories ORDER BY call_ctime DESC LIMIT 10000");
        req->mReqno = this->m_adb->execute(req->mSql);
        this->mRequests.insert(req->mReqno, req);

        // qDebug()<<req->mSql;
    } else {

    }
}

bool CallHistoryModel::canFetchMore ( const QModelIndex & parent ) const
{
    // qLogx()<<parent;
    if (!parent.isValid()) {
        if (this->mroot->lazy_flag == 0) {
            this->mroot->lazy_flag = 1;
            return true;
        }
    }
    return false;
}


bool CallHistoryModel::insertRows(int row, int count, const QModelIndex &parent)
{

    return false;
}

bool CallHistoryModel::removeRows(int row, int count, const QModelIndex & parent)
{
    return false;
}

////////////////
void CallHistoryModel::onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval)
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<reqno; 
    
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
        if (1) { //qrand() % 2 == 1) {
            cb_functor = req->mCbFunctor;
            bret = cb_functor(req);
        } else {
            cb_obj = req->mCbObject;
            cb_slot = req->mCbSlot;

            // qDebug()<<"qinvoke:"<<cb_obj<<cb_slot;
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

bool CallHistoryModel::onGetDbCallHistoryDone(boost::shared_ptr<SqlRequest> & req)
{
    QList<QSqlRecord> recs;

    recs = req->mResults;
    // qLogx()<<recs;

    this->onNewCallHistoryArrived(recs);

    this->mRequests.remove(req->mReqno);

    return true;
}

bool CallHistoryModel::onNewCallHistoryArrived(const QList<QSqlRecord> &results)
{
    QList<QSqlRecord> recs;
    QSqlRecord rec;

    recs = results;

    boost::shared_ptr<CallHistoryNode> cnode;
    boost::shared_ptr<CallHistoryNode> pnode;

    for (int i = 0; i < recs.count(); i ++) {
        cnode = boost::shared_ptr<CallHistoryNode>(new CallHistoryNode());
        cnode->mrow = i;
        cnode->lazy_flag = 2;
        
        rec = recs.at(i);
        cnode->mhid = rec.value("hid").toInt();
        cnode->mstatus = rec.value("call_status").toInt();
        cnode->mcontact_id = rec.value("contact_id").toInt();
        cnode->mphone_number = rec.value("phone_number").toString();
        cnode->ctime = rec.value("call_ctime").toString();

        this->beginInsertRows(QModelIndex(), i, 1);
        this->mroot->childs.append(cnode);
        this->endInsertRows();
    }

    return true;
}

bool CallHistoryModel::onCallHistoryRemoved(int hid)
{

    boost::shared_ptr<CallHistoryNode> pnode;    
    for (int i = 0; i < this->mroot->childs.count(); ++i) {
        pnode = this->mroot->childs.at(i);
        if (pnode->mhid == hid) {
            
            this->beginRemoveRows(QModelIndex(), i, 1);
            this->mroot->childs.remove(i);
            for (int j = i; j < this->mroot->childs.count(); ++j) {
                pnode = this->mroot->childs.at(j);
                pnode->mrow -= 1;
            }
            this->endRemoveRows();
            break;
        }
    }
    return true;
}

bool CallHistoryModel::onAllCallHistoryRemoved()
{
    this->beginRemoveRows(QModelIndex(), 0, this->mroot->childs.count());
    this->mroot->childs.clear();
    this->endRemoveRows();

    return true;
}

