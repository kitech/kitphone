// kitphone.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:46:42 +0800
// Version: $Id: kitphone.cpp 881 2011-05-17 14:59:51Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include "boost/smart_ptr.hpp"

#include "kitphone.h"
#include "ui_kitphone.h"

#include "aboutdialog.h"
#include "skypephone.h"
#include "sipphone.h"

#include "sipaccount.h"


KitPhone::KitPhone(QWidget *parent)
    : QMainWindow(parent),
      uiw(new Ui::KitPhone())
{
    this->uiw->setupUi(this);

    QLayout *lout = this->centralWidget()->layout();
    QVBoxLayout *vlout = static_cast<QVBoxLayout*>(lout);

    //////////////////
    QObject::connect(this->uiw->action_Exit, SIGNAL(triggered()),
                     this, SLOT(onQuitApp()));

    QObject::connect(this->uiw->action_About, SIGNAL(triggered()),
                     this, SLOT(onShowAbout()));


    this->uiw_skype = nullptr;
    this->uiw_sip = nullptr;

    //////////////////
    this->uiw_skype = new SkypePhone();
    vlout->addWidget(this->uiw_skype);
    this->uiw_skype->init_status_bar(this->statusBar());
    this->resize(this->uiw_skype->width(), this->uiw_skype->height());

    ///////////////////
    // this->uiw_sip = new SipPhone();
    // vlout->addWidget(this->uiw_sip);
    // this->resize(this->uiw_sip->width(), this->uiw_sip->height());
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

void KitPhone::onQuitApp()
{
    if (this->uiw_skype != nullptr) {
        // Application asked to unregister timer 0x1b00000b which is not registered in this thread. 
        // Fix application.
        // delete this->uiw_skype;
    }
    if (this->uiw_sip != nullptr) {
        // delete this->uiw_sip;
    }
    qApp->quit();
}

void KitPhone::onShowAbout()
{
    boost::scoped_ptr<AboutDialog> dlg(new AboutDialog(this));
    dlg->exec();
}


