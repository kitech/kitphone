// buddyitem.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-12 17:28:38 +0800
// Version: $Id$
// 
#ifndef _BUDDYITEM_H_
#define _BUDDYITEM_H_

#include <QtCore>
#include <QtGui>

#include "main.h"

class BuddyItem : public QListWidgetItem
{
public:
    BuddyItem ( QListWidget * parent = 0, int type = Type );
	BuddyItem ( const QString & text, QListWidget * parent = 0, int type = Type );
	BuddyItem ( const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type );
	BuddyItem ( const BuddyItem & other );
    virtual ~BuddyItem();

    void setIdentity(QString identity) {
        this->mIdentity = identity;
    }
    QString identity() {
        return this->mIdentity;
    }
public:
    Conversation::Ref conv;
    BuddyBox *bbox;
    QString mIdentity;;
};

#endif /* _BUDDYITEM_H_ */
