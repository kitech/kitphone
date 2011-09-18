// skycitengine.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-15 09:32:20 +0000
// Version: $Id: skycitengine.cpp 999 2011-09-17 14:25:17Z drswinghead $
// 

#include "simplelog.h"

#include "skycit.h"

#include "intermessage.h"
#include "websocketserver3.h"
#include "skycitengine.h"

SkycitEngine *SkycitEngine::m_inst = NULL;
SkycitEngine::SkycitEngine(QObject *parent)
    : QThread(parent)
{
    this->sct = NULL;
    this->sigbr = NULL;
    this->mws = NULL;

    this->old_thread = this->thread();
    // confirm all this's slot run in this run thread
    this->moveToThread(this);
    QObject::connect(this, SIGNAL(started()), this, SLOT(onSCEThreadStarted()));

    this->sct = new Skycit("karia2");
    QObject::connect(this->sct, SIGNAL(skypeError(int, QString, QString)),
                     this, SLOT(onSkypeError(int, QString, QString)));
    QObject::connect(this->sct, SIGNAL(skypeNotFound()),
                     this, SLOT(onSkypeNotFound()));
    
    QObject::connect(this->sct, SIGNAL(connected(QString)), this, SLOT(onSkypeConnected(QString)));
    QObject::connect(this->sct, SIGNAL(realConnected(QString)),
                     this, SLOT(onSkypeRealConnected(QString)));

    QObject::connect(this->sct, SIGNAL(UserStatus(QString,int)),
                     this, SLOT(onSkypeUserStatus(QString,int)));

    QObject::connect(this->sct, SIGNAL(newCallArrived(QString,QString,int)),
                     this, SLOT(onSkypeCallArrived(QString,QString,int)));
    QObject::connect(this->sct, SIGNAL(callHangup(QString,QString,int)),
                     this, SLOT(onSkypeCallHangup(QString,QString,int)));

}

SkycitEngine *SkycitEngine::instance()
{
    if (SkycitEngine::m_inst == NULL) {
        SkycitEngine::m_inst = new SkycitEngine();
    }
    return SkycitEngine::m_inst;
}

SkycitEngine::~SkycitEngine()
{

}

void SkycitEngine::run()
{
    qLogx()<<""<<QThread::currentThread();

    bool bret = false;
   
    this->sigbr = new SCBSignalBridge();

    QObject::connect(this->sigbr, SIGNAL(websocket_error(int)), this, SLOT(onWS_websocket_error(int)));
    QObject::connect(this->sigbr, SIGNAL(websocket_started()), this, SLOT(onWS_websocket_started()));
    QObject::connect(this->sigbr, SIGNAL(new_websocket_connection()), this, SLOT(onWS_new_websocket_connection()));
    QObject::connect(this->sigbr, SIGNAL(websocket_message(const QString&, int)),
                     this, SLOT(onWS_websocket_message(const QString &, int)));
    QObject::connect(this->sigbr, SIGNAL(websocket_connection_closed(int)),
                     this, SLOT(onWS_websocket_connection_closed(int)));

    this->mws = WebSocketServer3::instance();
    this->mws->error.connect(boost::bind(&SCBSignalBridge::on_websocket_error, this->sigbr, _1));
    this->mws->started.connect(boost::bind(&SCBSignalBridge::on_websocket_started, this->sigbr));
    this->mws->new_connection.connect(boost::bind(&SCBSignalBridge::on_new_websocket_connection, this->sigbr));
    this->mws->new_message.connect(boost::bind(&SCBSignalBridge::on_websocket_message, this->sigbr, _1, _2));
    this->mws->connection_closed.connect(boost::bind(&SCBSignalBridge::on_websocket_connection_closed, this->sigbr, _1));
    // this->mws->error.connect(boost::bind(&SkycitEngine::onWS_websocket_error, this, _1));
    // this->mws->started.connect(boost::bind(&SkycitEngine::onWS_websocket_started, this));
    // this->mws->new_connection.connect(boost::bind(&SkycitEngine::onWS_new_websocket_connection, this));
    // this->mws->new_message.connect(boost::bind(&SkycitEngine::onWS_websocket_message, this, _1, _2));
    // this->mws->connection_closed.connect(boost::bind(&SkycitEngine::onWS_websocket_connection_closed, this, _1));

    if (!this->mws->isRunning()) {
        // TODO start here maybe has block 
        bret = this->mws->start();
    }

    // this->runin();
    this->exec();
}

