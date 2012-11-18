// buddybox.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-12 12:42:50 +0800
// Version: $Id$
// 

#ifndef _BUDDYBOX_H_
#define _BUDDYBOX_H_

#include <QtCore>
#include <QtGui>

#include "main.h"

namespace Ui {
    class BuddyBox;
};

class BuddyBox : public QWidget
{
    Q_OBJECT;
public:
    explicit BuddyBox(QWidget *parent = 0);
    virtual ~BuddyBox();

    void setConversation(Conversation::Ref &conve);
    void setIdentity(QString identity);
    void setDisplayname(QString display_name);
    void setStatus(int status);
    void setSignText(QString sign_text);

    int availability() { return this->_availability; }
    void setAvailability(int availability);
 
private:
    QString identity;
    QString display_name;
    int status;
    QString sign_text;
    Conversation::Ref conv;
    int _availability;

private:
    Ui::BuddyBox *uiw;
};

#endif /* _BUDDYBOX_H_ */
