// aboutdialog.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-18 22:30:44 +0800
// Version: $Id$
// 


#include "ui_aboutdialog.h"
#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    ,uiw(new Ui::AboutDialog())
{
    this->uiw->setupUi(this);
    this->uiw->label_6->setText(KP_VERSION_STR);
}

AboutDialog::~AboutDialog()
{

}

