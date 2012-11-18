// groupinfodialog.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-28 15:30:00 +0800
// Version: $Id$
// 


#include "ui_groupinfodialog.h"
#include "groupinfodialog.h"

GroupInfoDialog::GroupInfoDialog(QWidget *parent)
    : QDialog(parent)
    , uiw(new Ui::GroupInfoDialog())
{
    this->uiw->setupUi(this);
}

GroupInfoDialog::~GroupInfoDialog()
{
}

QString GroupInfoDialog::groupName()
{
    QString group_name = this->uiw->comboBox->currentText();

    return group_name;
}

