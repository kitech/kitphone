// phonecontactproperty.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-27 15:24:37 +0800
// Version: $Id$
// 

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
}

