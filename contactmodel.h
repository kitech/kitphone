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

class ContactModel : public QAbstractItemModel
{
    Q_OBJECT;
public:
    explicit ContactModel(QObject *parent = 0);
    virtual ~ContactModel();

private:
    
};

#endif /* _CONTACTMODEL_H_ */