// void SkycitEngine::runin()
// {
//     qLogx()<<"";


//     // this->moveToThread(this->sigbr);
// }

void SkycitEngine::onSCEThreadStarted()
{
    qLogx()<<""<<this->thread();
    // confirm all this's slot run in this run thread
    // this->moveToThread(this);
    // emit runin_it();
}

void SkycitEngine::onWS_websocket_error(int eno)
{
    qLogx()<<eno;
}

void SkycitEngine::onWS_websocket_started()
{
    qLogx()<<QThread::currentThread()<<this<<(QThread::currentThread() == this);
    // this->sip_engine_error(eno);
    emit this->skycit_engine_started();
}

// TODO support multi client connect, from desktop app or web app
void SkycitEngine::onWS_new_websocket_connection()
{
    qLogx()<<"";
    qLogx()<<QThread::currentThread()<<this<<(QThread::currentThread() == this);
    std::string mvstr;
    // int cseq = this->mws->nextPendingConnection();
    // assert(cseq > 0);

    // if (this->su1.cseq != -1) {
    //     qLogx()<<"only allow 1 client:"<<this->su1.cseq<<", drop this."<<cseq;
    // }


    // mvstr = this->mws->conn_payload_path(cseq);
    // this->wsconns.left.insert(boost::bimap<int,int>::left_value_type(cseq, cseq));

    // // TODO check connect validation
    // this->su1.cseq = cseq;

    // ////////
    // this->move_runin(boost::bind(&SkycitEngine::on_send_codec_list, this));
}

void SkycitEngine::onWS_websocket_message(const QString &msg, int cseq)
{
    qLogx()<<""<<msg<<cseq;
    qLogx()<<QThread::currentThread()<<this<<(QThread::currentThread() == this);

    this->process_ctrl_message(msg.toStdString(), cseq);

    // char tbuf[512] = {0};
    // snprintf(tbuf, sizeof(tbuf), "Returned by server via %d ,%s", cseq, msg.toAscii().data());  
    // std::string ret_msg = std::string(tbuf, strlen(tbuf));

    // this->move_runin(boost::bind(&SkycitEngine::process_ctrl_message, this, msg, cseq));

    // for test only
    // this->mws->wssend(cseq, ret_msg);
}

void SkycitEngine::onWS_websocket_connection_closed(int cseq)
{
    // assert(this->su1.cseq == cseq);
    // this->su1.cseq = -1;
    qLogx()<<""<<cseq;
}

bool SkycitEngine::process_ctrl_message(const std::string &msg, int cseq)
{
    qLogx()<<"processing: "<<cseq<<msg.length()<<msg.c_str();

    bool bret = false;
    int cmd_id = -1;
    int cmd_len;
    int call_id = -1;

    CmdMakeCall mc;
    CmdRegister reger;

    cmd_id = InterMessage().jpack_cmdid(msg);

    switch (cmd_id) {
    // case IPC_NO_MAKE_CALL:
    //     InterMessage().junpack_message(mc, msg);
    //     this->su1.mc_str = msg;
    //     this->su1.cmd_mc = mc;
    //     bret = this->make_call(mc.acc_id, mc.caller_name, mc.callee_phone, mc.sip_server,
    //                            call_id, mc);
    //     if (call_id != PJSUA_INVALID_ID) {
    //         this->su1.call_id = call_id;
    //     }
    //     break;
     case IPC_NO_REGISTER:
         this->sct->connectToSkype();
    //     InterMessage().junpack_message(reger, msg);
    //     this->su1.reg_str = msg;
    //     this->su1.cmd_reg = reger;
    //     bret = this->register_account(reger.display_name, reger.user_name, reger.sip_server,
    //                                   !reger.unregister, reger.password, msg);
         break;
    default:
        qLogx()<<"Unsupported cmd:"<<cmd_id;
        break;
    }

    return true;
}


