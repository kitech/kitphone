// kitphone.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:46:42 +0800
// Version: $Id: kitphone.cpp 995 2011-09-16 09:51:17Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include "boost/smart_ptr.hpp"

#include "ystatusbar.h"

#include "kitphone.h"
#include "ui_kitphone.h"

#include "aboutdialog.h"
#include "skypephone.h"
#include "mosipphone.h"

#include "sipaccount.h"


KitPhone::KitPhone(QWidget *parent)
    : QMainWindow(parent),
      uiw(new Ui::KitPhone())
{
    this->uiw->setupUi(this);

    //////////////////
    QObject::connect(this->uiw->action_Exit, SIGNAL(triggered()),
                     this, SLOT(onQuitApp()));
    QObject::connect(this->uiw->action_About, SIGNAL(triggered()),
                     this, SLOT(onShowAbout()));
    QObject::connect(this->uiw->actionSip_Phone_Mode, SIGNAL(triggered()),
                     this, SLOT(onSwitchPhoneMode()));
    QObject::connect(this->uiw->actionSkype_Phone_Mode, SIGNAL(triggered()),
                     this, SLOT(onSwitchPhoneMode()));
    QObject::connect(this->uiw->actionEmpty_Phone_Mode, SIGNAL(triggered()),
                     this, SLOT(onSwitchPhoneMode()));


    this->uiw_skype = nullptr;
    this->uiw_sip = nullptr;


    this->setWindowIcon(QIcon(":/skins/default/null_team-48.png"));
    this->custom_status_bar();

    this->onSwitchPhoneMode();
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

void KitPhone::onSwitchPhoneMode()
{
    QAction *action = static_cast<QAction*>(sender());

    /*
    //////////////////
    this->uiw_skype = new SkypePhone();
    vlout->addWidget(this->uiw_skype);
    this->uiw_skype->init_status_bar(this->statusBar());
    this->resize(this->uiw_skype->width(), this->uiw_skype->height());

    ///////////////////
    // this->uiw_sip = new SipPhone();
    // vlout->addWidget(this->uiw_sip);
    // this->resize(this->uiw_sip->width(), this->uiw_sip->height());
     */
    QLayout *lout = this->centralWidget()->layout();
    QVBoxLayout *vlout = static_cast<QVBoxLayout*>(lout);

    // 默认界面模式
    if (action == NULL) {
        if (0) {
            this->uiw_skype = new SkypePhone();
            vlout->addWidget(this->uiw_skype);
            this->uiw_skype->init_status_bar(this->statusBar());
            this->resize(this->uiw_skype->width(), this->uiw_skype->height());
            this->uiw->actionSkype_Phone_Mode->setChecked(true);
            this->statusBar()->showMessage(tr("Switch to Skype tunnel Phone Mode"));
            this->setWindowTitle(tr("Kitphone - Skype Tunnel Phone"));
        } else {
            this->uiw_sip = new MosipPhone(this);
            vlout->addWidget(this->uiw_sip);
            this->resize(this->uiw_sip->width(), this->uiw_sip->height());
            this->statusBar()->showMessage(tr("Switch to Sip Phone mode"));
            this->setWindowTitle(tr("Kitphone - Standard Sip Phone"));
        }
    } else if (action == this->uiw->actionSip_Phone_Mode) {
        if (this->uiw_sip == nullptr) {
            Q_ASSERT(this->uiw_skype != nullptr);
            vlout->removeWidget(this->uiw_skype);
            delete this->uiw_skype;
            this->uiw_skype = nullptr;
            this->uiw->actionSkype_Phone_Mode->setChecked(false);
            this->uiw->actionEmpty_Phone_Mode->setChecked(false);

            this->uiw_sip = new MosipPhone(this);
            vlout->addWidget(this->uiw_sip);
            this->resize(this->uiw_sip->width(), this->uiw_sip->height());
        } else {
            // no operation
        }
        this->statusBar()->showMessage(tr("Switch to Sip Phone mode"));
        this->setWindowTitle(tr("Kitphone - Standard Sip Phone"));
    } else if (action == this->uiw->actionSkype_Phone_Mode) {
        if (this->uiw_skype == nullptr) {
            Q_ASSERT(this->uiw_sip != nullptr);
            vlout->removeWidget(this->uiw_sip);
            delete this->uiw_sip;
            this->uiw_sip = nullptr;
            this->uiw->actionSip_Phone_Mode->setChecked(false);
            this->uiw->actionEmpty_Phone_Mode->setChecked(false);

            this->uiw_skype = new SkypePhone();
            vlout->addWidget(this->uiw_skype);
            this->uiw_skype->init_status_bar(this->statusBar());
            this->resize(this->uiw_skype->width(), this->uiw_skype->height());
        } else {
            // no operation
        }

        this->statusBar()->showMessage(tr("Switch to Skype tunnel Phone Mode"));
        this->setWindowTitle(tr("Kitphone - Skype Tunnel Phone"));
    } else if (action == this->uiw->actionEmpty_Phone_Mode) {
        if (this->uiw_sip != nullptr) {
            vlout->removeWidget(this->uiw_sip);
            delete this->uiw_sip;
            this->uiw_sip = nullptr;
            this->uiw->actionSip_Phone_Mode->setChecked(false);
        }

        if (this->uiw_skype != nullptr) {
            vlout->removeWidget(this->uiw_skype);
            delete this->uiw_skype;
            this->uiw_skype = nullptr;
            this->uiw->actionSkype_Phone_Mode->setChecked(false);
        }

        this->resize(this->width(), this->height() / 3);
        this->uiw->actionEmpty_Phone_Mode->setChecked(true);

        this->statusBar()->showMessage(tr("Switch to Clear mode"));
        this->setWindowTitle(tr("Kitphone Phone"));
    } else {
        Q_ASSERT(1 == 2);
    }
}

// |-status icon tool button-|-status text label-|
void KitPhone::custom_status_bar()
{
    this->esb = new YStatusBar();
    this->setStatusBar((this->esb));

    this->stb = new QToolButton();
    this->stb->setIcon(QIcon(":/skins/default/status_offline.png"));
    this->stb->setAutoRaise(true);
    this->stb->setPopupMode(QToolButton::InstantPopup);
    this->stb->setToolTip(tr("Online Status"));
    this->esb->addWidget(this->stb);

    this->mtb = new QToolButton();
    this->mtb->setIcon(QIcon(":/skins/default/null_team-32.png"));
    this->mtb->setIcon(QIcon(":/skype-kitphone.ico"));
    this->mtb->setIcon(QIcon(":/skins/default/chan_idle.png"));
    this->mtb->setToolTip(tr("Voice Engine Mode"));
    this->esb->addPermanentWidget(this->mtb);
    
    this->htb = new QToolButton();
    this->htb->setIcon(QIcon(":/skins/default/quest.png"));
    this->esb->addPermanentWidget(this->htb);

    // QStatusBar *sb = this->statusBar();
    // this->sb_status = new QToolButton();
    // this->sb_status->setText("aaaaaaaa");


    // QToolButton *rtb = new QToolButton();
    // rtb->setText("rrrrrrright");
    // sb->addPermanentWidget(rtb);

    // // 还要把这句放在所有的QStatusBar调用之后？？？
    // // ((YStatusBar*)sb)->insertLeftPermanentWidget(0, this->sb_status);
    // sb->addWidget(this->sb_status);

    // rtb = new QToolButton();
    // rtb->setText("r222222");
    // sb->addPermanentWidget(rtb);

    // this->sb_status = new QToolButton();
    // this->sb_status->setText("aaaa2222");
    // sb->addWidget(this->sb_status);

    // QLayout *ly = sb->layout();
    // QHBoxLayout *hly = static_cast<QHBoxLayout*>(ly);
    // qDebug()<<"status bar layout:"<<ly;

    // hly->insertWidget(0, this->sb_status);
    // qDebug()<<this->esb<<sb<< (this->esb == (QStatusBar*)sb);
}

