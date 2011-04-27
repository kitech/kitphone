// phonecontact.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-27 13:48:41 +0800
// Version: $Id$
// 

#ifndef _PHONECONTACT_H_
#define _PHONECONTACT_H_

#include <QtCore>

class PhoneContact : public QObject
{
    Q_OBJECT;
public:
    explicit PhoneContact(QObject *parent = 0);
    virtual ~PhoneContact();

public:
    QString mDispName;
    QString mUserName;
    QString mPhoneNumber;
    int mGroupId;
    QString mGroupName;

private:
    
};

#endif /* _PHONECONTACT_H_ */