void SkycitEngine::onConnectSkype()
{
    // this->log_output(LT_USER, "正在连接Skype API。。。，");
    // this->log_output(LT_USER, "请注意Skype客户端弹出的认证提示。");
    // this->uiw->pushButton_3->setEnabled(false);
    // this->onInitPstnClient();

    qDebug()<<"Skype name:"<<this->sct->handlerName();
}

void SkycitEngine::onConnectApp2App()
{
    // QString skypeName = this->uiw->lineEdit->text();
    // this->mtun->setStreamPeer(skypeName);

    // QStringList contacts = this->mSkype->getContacts();
    // qDebug()<<skypeName<<contacts;

    // this->mSkype->newStream(skypeName);
    // this->mSkype->newStream("drswinghead");
}

void SkycitEngine::onSkypeNotFound()
{
    qLogx()<<"";
    // this->m_call_button_disable_count.ref();
    // this->uiw->pushButton_3->setEnabled(true);
    // this->log_output(LT_USER, QString("未安装或未登陆Skype客户端"));
}

void SkycitEngine::onSkypeConnected(QString user_name)
{
    qDebug()<<"Waiting handler name:"<<user_name;
}

void SkycitEngine::onSkypeRealConnected(QString user_name)
{
    qDebug()<<"Got handler name:"<<user_name;
    // this->uiw->label_3->setText(user_name);

    // count == 0
    // if (this->m_call_button_disable_count.deref() == false) {
    //     this->uiw->pushButton_4->setEnabled(true);
    // }

    // this->log_output(LT_USER, "连接Skype API成功，用户名：" + user_name);
    CmdResolvName cmd;
    cmd.name = user_name.toStdString();

    std::string jstr = InterMessage().jpack_message(cmd);
    
}

void SkycitEngine::onSkypeUserStatus(QString str_status, int int_status)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<str_status<<int_status;
    // QPixmap icon = QPixmap(":/skins/default/status_offline.png");
    // switch (int_status) {
    // case SS_ONLINE:
    // case SS_INVISIBLE:
    // case SS_SKYPEME:
    //     icon = QPixmap(":/skins/default/status_online.png");
    //     break;
    // case SS_AWAY:
    //     icon = QPixmap(":/skins/default/status_away.png");
    //     break;
    // case SS_DND:
    //     icon = QPixmap(":/skins/default/status_dnd.png");
    //     break;
    // case SS_NA:
    //     icon = QPixmap(":/skins/default/status_busy.png");
    //     break;
    // case SS_OFFLINE:
    //     break;
    // default:
    //     break;
    // }

    // this->uiw->label_14->setPixmap(icon);
    
    // QString old_tooltip = this->uiw->label_14->toolTip();
    // QString new_tooltip = old_tooltip.split(":").at(0) + ": " + str_status;
    // this->uiw->label_14->setToolTip(new_tooltip);
}

void SkycitEngine::onSkypeError(int code, QString msg, QString cmd)
{
    qLogx()<<code<<msg<<cmd;
}

void SkycitEngine::onSkypeCallArrived(QString callerName, QString calleeName, int callID)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<callerName<<calleeName<<callID;
    // this->m_curr_skype_call_id = callID;
    // this->m_curr_skype_call_peer = calleeName;
}

void SkycitEngine::onSkypeCallHangup(QString contactName, QString callerName, int callID)
{
    qLogx()<<"";
    int skype_call_id = callID;

    // if (callID != this->m_curr_skype_call_id) {
    //     qLogx()<<"Warning:";
    // }
    // this->m_curr_skype_call_id = -1;
    // if (this->m_call_state_widget->isVisible()) {
    //     this->onDynamicSetVisible(this->m_call_state_widget, false);
    // }

    // if (this->wscli.get() && this->wscli->isClosed()) {
    //     qLogx()<<"ws ctrl unexception closed, should cleanup here now.";
    //     this->log_output(LT_USER, "通话异常终止，请重试。");

    //     {
    //         this->onDynamicSetVisible(this->m_call_state_widget, false);
    //         // 关闭网络连续，如果有的话。
    //         this->wscli.reset();
    //         this->uiw->pushButton_4->setEnabled(true);
    //     }
    // }
}

