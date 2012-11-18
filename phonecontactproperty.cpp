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

    this->cid = 0;
    this->modify_mode = false;
}

PhoneContactProperty::~PhoneContactProperty()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
}

PhoneContact* PhoneContactProperty::contactInfo()
{
    // boost::shared_ptr<PhoneContact> ci(new PhoneContact());
    PhoneContact *ci = NULL;

    ci = new PhoneContact();
    ci->mUserName = ci->mDispName = this->uiw->comboBox->currentText();
    ci->mPhoneNumber = this->uiw->comboBox_4->currentText();
    ci->mGroupName = this->uiw->comboBox_3->currentText();

    if (this->modify_mode) {
        ci->mContactId = this->cid;
    }

    return ci;
}

// bool PhoneContactProperty::setContactInfo(boost::shared_ptr<PhoneContact> ci)
bool PhoneContactProperty::setContactInfo(PhoneContact * ci)
{
    this->uiw->comboBox->setEditText(ci->mDispName);
    this->uiw->comboBox_4->setEditText(ci->mPhoneNumber);
    // this->uiw->comboBox_3->setEditText(ci->mGroupName);
    int index = this->uiw->comboBox_3->findText(ci->mGroupName);

    qDebug()<<ci->mGroupName<<index;
    if (index == -1) {
        Q_ASSERT(index != -1);
    } else {
        this->uiw->comboBox_3->setCurrentIndex(index);
    }

    this->modify_mode = true;
    this->cid = ci->mContactId;

    return true;
}

