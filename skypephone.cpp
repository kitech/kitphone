// kitpstn.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-20 17:20:22 +0800
// Version: $Id: skypephone.cpp 850 2011-04-24 15:07:53Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include "ui_skypephone.h"
#include "skypephone.h"

#include "metauri.h"
#include "skype.h"
#include "skypetunnel.h"
#include "skypetracer.h"

#include "websocketclient.h"
#include "asyncdatabase.h"

SkypePhone::SkypePhone(QWidget *parent)
    :QWidget(parent),
     uiw(new Ui::SkypePhone())
{
    this->uiw->setupUi(this);

    this->defaultPstnInit();
    this->m_adb = new AsyncDatabase();
    this->m_adb->start();
}

SkypePhone::~SkypePhone()
{
    this->m_adb->quit();
    this->m_adb->wait();
    delete this->m_adb;

    delete uiw;
}

void SkypePhone::paintEvent ( QPaintEvent * event )
{
    QWidget::paintEvent(event);
    // qDebug()<<"parintttttt"<<event<<event->type();
    if (this->first_paint_event) {
        this->first_paint_event = false;
		// QTimer::singleShot(50, this, SLOT(main_ui_draw_complete()));
    }
}
void SkypePhone::showEvent ( QShowEvent * event )
{
    QWidget::showEvent(event);
    // qDebug()<<"showwwwwwwwwwwww"<<event<<event->type();
}

// call only once
void SkypePhone::defaultPstnInit()
{
    this->mSkype = NULL;
    this->mtun = NULL;
    this->mSkypeTracer = NULL;

    // QObject::connect(this->uiw->checkBox_2, SIGNAL(stateChanged(int)),
    //                  this, SLOT(onInitPstnClient()));

    QObject::connect(this->uiw->pushButton, SIGNAL(clicked()),
                     this, SLOT(onShowSkypeTracer()));
    QObject::connect(this->uiw->pushButton_3, SIGNAL(clicked()),
                     this, SLOT(onConnectSkype()));
    // QObject::connect(this->mainUI.pushButton_2, SIGNAL(clicked()),
    //                  this, SLOT(onSendPackage()));
    QObject::connect(this->uiw->pushButton_4, SIGNAL(clicked()),
                     this, SLOT(onCallPstn()));
    QObject::connect(this->uiw->toolButton_10, SIGNAL(clicked()),
                     this, SLOT(onHangupPstn()));

    QObject::connect(this->uiw->toolButton_6, SIGNAL(clicked()),
                     this, SLOT(onAddContact()));
}

// maybe called twice or more
void SkypePhone::onInitPstnClient()
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
    this->mSkype->setRunAsClient();
    // this->mSkype->connectToSkype();
    // QObject::connect(this->mSkype, SIGNAL(skypeError(int, QString)),
    //                  this, SLOT(onSkypeError(int, QString)));

    QObject::connect(this->mSkype, SIGNAL(connected(QString)), this, SLOT(onSkypeConnected(QString)));
    QObject::connect(this->mSkype, SIGNAL(realConnected(QString)),
                     this, SLOT(onSkypeRealConnected(QString)));

    QObject::connect(this->mSkype, SIGNAL(UserStatus(QString,int)),
                     this, SLOT(onSkypeUserStatus(QString,int)));

    QObject::connect(this->mSkype, SIGNAL(newCallArrived(QString,QString,int)),
                     this, SLOT(onSkypeCallArrived(QString,QString,int)));

    this->mtun = new SkypeTunnel(this);
    this->mtun->setSkype(this->mSkype);
}

void SkypePhone::onShowSkypeTracer()
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

void SkypePhone::onConnectSkype()
{
    this->uiw->pushButton_3->setEnabled(false);
    this->onInitPstnClient();

    qDebug()<<"Skype name:"<<this->mSkype->handlerName();
}

void SkypePhone::onConnectApp2App()
{
    // QString skypeName = this->uiw->lineEdit->text();
    // this->mtun->setStreamPeer(skypeName);

    // QStringList contacts = this->mSkype->getContacts();
    // qDebug()<<skypeName<<contacts;

    // this->mSkype->newStream(skypeName);
    // this->mSkype->newStream("drswinghead");
}

void SkypePhone::onSkypeConnected(QString user_name)
{
    qDebug()<<"Waiting handler name:"<<user_name;
}

void SkypePhone::onSkypeRealConnected(QString user_name)
{
    qDebug()<<"Got handler name:"<<user_name;
    this->uiw->label_3->setText(user_name);
    this->uiw->pushButton_4->setEnabled(true);
}

void SkypePhone::onSkypeUserStatus(QString str_status, int int_status)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<str_status<<int_status;
    QPixmap icon = QPixmap(":/skins/default/status_offline.png");
    switch (int_status) {
    case SS_ONLINE:
    case SS_INVISIBLE:
    case SS_SKYPEME:
        icon = QPixmap(":/skins/default/status_online.png");
        break;
    case SS_AWAY:
        icon = QPixmap(":/skins/default/status_away.png");
        break;
    case SS_DND:
        icon = QPixmap(":/skins/default/status_dnd.png");
        break;
    case SS_NA:
        icon = QPixmap(":/skins/default/status_busy.png");
        break;
    case SS_OFFLINE:
        break;
    default:
        break;
    }

    this->uiw->label_14->setPixmap(icon);
    this->uiw->label_14->setToolTip(str_status);
}

