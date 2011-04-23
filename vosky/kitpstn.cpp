// kitpstn.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-20 17:20:22 +0800
// Version: $Id: kitpstn.cpp 556 2010-11-05 10:10:04Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include "vosky.h"
#include "ui_vosky.h"

#include "metauri.h"
#include "skype.h"
#include "skypetunnel.h"
#include "skypetracer.h"

// call only once
void KitPhone::defaultPstnInit()
{
    this->mSkype = NULL;
    this->mtun = NULL;
    this->mSkypeTracer = NULL;

    QObject::connect(this->ui->checkBox_2, SIGNAL(stateChanged(int)),
                     this, SLOT(onInitPstnClient()));

    QObject::connect(this->ui->toolButton, SIGNAL(clicked()),
                     this, SLOT(onShowSkypeTracer()));
    QObject::connect(this->ui->pushButton_3, SIGNAL(clicked()),
                     this, SLOT(onConnectApp2App()));
    // QObject::connect(this->mainUI.pushButton_2, SIGNAL(clicked()),
    //                  this, SLOT(onSendPackage()));
    QObject::connect(this->ui->pushButton_4, SIGNAL(clicked()),
                     this, SLOT(onCallPstn()));
}

// maybe called twice or more
void KitPhone::onInitPstnClient()
{
    if (this->mtun != NULL) {
        delete this->mtun;
        this->mtun = NULL;
    }
    if (this->mSkype != NULL) {
        this->mSkype->disconnect();
        delete this->mSkype;
        this->mSkype = NULL;
    }
    this->mSkype = new Skype("karia2");
    // this->mSkype->connectToSkype();
    // QObject::connect(this->mSkype, SIGNAL(skypeError(int, QString)),
    //                  this, SLOT(onSkypeError(int, QString)));

    this->mtun = new SkypeTunnel(this);
    this->mtun->setSkype(this->mSkype);
}

void KitPhone::onShowSkypeTracer()
{
    if (this->mSkypeTracer == NULL) {
        this->mSkypeTracer = new SkypeTracer(this);
        QObject::connect(this->mSkype, SIGNAL(commandRequest(QString)),
                         this->mSkypeTracer, SLOT(onCommandRequest(QString)));
        QObject::connect(this->mSkype, SIGNAL(commandResponse(QString)),
                         this->mSkypeTracer, SLOT(onCommandResponse(QString)));
        QObject::connect(this->mSkypeTracer, SIGNAL(commandRequest(QString)),
                         this->mSkype, SLOT(onCommandRequest(QString)));
    }

    this->mSkypeTracer->setVisible(!this->mSkypeTracer->isVisible());
}

void KitPhone::onConnectApp2App()
{
    QString skypeName = this->ui->lineEdit->text();
    this->mtun->setStreamPeer(skypeName);

    QStringList contacts = this->mSkype->getContacts();
    qDebug()<<skypeName<<contacts;

    this->mSkype->newStream(skypeName);
    // this->mSkype->newStream("drswinghead");
}

void KitPhone::onCallPstn()
{
   QString num = this->ui->comboBox_3->currentText();
    
    SkypePackage sp;
    sp.seq = SkypeCommand::nextID().toInt();
    sp.type = SkypePackage::SPT_GW_SELECT;
    sp.data = num;

    QString spStr = sp.toString();
    qDebug()<<spStr;
    // bool ok = this->mSkype->sendPackage(this->ui->lineEdit->text(), spStr);
    // if (!ok) {
    //     qDebug()<<"maybe stream dosn't created, try create stream first";
    // }
    
    this->mtun->setPhoneNumber(num);
    // QString callee_name = QString("%1,%2").arg("liuguangzhao01").arg(num);
    QString callee_name = QString("%1").arg("liuguangzhao02");
    // this->mSkype->callFriend(callee_name);
    this->mtun->skypeCallFriend(callee_name, num);
}

void KitPhone::onHangupPstn()
{
    
}

