// sipaccountpropertieswindow.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:35:35 +0800
// Version: $Id: sipaccountpropertieswindow.cpp 581 2010-11-21 15:03:14Z drswinghead $
// 

#include "sipaccountpropertieswindow.h"
#include "ui_sipaccountpropertieswindow.h"

SipAccountPropertiesWindow::SipAccountPropertiesWindow(QString uname, QWidget *parent)
    : QDialog(parent),
      uiw(new Ui::SipAccountPropertiesWindow())
{
    this->uiw->setupUi(this);

    QObject::connect(this->uiw->pushButton, SIGNAL(clicked()),
                     this, SLOT(reject()));
    QObject::connect(this->uiw->pushButton_2, SIGNAL(clicked()),
                     this, SLOT(accept()));
    QObject::connect(this->uiw->pushButton_3, SIGNAL(clicked()),
                     this, SLOT(accept()));

    if (!uname.isEmpty()) {
        this->setAccount(uname);
    }
}

SipAccountPropertiesWindow::~SipAccountPropertiesWindow()
{

}


SipAccount SipAccountPropertiesWindow::getAccount()
{
    SipAccount acc;

    acc.userName = this->uiw->lineEdit->text();
    acc.password = this->uiw->lineEdit_2->text();
    acc.domain = this->uiw->lineEdit_3->text();

    return acc;
}

bool SipAccountPropertiesWindow::setAccount(QString uname)
{
    SipAccount acc;

    if (uname.isEmpty()) {
        return false;
    }

    acc = acc.getAccount(uname);

    if (!acc.userName.isEmpty()) {
        this->uiw->lineEdit->setText(acc.userName);
        this->uiw->lineEdit_2->setText(acc.password);
        this->uiw->lineEdit_3->setText(acc.domain);
    } else {
        qDebug()<<"can not find account: "<<uname;
    }

    return true;
}