void SkypePhone::onSkypeCallArrived(QString callerName, QString calleeName, int callID)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<callerName<<calleeName<<callID;
    this->m_curr_skype_call_id = callID;
    this->m_curr_skype_call_peer = calleeName;
}

void SkypePhone::onCallPstn()
{

    // 检测号码有效性
    
    // 设置呼叫状态
    // this->uiw->pushButton_4->setEnabled(false);
    this->uiw->label_5->setText(this->uiw->comboBox_3->currentText());

    QString wsuri = "ws://202.108.12.212:80/" + this->mSkype->handlerName() + "/";
    this->wscli = boost::shared_ptr<WebSocketClient>(new WebSocketClient(wsuri));
    QObject::connect(this->wscli.get(), SIGNAL(onConnected(QString)), this, SLOT(onWSConnected(QString)));
    QObject::connect(this->wscli.get(), SIGNAL(onDisconnected(QString)), this, SLOT(onWSDisconnected(QString)));
    QObject::connect(this->wscli.get(), SIGNAL(onWSMessage(QByteArray)), this, SLOT(onWSMessage(QByteArray)));
    bool ok = this->wscli->connectToServer();
    Q_ASSERT(ok);

   // QString num = this->uiw->comboBox_3->currentText();
    
   //  SkypePackage sp;
   //  sp.seq = SkypeCommand::nextID().toInt();
   //  sp.type = SkypePackage::SPT_GW_SELECT;
   //  sp.data = num;

   //  QString spStr = sp.toString();
   //  qDebug()<<spStr;
   //  // bool ok = this->mSkype->sendPackage(this->uiw->lineEdit->text(), spStr);
   //  // if (!ok) {
   //  //     qDebug()<<"maybe stream dosn't created, try create stream first";
   //  // }
    
   //  this->mtun->setPhoneNumber(num);
   //  // QString callee_name = QString("%1,%2").arg("liuguangzhao01").arg(num);
   //  QString callee_name = QString("%1").arg("liuguangzhao01");
   //  this->mSkype->callFriend(callee_name);
}

void SkypePhone::onHangupPstn()
{
    this->mSkype->setCallHangup(QString::number(this->m_curr_skype_call_id));
}

void SkypePhone::onAddContact()
{
    
}

void SkypePhone::onWSConnected(QString path)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<path;

    // 
    QString num = this->uiw->comboBox_3->currentText();
    QString cmd = QString("101$%1$%2").arg(this->mSkype->handlerName()).arg(num);
    this->wscli->sendMessage(cmd.toAscii());
}

void SkypePhone::onWSError()
{

}

void SkypePhone::onWSDisconnected()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
}

void SkypePhone::onWSMessage(QByteArray msg)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<msg;
    
    QString router_name;
    QStringList tmps;
    QStringList fields = QString(msg).split("$");
    QString log_msg;
    QListWidgetItem *witem = nullptr;

    QTextCodec *u8codec = QTextCodec::codecForName("UTF-8");
    QString log_time = QDateTime::currentDateTime().toString("hh:mm:ss");
    
    switch (fields.at(0).toInt()) {
    case 102:
        Q_ASSERT(fields.at(1) == this->mSkype->handlerName());
        tmps = fields.at(2).split("\n");
        if (tmps.at(0).trimmed() == "200 OK") {
            Q_ASSERT(tmps.at(1).trimmed().toInt() >= 1);
            router_name = tmps.at(2).trimmed();
            
            this->mSkype->callFriend(router_name);
        }
        break;
    case 108:
        log_msg = u8codec->toUnicode("通话结束。");
        // log_msg = QString("<p><img src='%1'/> %2</p>").arg(":/skins/default/info.png").arg(log_msg);
        // this->uiw->plainTextEdit->appendPlainText(log_msg);
        // this->uiw->plainTextEdit->appendHtml(log_msg);
        log_msg = log_time + " " + log_msg;
        witem = new QListWidgetItem(QIcon(":/skins/default/info.png"), log_msg);
        this->uiw->listWidget->addItem(witem);
        break;
    case 118:
        log_msg = u8codec->toUnicode(QString("对方已挂机，代码:%1").arg(fields.at(5)).toAscii());
        // log_msg = QString("<p><img src='%1'/> %2</p>").arg(":/skins/default/info.png").arg(log_msg);
        // this->uiw->plainTextEdit->appendPlainText(log_msg);
        // this->uiw->plainTextEdit->appendHtml(log_msg);
        log_msg = log_time + " " + log_msg;
        witem = new QListWidgetItem(QIcon(":/skins/default/info.png"), log_msg);
        this->uiw->listWidget->addItem(witem);
        break;
    default:
        qDebug()<<"Unknwon ws msg:"<<msg;
        break;
    }

    // 清除多余日志
    static int max_log_count = 2;
    if (this->uiw->listWidget->count() > max_log_count) {
        int rm_count = this->uiw->listWidget->count() - max_log_count;
        for (int i = rm_count - 1; i >= 0; i--) {
            witem = this->uiw->listWidget->takeItem(max_log_count+i);
            delete witem;
        }
    }
}


