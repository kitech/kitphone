// buddyitem.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-12 17:28:50 +0800
// Version: $Id$
// 

#include "utils.h"

#include "buddybox.h"
#include "buddyitem.h"

BuddyItem::BuddyItem ( QListWidget * parent, int type)
    : QListWidgetItem(parent, type)
{
    this->conv = CConversation::Ref(0);
    this->bbox = NULL;
}

BuddyItem::BuddyItem ( const QString & text, QListWidget * parent, int type)
    : QListWidgetItem(text, parent, type)
{
    this->conv = CConversation::Ref(0);
    this->bbox = NULL;
}

BuddyItem::BuddyItem ( const QIcon & icon, const QString & text, QListWidget * parent, int type)
    : QListWidgetItem(icon, text, parent, type)
{
    this->conv = CConversation::Ref(0);
    this->bbox = NULL;
}

BuddyItem::BuddyItem ( const BuddyItem & other )
    : QListWidgetItem(other)
{
    this->conv = CConversation::Ref(0);
    this->bbox = NULL;
}

BuddyItem::~BuddyItem()
{
    q_debug()<<"";
}