//////////////////
SkycitEngine::SessionUnit::SessionUnit()
{
    //this->wsi = NULL;
    this->cseq = -1;
    this->acc_id = -1;
}

SkycitEngine::SessionUnit::~SessionUnit()
{

}


///////////////////////////
SCBSignalBridge::SCBSignalBridge(QObject *parent)
    : QThread(parent)
{
    qLogx()<<this->thread();
}

SCBSignalBridge::~SCBSignalBridge()
{

}

// void SCBSignalBridge::run()
// {
//     this->exec();
// }

void SCBSignalBridge::on_websocket_error(int eno)
{
    emit websocket_error(eno);
    qLogx()<<eno;
}

void SCBSignalBridge::on_websocket_started()
{
    emit websocket_started();
    // qLogx()<<QThread::currentThread()<<this<<(QThread::currentThread() == this);
    // this->sip_engine_error(eno);
    // emit this->move_runin(boost::bind(&SkycitEngine::on_websocket_started, this));    
    // int seq = ++this->async_seq;
    // boost::function<void()> func = boost::bind(&SkycitEngine::on_websocket_started, this);
    // this->async_funcs[seq] = func;
    // emit this->move_runin(seq);
    
}

// TODO support multi client connect, from desktop app or web app
void SCBSignalBridge::on_new_websocket_connection()
{
    qLogx()<<"";
    emit new_websocket_connection();

    // qLogx()<<QThread::currentThread()<<this<<(QThread::currentThread() == this);
    // std::string mvstr;
    // int cseq = this->mws->nextPendingConnection();
    // assert(cseq > 0);

    // if (this->su1.cseq != -1) {
    //     qLogx()<<"only allow 1 client:"<<this->su1.cseq<<", drop this."<<cseq;
    // }


    // mvstr = this->mws->conn_payload_path(cseq);
    // this->wsconns.left.insert(boost::bimap<int,int>::left_value_type(cseq, cseq));

    // // TODO check connect validation
    // this->su1.cseq = cseq;

    // ////////
    // this->move_runin(boost::bind(&SkycitEngine::on_send_codec_list, this));
}

void SCBSignalBridge::on_websocket_message(const std::string &msg, int cseq)
{
    QString qmsg = QString::fromStdString(msg);
    emit websocket_message(qmsg, cseq);
    // qLogx()<<""<<msg.c_str()<<cseq;
    // qLogx()<<QThread::currentThread()<<this<<(QThread::currentThread() == this);

    // if (QThread::currentThread() == this) {
    //     qLogx()<<"got from my wanted thread";
    // } else {
    //     int seq = ++this->async_seq;
    //     boost::function<void()> func = boost::bind(&SkycitEngine::on_websocket_message, this, msg, cseq);
    //     this->async_funcs[seq] = func;
    //     // emit this->move_runin(seq);
    //     this->move_runin_wrapper(this, seq);
    // }

    char tbuf[512] = {0};
    snprintf(tbuf, sizeof(tbuf), "Returned by server via %d ,%s", cseq, msg.c_str());  
    std::string ret_msg = std::string(tbuf, strlen(tbuf));

    // this->move_runin(boost::bind(&SkycitEngine::process_ctrl_message, this, msg, cseq));

    // for test only
    // this->mws->wssend(cseq, ret_msg);
}

void SCBSignalBridge::on_websocket_connection_closed(int cseq)
{
    // assert(this->su1.cseq == cseq);
    // this->su1.cseq = -1;
    qLogx()<<""<<cseq;
    emit websocket_connection_closed(cseq);
}

