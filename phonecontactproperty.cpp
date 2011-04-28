// phonecontactproperty.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-27 15:24:37 +0800
// Version: $Id$
// 

#include "phonecontact.h"

#include "ui_phonecontactproperty.h"
#include "phonecontactproperty.h"

PhoneContactProperty::PhoneContactProperty(QWidget *parent)
    : QDialog(parent)
    ,uiw(new Ui::PhoneContactProperty())
{
    this->uiw->setupUi(this);
}

PhoneContactProperty::~PhoneContactProperty()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
}

boost::shared_ptr<PhoneContact> PhoneContactProperty::contactInfo()
{
    boost::shared_ptr<PhoneContact> ci(new PhoneContact());

    ci->mUserName = ci->mDispName = this->uiw->comboBox->currentText();
    ci->mPhoneNumber = this->uiw->comboBox_4->currentText();
    ci->mGroupName = this->uiw->comboBox_3->currentText();

    return ci;
}
