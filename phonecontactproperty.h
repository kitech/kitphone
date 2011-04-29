// phonecontactproperty.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-27 15:22:23 +0800
// Version: $Id$
// 

#ifndef _PHONECONTACTPROPERTY_H_
#define _PHONECONTACTPROPERTY_H_

#include <QtCore>
#include <QtGui>

#include "boost/smart_ptr.hpp"

class PhoneContact;

namespace Ui {
    class PhoneContactProperty;
};

class PhoneContactProperty : public QDialog
{
    Q_OBJECT;
public:
    explicit PhoneContactProperty(QWidget *parent = 0);
    virtual ~PhoneContactProperty();

    boost::shared_ptr<PhoneContact> contactInfo();
    bool setContactInfo(boost::shared_ptr<PhoneContact> ci);

private:
    Ui::PhoneContactProperty *uiw;
    int cid;
    bool modify_mode;
};

#endif /* _PHONECONTACTPROPERTY_H_ */
