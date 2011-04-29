// kitphone.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:46:42 +0800
// Version: $Id: kitphone.cpp 849 2011-04-23 14:03:48Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include "kitphone.h"
#include "ui_kitphone.h"

#include "skypephone.h"

#include "sipaccount.h"


KitPhone::KitPhone(QWidget *parent)
    : QMainWindow(parent),
      uiw(new Ui::KitPhone())
{
    this->uiw->setupUi(this);

    this->uiw_skype = new SkypePhone();
    QLayout *lout = this->centralWidget()->layout();
    QVBoxLayout *vlout = static_cast<QVBoxLayout*>(lout);
    vlout->addWidget(this->uiw_skype);

    this->uiw_skype->init_status_bar(this->statusBar());
}

KitPhone::~KitPhone()
{
    delete uiw;
}

void KitPhone::paintEvent ( QPaintEvent * event )
{
    QMainWindow::paintEvent(event);
    // qDebug()<<"parintttttt"<<event<<event->type();
}
void KitPhone::showEvent ( QShowEvent * event )
{
    QMainWindow::showEvent(event);
    // qDebug()<<"showwwwwwwwwwwww"<<event<<event->type();
}

