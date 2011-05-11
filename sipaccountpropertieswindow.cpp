// sipaccountpropertieswindow.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:35:35 +0800
// Version: $Id: sipaccountpropertieswindow.cpp 581 2010-11-21 15:03:14Z drswinghead $
// 

#include "simplelog.h"

#include "sipaccountpropertieswindow.h"
#include "ui_sipaccountpropertieswindow.h"

// SipAccountPropertiesWindow::SipAccountPropertiesWindow(QString uname, QWidget *parent)
//     : QDialog(parent),
//       uiw(new Ui::SipAccountPropertiesWindow())
// {
//     this->uiw->setupUi(this);

//     QObject::connect(this->uiw->pushButton, SIGNAL(clicked()),
//                      this, SLOT(reject()));
//     QObject::connect(this->uiw->pushButton_2, SIGNAL(clicked()),
//                      this, SLOT(accept()));
//     QObject::connect(this->uiw->pushButton_3, SIGNAL(clicked()),
//                      this, SLOT(accept()));

//     if (!uname.isEmpty()) {
//         this->setAccount(uname);
//     }
// }

SipAccountPropertiesWindow::SipAccountPropertiesWindow(SipAccount &acc, QWidget *parent)
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

    // if (!uname.isEmpty()) {
    //     this->setAccount(uname);
    // }

    this->macc = acc;
    if (this->macc.uid >= 0) {
        this->setAccount();
    }
}

SipAccountPropertiesWindow::~SipAccountPropertiesWindow()
{

}


SipAccount SipAccountPropertiesWindow::getAccount()
{
    SipAccount acc;

    acc = this->macc;

    acc.userName = this->uiw->lineEdit->text();
    acc.password = this->uiw->lineEdit_2->text();
    acc.domain = this->uiw->lineEdit_3->text();

    return acc;
}

bool SipAccountPropertiesWindow::setAccount()
{
    SipAccount acc;

    if (this->macc.uid < 0) {
        return false;
    }

    // acc = acc.getAccount(uname);
    acc = this->macc;

    if (!acc.userName.isEmpty()) {
        this->uiw->lineEdit->setText(acc.userName);
        this->uiw->lineEdit_2->setText(acc.password);
        this->uiw->lineEdit_3->setText(acc.domain);
    } else {
        qLogx()<<"can not find account: "<<acc.uid;
    }

    return true;
}

