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

#include "boost/smart_ptr.hpp"

class CallHistoryNode 
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
    QString mphone_number;
    QString mdate;
    
    QVector<boost::shared_ptr<CallHistoryNode> > childs;
};

class CallHistoryModel : public QAbstractItemModel
{
    Q_OBJECT;
public:
    explicit CallHistoryModel(QObject *parent = 0);
    virtual ~CallHistoryModel();

    
private:
    CallHistoryNode *mroot;
};

#endif /* _CALLHISTORYMODEL_H_ */
