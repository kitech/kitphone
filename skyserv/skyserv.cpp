// skyserv.cpp ---
//
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL:
// Created: 2010-07-03 15:35:48 +0800
// Version: $Id: skyserv.cpp 883 2011-05-19 03:42:32Z drswinghead $
//

#include <sys/socket.h>
#include <typeinfo>

#include <QtCore>

#include "pjsip-ua/sip_inv.h"  // for PJSIP_INV_STATE_CONFIRMED

// #include "utils.h"
#include "simplelog.h"
#include "websocket.h"
#include "metauri.h"
#include "skype.h"

#include "configs.h"
#include "database.h"

#include "sip_proc.h"
#include "skyserv.h"

#include "limit_detect.h"

#include "newlisp_api.h"
#include "lisp_bridge.h"

#include "nbtimer.h"

struct abcde {
    void operator()() {
        printf("in slot, recive timeout event\n");
    }
};

SkyServ::SkyServ(QObject *parent)
    : QObject(parent)
{
    // MyTimer *t = new MyTimer();
    // t->start(3300);
    MyTimer::singleShot(3200, abcde());
    // auto ff = XObject::NBSLOTx(this, &SkyServ::onSipCallFinished);
    // XObject::NBSLOTn(this, SkyServ::onSipCallFinished);
    
    // auto ff = NBSLOT3(SkyServ::onSipCallFinished, this); // ok

    // decltype(&SkyServ::onSipCallFinished) a;
    
    // const std::type_info & ti = typeid(&SkyServ::send_ws_command_100);
    // qDebug()<<ti.name();
    
    // MyTimer *t2 = new MyTimer();
    // auto fn = boost::bind(&SkyServ::onNewWSConnection, this);
    // XObject::connect2(t2->sig_timeout, NBSLOT(this, SkyServ::onNewWSConnection));
    
    // return;

    this->init_unix_signal_handlers();
    this->quit_cleaning = false;

    this->start_time = QDateTime::currentDateTime();
    this->first_reconnect_times = 0;
    this->scn_ws_serv2 = nullptr;
    // this->reconnSkypeTimer = NULL;
    // this->reconnSkypeTimer = new QTimer();
    this->reconnSkypeTimer = boost::shared_ptr<QTimer>(new QTimer());
    this->reconnSkypeTimer->setInterval(2000);
    QObject::connect(this->reconnSkypeTimer.operator->(), SIGNAL(timeout()), this, SLOT(onReconnSkypeTimeout()));

    qDebug()<<"my self proccess id:"<<getpid();
    // qlog("%s, %d\n", "my self proccess id:", getpid());
    // todo , connection stabilty
    this->db = new Database();
    if (!this->db->connectdb()) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"connect database error";
        exit(-1);
    }

    this->first_connected = false;
    this->lder = new LimitDetector();
    QObject::connect(this->lder, SIGNAL(exception_detected(int)),
                     this, SLOT(onLimitationExceptionDetected(int)));

    this->mSkype = new Skype("karia2");
    QStringList router_names;
    // this->mSkype->setSpecifiedRouters(Configs().getSkypeRouters());
    auto routers = Configs().getSkypeRouters();
    std::for_each(routers.begin(), routers.end(),
                  [&] (QPair<QString,QString> &elm) {
                      router_names << elm.first;
                  });
    this->mSkype->setSpecifiedRouters(router_names);

    QObject::connect(this->mSkype, SIGNAL(commandResponse(QString, QString)),
                     this, SLOT(onSkypeRawMessage(QString, QString)));
    QObject::connect(this->mSkype, SIGNAL(connected(QString)),
                     this, SLOT(onSkypeConnected(QString)));
    QObject::connect(this->mSkype, SIGNAL(realConnected(QString)),
                     this, SLOT(onSkypeRealConnected(QString)));
    QObject::connect(this->mSkype, SIGNAL(connectionLost(QString)),
                     this, SLOT(onSkypeDisconnected(QString)));
    QObject::connect(this->mSkype, SIGNAL(skypeError(int, QString, QString)),
                     this, SLOT(onSkypeError(int, QString, QString)));

    QObject::connect(this->mSkype, SIGNAL(packageArrived(QString, int, QString)),
                     this, SLOT(onSkypePackageArrived(QString, int, QString)));
    QObject::connect(this->mSkype, SIGNAL(newRouteCallArrived(QString, QString, QString, int)),
                     this, SLOT(onNewRouteCallArrived(QString, QString, QString, int)));

    QObject::connect(this->mSkype, SIGNAL(routeCallTransferred(int, QString, QString, QString)),
                     this, SLOT(onRouteCallTransferred(int, QString, QString, QString)));
    QObject::connect(this->mSkype, SIGNAL(routeCallMissed(int, QString, QString)),
                     this, SLOT(onRouteCallMissed(int, QString, QString)));
    QObject::connect(this->mSkype, SIGNAL(routeCallRefused(int, QString, QString)),
                     this, SLOT(onRouteCallRefused(int, QString, QString)));

    QObject::connect(this->mSkype, SIGNAL(newForwardCallArrived(QString, QString, int)),
                     this, SLOT(onNewForwardCallArrived(QString, QString, int)));
    // QObject::connect(this->mSkype, SIGNAL(callParticipantArrived(int, QString, QString, QString)),
    //                  this, SLOT(onSkypeCallParticipantArrived(int, QString, QString, QString)));
    // QObject::connect(this->mSkype, SIGNAL(forwardCallPstnArrived(int, QString, QString, QString)),
    //                  this, SLOT(onSkypeForwardCallPstnArrived(int, QString, QString, QString)));

    QObject::connect(this->mSkype, SIGNAL(callHangup(QString, QString, int)),
                     this, SLOT(onSkypeCallHangup(QString, QString, int)));

    QObject::connect(this->mSkype, SIGNAL(forwardCallAnswered(int, QString, QString)),
                     this, SLOT(onSkypeForwardCallAnswered(int, QString, QString)));
    QObject::connect(this->mSkype, SIGNAL(dtmfSended(int, QString, QString, QString)),
                     this, SLOT(onSkypeForwardCallDtmfArrived(int, QString, QString, QString)));
    QObject::connect(this->mSkype, SIGNAL(forwardCallHold(int, QString, QString)),
                     this, SLOT(onSkypeForwardCallHold(int, QString, QString)));
    QObject::connect(this->mSkype, SIGNAL(forwardCallUnhold(int, QString, QString)),
                     this, SLOT(onSkypeForwardCallUnhold(int, QString, QString)));


    if (!this->mSkype->connectToSkype()) {
        qDebug()<<__FILE__<<__LINE__<<"connect to skype faild. Retry now.";
        this->reconnSkypeTimer->start();
    } else {
        // qDebug()<<__FILE__<<__LINE__<<"connected skype instance:"<<this->mSkype->handlerName();
        // this->init_ws_serv(this->mSkype->handlerName());
    }

    ////////// test new structure
    // socketpair has limit, support NONBLOCK mode sience > 2.6.27, so can not use this method

    // 这个server只在本机上使用，负责qt线程与sip线程之间的通信。
    QHostAddress serv_addr("127.0.0.1");
    this->skype_sip_rpc_serv = new QTcpServer();
    QObject::connect(this->skype_sip_rpc_serv, SIGNAL(newConnection()),
                     this, SLOT(onIncomingRpcConnection()));
    this->skype_sip_rpc_serv->listen(serv_addr);
    qDebug()<<serv_addr<<this->skype_sip_rpc_serv->serverPort();

    // sip_proc_param_t *param = (sip_proc_param_t *)calloc(1, sizeof(sip_proc_param_t));
    // memset(param, 0, sizeof(sip_proc_param_t));
    // param->peer_port = this->skype_sip_rpc_serv->serverPort();

    unsigned short peer_port = this->skype_sip_rpc_serv->serverPort();
    void *param = calloc(1, sizeof(unsigned short));
    memcpy(param, &peer_port, sizeof(unsigned short));
    // pthread_t sip_main_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // pthread_create(&sip_main_thread, &attr, &sip_main_proc, param);
    pthread_create(&this->sip_main_thread, &attr, &sip_main_proc, param);
    pthread_attr_destroy(&attr);

    /// test lisp robot
    // this->nlbr = new NLBridge();
    // this->nlbr->initEnv();
    // this->nlbr->load();

    // sleep(55);
    // exit(0);

    /// test websocket
    // WebSocket *ws = new WebSocket();
    // ws->listen(8080);
}

SkyServ::~SkyServ()
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;
    // delete this->hSip; this->hSip = NULL;
    // this->mSkype->disconnectFromSkype();
    // delete this->mSkype;
    // this->db->disconnectdb();
    // delete this->db;
}

// static 
int SkyServ::sighupFd[2] = {0};
int SkyServ::sigtermFd[2] = {0};
int SkyServ::sigintFd[2] = {0};
int SkyServ::sigquitFd[2] = {0};

// static 
void SkyServ::init_unix_signal_handlers()
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, SkyServ::sighupFd)) {
        qFatal("Couldn't create HUP socketpair");
    }
    
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, SkyServ::sigtermFd)) {
        qFatal("Couldn't create TERM socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, SkyServ::sigintFd)) {
        qFatal("Couldn't create INT socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, SkyServ::sigquitFd)) {
        qFatal("Couldn't create QUIT socketpair");
    }

    this->snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

    this->snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));

    this->snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

    this->snQuit = new QSocketNotifier(sigquitFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snQuit, SIGNAL(activated(int)), this, SLOT(handleSigQuit()));

}

// static 
void SkyServ::hupSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(SkyServ::sighupFd[0], &a, sizeof(a));
}

// static 
void SkyServ::termSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(SkyServ::sigtermFd[0], &a, sizeof(a));
}

// static 
void SkyServ::intSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(SkyServ::sigintFd[0], &a, sizeof(a));
}

// static 
void SkyServ::quitSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(SkyServ::sigquitFd[0], &a, sizeof(a));
}

// 重加载配置文件!!!
void SkyServ::handleSigHup()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snHup->setEnabled(false);
     char tmp;
     ::read(sighupFd[1], &tmp, sizeof(tmp));

     // do Qt stuff

    this->snHup->setEnabled(true);
}
void SkyServ::handleSigTerm()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snTerm->setEnabled(false);
     char tmp;
     ::read(sigtermFd[1], &tmp, sizeof(tmp));

     // do Qt stuff

    this->snTerm->setEnabled(true);
}

// 妥善退出
void SkyServ::handleSigInt()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snInt->setEnabled(false);
     char tmp;
     ::read(sigintFd[1], &tmp, sizeof(tmp));

     // do Qt stuff
     // this->on_app_want_quit();

    this->snInt->setEnabled(true);
}
// 妥善退出
void SkyServ::handleSigQuit()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snQuit->setEnabled(false);
     char tmp;
     ::read(sigquitFd[1], &tmp, sizeof(tmp));

     // do Qt stuff
     this->on_app_want_quit();

    this->snQuit->setEnabled(true);
}

// 没有用呢???
void SkyServ::on_app_want_quit()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->quit_cleaning = true;

    // depcreated    
//     if (this->activeGateways.count() > 0) {
//         qDebug()<<"Maybe have active call";
// #if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)) && __GXX_EXPERIMENTAL_CXX0X__)
//         typedef decltype(this->activeGateways.begin()) Tmp;
//         Tmp it;
// #else
//         typeof(this->activeGateways.begin()) it;
// #endif
//         for (it = this->activeGateways.begin(); it != this->activeGateways.end(); it ++) {
//             qDebug()<<QString("call -> gateway: %1 -> %2").arg(it.key()).arg(it.value());
//         }
//     }

    // TODO 用新的call_meta_info了
//     if (this->mSkypeSipCallMap.count() > 0) {
//         qDebug()<<"Maybe have active call2";
// #if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)) && __GXX_EXPERIMENTAL_CXX0X__)
//         typedef decltype(this->mSkypeSipCallMap.leftBegin()) TmpType;
//         TmpType it = this->mSkypeSipCallMap.leftBegin(); // ok
// #else
//         typeof(this->mSkypeSipCallMap.leftBegin()) it = this->mSkypeSipCallMap.leftBegin(); // ok
// #endif
//         for (; it != this->mSkypeSipCallMap.leftEnd(); it ++) {
//             qDebug()<<QString("skype id -> sip id: %1 -> %2").arg(it.key()).arg(it.value());
//             this->mSkype->setCallHangup(QString::number(it.key()));
//         }
//     }

    this->reconnSkypeTimer->stop();
    this->reconnSkypeTimer.reset();

    // delete this->scn_ws_serv;
    // this->scn_ws_serv = NULL;
    delete this->scn_ws_serv2;
    this->scn_ws_serv2 = NULL;

    ::quit_sip_proc = true;
    this->skype_sip_rpc_peer->close();
    delete this->skype_sip_rpc_peer;
    this->skype_sip_rpc_peer = NULL;
    
    this->skype_sip_rpc_serv->close();
    delete this->skype_sip_rpc_serv;
    this->skype_sip_rpc_serv = NULL;

    qDebug()<<"waiting sip thread done...";
    pthread_join(this->sip_main_thread, NULL);
    qDebug()<<"sip thread done.";

    this->mSkype->disconnectFromSkype();
    delete this->mSkype;
    this->mSkype = NULL;

    this->db->disconnectdb();
    delete this->db;
    this->db = NULL;

    delete this->snInt;

    // 现在，可以退出了。
    qApp->quit();
}

void SkyServ::onReconnSkypeTimeout()
{
    qDebug()<<__FILE__<<__LINE__<<"";
    if (this->mSkype->isConnected()) {
        this->reconnSkypeTimer->stop();
    } else {
        if (this->first_connected == false) {
            this->first_reconnect_times ++;
        }
        this->mSkype->connectToSkype();
    }
}

void SkyServ::clear_skype_history()
{
    // 0.5 sec -- 6.5 sec 之间随机某时间之后调用
    QTimer::singleShot(rand() % 6000 + 500, this, SLOT(clear_skype_history_sched()));
}
void SkyServ::clear_skype_history_sched()
{
    if (rand() % 100 == 99) {
        this->mSkype->clearCallHistory();
        this->mSkype->clearChatHistory();
        this->mSkype->clearVoiceMailHistory();
    }
}

void SkyServ::onSkypeRawMessage(QString skypeName, QString msg)
{
    // qlog("%s, %s", skypeName.toAscii().data(), msg.toAscii().data());
    if (this->first_connected) {
        this->lder->on_skype_status(skypeName, msg);
    }
}

void SkyServ::onSkypeError(int errNo, QString msg, QString cmd)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<errNo<<msg<<cmd;

    int ret = -1;
    QStringList sl;

    sl = cmd.split(" ");
    switch(errNo) {
    case 592: // ERROR 592 ALTER CALL: transferring the call failed"
        // need call id here for hangup this call
        // #20 ALTER CALL 116 TRANSFER drswinghead
        ret = this->db->releaseGateway(sl.at(5), sl.at(5));
        this->mSkype->setCallHangup(sl.at(3));
        // this->activeGateways.remove(sl.at(3).toInt()); // depcreated
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Release gateway result:"<<ret;
        break;
    default:
        break;
    }
}

// maybe skypename is empty
void SkyServ::onSkypeConnected(QString skypeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeName<<this->mSkype->handlerName();
    this->reconnSkypeTimer->stop();

    this->first_connected = true;
}

void SkyServ::onSkypeRealConnected(QString skypeName)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeName<<this->mSkype->handlerName();

  this->first_connected = true;

  this->init_ws_serv(skypeName);
}

void SkyServ::onSkypeDisconnected(QString skypeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeName;
    this->reconnSkypeTimer->start();
}

bool SkyServ::init_ws_serv(QString handle_name)
{
    QStringList router_names;
    auto routers = Configs().getSkypeRouters();
    bool bret = false;

    if (this->scn_ws_serv2 != NULL) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__
                <<"Skype call notice websocket server already listened on"
                <<this->scn_ws_serv2->serverPort();
        return true;
    }

    std::for_each(routers.begin(), routers.end(),
                  [&] (QPair<QString,QString> &elm) {
                      router_names << elm.first;
                  });

    if (router_names.contains(handle_name)) {
        unsigned short router_port = Configs().getRouterPort();
        Q_ASSERT(router_port > 0);
        // this->scn_ws_serv2 = new WebSocketServer();
        // QObject::connect(this->scn_ws_serv2, SIGNAL(newWSMessage(QByteArray, QTcpSocket *)),
        //                  this, SLOT(onRouterWSMessage(QByteArray, QTcpSocket *)));
        // bret = this->scn_ws_serv2->listen(router_port);
        // Q_ASSERT(bret);
        this->scn_ws_serv2 = new WebSocketServer2();
        bret = this->scn_ws_serv2->listen(router_port);
        // bret = this->scn_ws_serv2->listen(0);

        QObject::connect(this->scn_ws_serv2, SIGNAL(newWSMessage(QByteArray, qint64)),
                         this, SLOT(onRouterWSMessage(QByteArray, qint64)));

        QObject::connect(this->scn_ws_serv2, SIGNAL(newConnection()),
                         this, SLOT(onNewWSConnection()));

        QObject::connect(this->scn_ws_serv2, SIGNAL(clientSessionClosed(qint64)),
                         this, SLOT(onWSConnectionClosed(qint64)));
    } else {
        this->scn_ws_serv2 = new WebSocketServer2();
        QObject::connect(this->scn_ws_serv2, SIGNAL(newWSMessage(QByteArray, qint64)),
                         this, SLOT(onForwardWSMessage(QByteArray, qint64)));
        bret = this->scn_ws_serv2->listen(0);
        Q_ASSERT(bret);
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__
                <<"Forward ws port:"<<this->scn_ws_serv2->serverPort();
        QString ws_ipaddr = this->scn_ws_serv2->serverIpAddr(0);
        bret = this->db->setForwardPort(handle_name, this->scn_ws_serv2->serverPort(), ws_ipaddr);
        Q_ASSERT(bret);

        QObject::connect(this->scn_ws_serv2, SIGNAL(newConnection()),
                         this, SLOT(onNewWSConnection()));
        QObject::connect(this->scn_ws_serv2, SIGNAL(clientSessionClosed(qint64)),
                         this, SLOT(onWSConnectionClosed(qint64)));

    }
//     QObject::connect(this->scn_ws_serv2, SIGNAL(newConnection(QString, QTcpSocket*)),
//                      this, SLOT(onNewWSConnection(QString, QTcpSocket *)));
    // QObject::connect(this->scn_ws_serv2, SIGNAL(newConnection()),
    //                     this, SLOT(onNewWSConnection()));

    return true;
}

void SkyServ::onNewStreamCreated(QString contactName, int stream)
{
    Q_UNUSED(contactName);
    Q_UNUSED(stream);
}

void SkyServ::onLimitationExceptionDetected(int r)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<r;
}


/*
  从数据库取一个可用的media forward gatewary
  transfer to this gateway
  // maybe we play sound notice if has error
  now calleePhone is always empty string
  */
void SkyServ::onNewRouteCallArrived(QString callerName, QString calleeName, QString calleePhone, int skypeCallID)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<calleeName<<calleePhone<<skypeCallID;
    // skyserv.cpp onNewRouteCallArrived 129 "yat-sen" "liuguangzhao01" 79
    int ret = -1;
    QString callee_phone;
    QString caller_ipaddr;
    QString gateway;
    unsigned short ws_port = 0;
    QString ws_ipaddr;

    // boost::shared_ptr<call_meta_info> cmi;
    // {
    //     Q_ASSERT(this->nWebSocketNameMap.leftContains(callerName));
    //     cmi = this->nWebSocketNameMap.findLeft(callerName).value();
    //     // cmi->caller_name = callerName;
    //     cmi->callee_name = calleeName;
    //     cmi->skype_call_id = skypeCallID;
    //     cmi->callee_phone = calleePhone; // no use?
    //     cmi->call_state = CallState::CS_CALL_ARRIVED;
    //     cmi->mtime = QDateTime::currentDateTime();

    //     if (this->nSkypeCallMap.leftContains(skypeCallID)) {
    //         // how to do
    //         Q_ASSERT(!this->nSkypeCallMap.leftContains(skypeCallID));
    //     } else {
    //         this->nSkypeCallMap.insert(skypeCallID, cmi);
    //     }
    // }
    call_meta_info *cmi;
    cmi = this->find_call_meta_info_by_caller_name(callerName);
    Q_ASSERT(cmi != NULL);
    cmi->callee_name = calleeName;
    cmi->skype_call_id = skypeCallID;
    cmi->callee_phone = calleePhone; // no use?
    cmi->call_state = CallState::CS_CALL_ARRIVED;
    cmi->mtime = QDateTime::currentDateTime();
    

    if (this->quit_cleaning) {
        qDebug()<<"server is quiting, retry later.";
        this->mSkype->setCallHangup(QString::number(skypeCallID));
        return;
    }

    // 给客户端的信息，确认用户信息。
    // 因为找不到用户信息，所以根本无法确定用户的连接是哪个
    // 没有办法发送信息给用户。
    
    // check call pair first
    callee_phone = this->db->getCallPeer(callerName, caller_ipaddr);
    if (callee_phone.isEmpty() || callee_phone.length() == 0) {
        qDebug()<<"Error: call pair not found.";
        cmi->call_state = CallState::CS_NO_CALL_PAIR;
        this->mSkype->setCallHangup(QString::number(skypeCallID));
        return;
    }

    // 通知正在分配通话线路资源116
    this->send_ws_command_116(callerName, "", skypeCallID, QString("Allocate circuitry..."));
    callee_phone = calleePhone; // ???
    ret = this->db->acquireGateway(callerName, callee_phone, gateway, ws_port, ws_ipaddr);
    if (ret == 200) {
        cmi->call_state = CallState::CS_ROUTER_CONNECTING_SWITCHER;
        cmi->mtime = QDateTime::currentDateTime();

        // this->send_ws_command_100(callerName, gateway, QString("202.108.12.212"), ws_port);
        QString ws_uri = QString("ws://%1:%2/%3/%4/").arg(ws_ipaddr).arg(ws_port)
            .arg(callerName).arg(callee_phone);
        boost::shared_ptr<WebSocketClient> wsc(new WebSocketClient(ws_uri));
        // WebSocketClient *wsc = new WebSocketClient(ws_uri);
        QObject::connect(wsc.get(), SIGNAL(onConnected(QString)), 
                         this, SLOT(on_ws_proxy_connected(QString)));
        QObject::connect(wsc.get(), SIGNAL(onWSMessage(QByteArray)), 
                         this, SLOT(on_ws_proxy_message(QByteArray)));
        wsc->connectToServer(QString("/%1/%2/").arg(callerName).arg(callee_phone));
        // this->wsProxy.insert(callerName, boost::shared_ptr<WebSocketClient>(wsc));
        // this->wsProxy.insert(callerName, wsc->shared_from_this());
        // this->wsProxy.insert(callerName, wsc->refit());
        // Q_ASSERT(this->wsProxy.leftContains(callerName));
        // Q_ASSERT(this->wsProxy.rightContains(wsc->refit()));
        // this->nWebSocketProxyMap.insert(wsc, cmi);
        cmi->ws_proxy = wsc;

        cmi->call_state = CallState::CS_ROUTER_CONNECTING_SWITCHER;
        cmi->ws_proxy = wsc;
        cmi->mtime = QDateTime::currentDateTime();
        
        // 使用代理后，不再把ip/端口信息发送到客户端了。
        // this->send_ws_command_100(callerName, gateway, ws_ipaddr, ws_port);
        // this->activeGateways.insert(skypeCallID, gateway);
        cmi->switcher_name = gateway;
        this->mSkype->setCallForward(QString::number(skypeCallID), gateway);
    } else {
        cmi->call_state = CallState::CS_ROUTER_ALL_LINE_BUSY;
        cmi->mtime = QDateTime::currentDateTime();

        this->send_ws_command_104(callerName, "", skypeCallID, 
                                  QString("All call lines are busy."));
        this->mSkype->setCallHangup(QString::number(skypeCallID));
    }
}

void SkyServ::onRouteCallTransferred(int skypeCallID, QString callerName, QString calleeName, QString last_status)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<calleeName<<skypeCallID;
    int ret = 0;

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skypeCallID));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skypeCallID).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);

    QString gateway = cmi->switcher_name;

    if (last_status == "FINISHED" || last_status == "UNPLACED"
        || last_status == "RINGING" || last_status == "ROUTING") {
        // mayby user canceled before transferred to gatewary, #see 307
        // the normal status should be "INPROGRESS"
        // should relase gateway
        cmi->call_state = CallState::CS_CALL_FINISHED;
        cmi->mtime = QDateTime::currentDateTime();

        ret = this->db->releaseGateway(calleeName, gateway);
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Release gateway result:"<<ret;

    }
    
    // 永远记住，直到通话终止，包括与switcher的通话在内。这个不需要了。
    // this->activeGateways.remove(skypeCallID); // ????
}

void SkyServ::onRouteCallMissed(int skypeCallID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<calleeName<<skypeCallID;
    int ret = 0;
    // QString gateway = this->activeGateways.value(skypeCallID);

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skypeCallID));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skypeCallID).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);

    cmi->call_state = CallState::CS_CALL_MISSED;
    cmi->mtime = QDateTime::currentDateTime();

    QString gateway = cmi->switcher_name;
    ret = this->db->releaseGateway(calleeName, gateway);

    // this->activeGateways.remove(skypeCallID);

    this->mSkype->setCallHangup(QString::number(skypeCallID));

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Release gateway result:"<<ret;
}

void SkyServ::onRouteCallRefused(int skypeCallID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<calleeName<<skypeCallID;
    auto routers = Configs().getSkypeRouters();
    QStringList router_names;
    int ret;

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skypeCallID));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skypeCallID).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);

    cmi->call_state = CallState::CS_CALL_REFUSED;
    cmi->mtime = QDateTime::currentDateTime();

    // because lines busy and hangup 
    std::for_each(routers.begin(), routers.end(),
                  [&](QPair<QString,QString> &elm) {
                      router_names << elm.first;
                  });
    // if (this->mSkype->handlerName() == "liuguangzhao01") {
    if (router_names.contains(this->mSkype->handlerName())) {
        // ctrl switcher, do nothing
        this->send_ws_command_108(callerName, calleeName, skypeCallID, 123, QString("abcd"));
        // this->wsMap.removeLeft(callerName);
        // this->wsProxy.removeLeft(callerName);
        ret = this->db->removeCallPair(callerName);
        if (ret) {}

        // this->nWebSocketSeqMap.removeLeft(cmi->conn_seq);
        // this->nWebSocketNameMap.removeLeft(callerName);
        // this->nWebSocketProxyMap.removeRight(cmi);
        // this->remove_call_meta_info(callerName);
    } else {
    }
}

void SkyServ::onNewForwardCallArrived(QString callerName, QString calleeName, int skypeCallID)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<calleeName<<skypeCallID;
    QString callee_phone;
    QString caller_ipaddr;
    QString record_file_name;

    // Q_ASSERT(this->nWebSocketNameMap.leftContains(callerName));
    // boost::shared_ptr<call_meta_info> cmi = this->nWebSocketNameMap.findLeft(callerName).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);

    {
        cmi->callee_name = calleeName;
        cmi->skype_call_id = skypeCallID;
        cmi->call_state = CallState::CS_CALL_ARRIVED;
        cmi->mtime = QDateTime::currentDateTime();

    }

    // if (this->nSkypeCallMap.leftContains(skypeCallID)) {
    //     Q_ASSERT(!this->nSkypeCallMap.leftContains(skypeCallID));
    // } else {
    //     cmi->callee_name = calleeName;
    //     cmi->skype_call_id = skypeCallID;
    //     cmi->call_state = CallState::CS_CALL_ARRIVED;
    //     cmi->mtime = QDateTime::currentDateTime();

    //     this->nSkypeCallMap.insert(skypeCallID, cmi);
    // }

    if (this->quit_cleaning) {
        qDebug()<<"server is quiting, retry later.";
        this->mSkype->setCallHangup(QString::number(skypeCallID));
        return;
    }

    // 查找 call pair
    callee_phone = this->db->getCallPeer(callerName, caller_ipaddr);
    if (callee_phone.isEmpty() || callee_phone.length() == 0) {
        qDebug()<<"Error: call pair not found.";
        this->mSkype->setCallHangup(QString::number(skypeCallID));
        this->send_ws_command_106(callerName, calleeName, skypeCallID, 
                                  QString("Error: your supplied account and call acount is not match.")
                                  );
        return;
    }

    // this->mSkype->setCallInputNull(QString("%1").arg(skypeCallID));
    // this->mSkype->setCallOutputNull(QString("%1").arg(skypeCallID));
    this->mSkype->setCallInputFile(QString::number(skypeCallID),
                                   QString(getenv("HOME")) + "/SKYPE1.wav");
    // QString("/home/gzleo/SKYPE1.wav"));
    // this->mSkype->setCallInputFile(QString("%1").arg(skypeCallID), QString("/dev/zero"));
    this->mSkype->setCallOutputFile(QString::number(skypeCallID), QString("/dev/null"));
    // 也许这使用一个真实的文件，就不会使得skype客户端拨通之后出来静音的问题。
    // 即使这样，skype客户也会有静音问题。
    // record_file_name = QString(::tempnam(Configs().get_log_path().toAscii().data(), "skp_tmp_out_"))
    // + QString(".wav");
    // record_file_name = QString("%1/skp_tmp_out_%2_%3.wav").arg(Configs().get_log_path().toAscii().data())
    //     .arg(callerName).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"));
    // this->mSkype->setCallOutputFile(QString::number(skypeCallID), record_file_name);
    // qlog("Temporary record: %s\n", record_file_name.toAscii().data());
    
    // accept
    this->mSkype->answerCall(QString::number(skypeCallID));
    // TODO, 如果answerCall 出错,下面不能继续执行. 所以,下面几行,应该放在slot onSkypeCallAnswered中
}

void SkyServ::onSkypeForwardCallAnswered(int skypeCallID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;
    QString callee_phone;
    QString caller_ipaddr;

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skypeCallID));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skypeCallID).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);

    cmi->call_state = CallState::CS_CALL_ANSWERED;
    cmi->mtime = QDateTime::currentDateTime();

    callee_phone = this->db->getCallPeer(callerName, caller_ipaddr);
    if (callee_phone.isEmpty() || callee_phone.length() == 0) {
        qDebug()<<"Error: call pair not found.";
        this->mSkype->setCallHangup(QString::number(skypeCallID));
        return;
    }

    // sip call, sip serv addr 要放在配置文件中
    QHash<QString, int> sip_servers = Configs().getSipServers();
    Q_ASSERT(sip_servers.count() > 0);
    QString serv_addr = sip_servers.begin().key(); //"202.108.29.234:4060";// "172.24.172.21:4060";
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"before sip call phone call";

    // this->mSkypeSipCallMap.insert(skypeCallID, SSCM_WAIT_SIP_CALL_ID);
    cmi->sip_call_id = SSCM_WAIT_SIP_CALL_ID;

    QStringList args;
    args<<callerName<<callee_phone<<serv_addr
        <<QString::number(skypeCallID)
        <<caller_ipaddr
        ;
    QString arg_list = args.join(",");

    int cmdlen = 0;
    char *wbuf = new_rpc_command(10, &cmdlen, "s", arg_list.toAscii().data());
    this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + cmdlen);
    free(wbuf);

    this->send_ws_command_117(callerName, calleeName, skypeCallID, 
                              QString("Info: connect pstn network.")
                              );

    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"after sip call phone call";
}

void SkyServ::onSkypeForwardCallHold(int skypeCallID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;

    int skype_call_id = skypeCallID;
    int sip_call_id = -1;

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skype_call_id).value();

    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);


    sip_call_id = cmi->sip_call_id;
    // Q_ASSERT(this->mSkypeSipCallMap.leftContains(skype_call_id));
    // sip_call_id = this->mSkypeSipCallMap.findLeft(skype_call_id).value();
    QStringList args;
    args<<QString::number(skypeCallID)<<QString::number(sip_call_id)<<QString::number(1);
    QString arg_list = args.join(",");
    // strcpy(cmdbuf, "yat-sen,99008665108013552776960,202.108.29.234:4060,5678");

    int cmdlen = 0;
    char *wbuf = new_rpc_command(18, &cmdlen, "s", arg_list.toAscii().data());
    this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + cmdlen);
    free(wbuf);
    // 
    bool bret = this->send_ws_command_110(callerName, calleeName, skypeCallID,
                                          QString("Maybe ine unstable state."));
    if (bret) {
    }
}

void SkyServ::onSkypeForwardCallUnhold(int skypeCallID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;
    Q_UNUSED(callerName);
    Q_UNUSED(calleeName);

    int skype_call_id = skypeCallID;
    int sip_call_id = -1;

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skypeCallID));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skypeCallID).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);


    cmi->call_state = CallState::CS_CALL_ANSWERED;
    cmi->mtime = QDateTime::currentDateTime();

    sip_call_id = cmi->sip_call_id;
    // Q_ASSERT(this->mSkypeSipCallMap.leftContains(skype_call_id));
    // sip_call_id = this->mSkypeSipCallMap.findLeft(skype_call_id).value();
    QStringList args;
    args<<QString::number(skypeCallID)<<QString::number(sip_call_id)<<QString::number(0);
    QString arg_list = args.join(",");
    // strcpy(cmdbuf, "yat-sen,99008665108013552776960,202.108.29.234:4060,5678");

    int cmdlen = 0;
    char *wbuf = new_rpc_command(18, &cmdlen, "s", arg_list.toAscii().data());
    this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + cmdlen);
    free(wbuf);
}

void SkyServ::onSkypeForwardCallDtmfArrived(int skypeCallID, QString callerName, QString calleeName, QString dtmf)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;
    Q_UNUSED(callerName);
    Q_UNUSED(calleeName);

    int skype_call_id = skypeCallID;
    int sip_call_id = -1;

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skypeCallID));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skypeCallID).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);

    sip_call_id = cmi->sip_call_id;
    // Q_ASSERT(this->mSkypeSipCallMap.leftContains(skype_call_id));
    // sip_call_id = this->mSkypeSipCallMap.findLeft(skype_call_id).value();
    QStringList args;
    args<<QString::number(skypeCallID)<<QString::number(sip_call_id)<<dtmf;
    QString arg_list = args.join(",");
    // strcpy(cmdbuf, "yat-sen,99008665108013552776960,202.108.29.234:4060,5678");
    int cmdlen = 0;
    char *wbuf = new_rpc_command(16, &cmdlen, "s", arg_list.toAscii().data());
    this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + cmdlen);
    free(wbuf);
}

// depcreated
// void SkyServ::onSkypeCallParticipantArrived(int skypeCallID, QString callerName,
//                                             QString callee_name, QString participant)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;
// }

// depcreated
// void SkyServ::onSkypeForwardCallPstnArrived(int skypeCallID, QString callerName,
//                                             QString callee_name, QString pstn)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<skypeCallID<<pstn;

//     pj_status_t status;
//     QString callee_phone;

//     return; // dont use dtmf way
//     // callee_phone = participant.right(participant.length()-1); // drop + prefix
//     callee_phone = pstn;
//     // sip call, serv addr 要放在配置文件中
//     QHash<QString, int> sip_servers = Configs().getSipServers();
//     Q_ASSERT(sip_servers.count() > 0);
//     QString serv_addr = sip_servers.begin().key();//"202.108.29.234:4060";// "172.24.172.21:4060";
//     if (status != PJ_SUCCESS) {
//         qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Call sip peer faild";
//     }
// }

void SkyServ::onSkypeCallHangup(QString contactName, QString calleeName, int skypeCallID)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<contactName<<calleeName<<skypeCallID;
    int ret = 0, ret2 = 0;
    int sip_call_curr_id;
    int sip_call_id;
    int skype_call_id = skypeCallID;
    int cmdlen;
    char *wbuf = NULL;

    QStringList router_names;
    auto routers = Configs().getSkypeRouters();

    std::for_each(routers.begin(), routers.end(),
                  [&](QPair<QString,QString> &elm) {
                      router_names << elm.first;
                  });

    // Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
    // boost::shared_ptr<call_meta_info> cmi = this->nSkypeCallMap.findLeft(skype_call_id).value();
    call_meta_info *cmi = this->find_call_meta_info_by_skype_call_id(skypeCallID);
    Q_ASSERT(cmi != NULL);


    // if (this->mSkype->handlerName() == "liuguangzhao01") {
    if (router_names.contains(this->mSkype->handlerName())) {
        // ctrl router, do nothing
        this->send_ws_command_108(calleeName, contactName, skypeCallID, 123, QString("abcd"));

        // this->nWebSocketNameMap.removeLeft(contactName);
        // this->nWebSocketSeqMap.removeLeft(cmi->conn_seq);
        // this->remove_call_meta_info(contactName);
    } else {
        // release gateway resouce
        ret = this->db->releaseGateway(calleeName, contactName);
        ret2 = this->db->removeCallPair(calleeName);
        this->send_ws_command_108(calleeName, contactName, skypeCallID, 123, QString("abcd"));

        {
            cmi->skype_call_id = -1;
            cmi->call_state = CallState::CS_CALL_FINISHED;
            cmi->mtime = cmi->etime = QDateTime::currentDateTime();
            
            //this->nSkypeCallMap.removeLeft(skypeCallID);
            // this->nWebSocketNameMap.removeLeft(contactName);
            // this->nWebSocketSeqMap.removeLeft(cmi->conn_seq);
        }
        // this->remove_call_meta_info(contactName);
        
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Release gateway result:"<<ret<<ret2;
    }

    this->clear_skype_history();
}

// depcreated
void SkyServ::onSkypePackageArrived(QString contactName, int stream, QString data)
{
    qDebug()<<contactName<<stream<<data;
    SkypePackage sp = SkypePackage::fromString(data);
    // Q_ASSERT(sp.isValid());
    if (!sp.isValid()) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"invalid package";
        return;
    }

    this->processRequest(contactName, stream, &sp);
}

// depcreated
void SkyServ::processRequest(QString contactName, int stream, SkypePackage *sp)
{
    MetaUri mu;
    MetaUri rmu; // response
    SkypePackage rsp; // response package
    QString rspStr;
    QString myStr;
    unsigned short ws_port;
    QString ws_ipaddr;
    int ret = -1;

    switch (sp->type) {
    case SkypePackage::SPT_MU_ADD:
        mu = MetaUri::fromString(sp->data);
        // add to storage here;
        mu.dump();
        rsp.seq = sp->seq; // the same as request
        rsp.type = SkypePackage::SPT_MU_ADD_RESP;
        rsp.data = QString("OK");

        rspStr = rsp.toString();
        this->mSkype->sendPackage(contactName, stream, rspStr);
        break;
    case SkypePackage::SPT_MU_DELETE:
        break;
    case SkypePackage::SPT_GW_SELECT:
        qDebug()<<"SPT_GW_SELECT: "<<sp->data;

        ret = this->db->acquireGateway(contactName, sp->data, myStr, ws_port, ws_ipaddr);

        rsp.seq = sp->seq;
        rsp.type = SkypePackage::SPT_GW_SELECT_RESULT;
        rsp.data = QString("ret=%1&gateway=%2").arg(ret).arg(myStr);
        
        rspStr = rsp.toString();
        this->mSkype->sendPackage(contactName, stream, rspStr);
        this->ccMap[contactName] = sp->data;
        
        break;
    case SkypePackage::SPT_GW_RELEASE:
        
        qDebug()<<"SPT_GW_RELEASE: "<<sp->data;
        // this cmd will recived from gateway
        ret = this->db->releaseGateway(contactName, sp->data);
        // ret = this->db->releaseGateway(contactName, sp->data);
        rsp.seq = sp->seq;
        rsp.type = SkypePackage::SPT_GW_RELEASE_RESULT;
        rsp.data = QString("ret=%1&caller=%2&gateway=%3").arg(ret).arg(contactName).arg(sp->data);
        
        rspStr = rsp.toString();
        this->mSkype->sendPackage(contactName, stream, rspStr);
        this->ccMap.remove(contactName);

        break;
    default:
        Q_ASSERT(1==2);
        break;
    };
}


// from sip signals
void SkyServ::onSipCallFinished(int sip_call_id, int status_code, int skype_call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id<<status_code;

    // if (!this->hSip->mSkypeSipCallMap.rightContains(sip_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Can not found map call id";
    // }
    // int skype_call_id = this->hSip->mSkypeSipCallMap.findRight(sip_call_id).value();
    // this->mSkype->setCallHangup(QString::number(skype_call_id));

    //if (sip_call_id == SSCM_HANGUP_FROM_SIP) {
    this->mSkype->setCallHangup(QString::number(skype_call_id));
    // this->mSkypeSipCallMap.removeLeft(skype_call_id);
    //}

    // boost::shared_ptr<call_meta_info> cmi;
    call_meta_info *cmi = this->find_call_meta_info_by_sip_call_id(sip_call_id);
    // if (this->nSipCallMap.leftContains(sip_call_id)) {
    //     cmi = this->nSipCallMap.findLeft(sip_call_id).value();

    //     if (cmi->skype_call_id != -1) {
    //         Q_ASSERT(cmi->skype_call_id == skype_call_id);
    //     }

    //     cmi->sip_call_id = -1;
    //     cmi->call_state = CallState::CS_SWITCHER_SIP_HANGUP;
    //     cmi->mtime = QDateTime::currentDateTime();
        
    //     this->nSipCallMap.removeLeft(sip_call_id);
    // }
    
}

void SkyServ::onSipCallExceedMaxCount(int skype_call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skype_call_id;
    this->mSkype->setCallHangup(QString::number(skype_call_id));
}

void SkyServ::onSipCallOutgoingMediaServerReady(int sip_call_id, unsigned short port, int skype_call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<port;

    // boost::shared_ptr<call_meta_info> cmi;

    // if (!this->nSipCallMap.leftContains(sip_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id is lost???";        
    //     // Q_ASSERT(this->nSipCallMap.leftContains(sip_call_id));
    //     // cmi = this->find_call_meta_info_by_caller_name(QString(), sip_call_id, -1);
    //     if (cmi == boost::shared_ptr<call_meta_info>()) {
    //         Q_ASSERT(this->nSipCallMap.leftContains(sip_call_id));
    //     }
    // } else {
    //     cmi = this->nSipCallMap.findLeft(sip_call_id).value();
    // }
    // if (!this->nSkypeCallMap.leftContains(skype_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id mapped error skype call id???";
    //     // Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
    //     // cmi = this->find_call_meta_info_by_caller_name(QString(), -1, skype_call_id);
    //     if (cmi == boost::shared_ptr<call_meta_info>()) {
    //         Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
    //     }
    // } else {
    //     cmi = this->nSkypeCallMap.findLeft(skype_call_id).value();
    // }
    // if (!this->mSkypeSipCallMap.rightContains(sip_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id is lost???";        
    //     Q_ASSERT(this->mSkypeSipCallMap.rightContains(sip_call_id));
    // }
    // if (!this->mSkypeSipCallMap.findRight(sip_call_id).value() == skype_call_id) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id mapped error skype call id???";
    //     Q_ASSERT(this->mSkypeSipCallMap.findRight(sip_call_id).value() == skype_call_id);
    // }

    call_meta_info *cmi = this->find_call_meta_info_by_sip_call_id(sip_call_id);
    if (cmi == NULL) {
        
    }

    int ok = this->mSkype->setCallMediaInputPort(QString::number(skype_call_id), port);
    if (!ok) {}
}

// TODO 找出来为什么会有找到对应关系的问题？
// 如果现实确实有存在这种情况的可能，应该如何做后续处理？
void SkyServ::onSipCallIncomingMediaServerReady(int sip_call_id, unsigned short port, int skype_call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<port;

    // boost::shared_ptr<call_meta_info> cmi;

    // if (!this->nSipCallMap.leftContains(sip_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id is lost???";        
    //     // Q_ASSERT(this->nSipCallMap.leftContains(sip_call_id));
    //     // cmi = this->find_call_meta_info_by_caller_name(QString(), sip_call_id, -1);
    //     if (cmi == boost::shared_ptr<call_meta_info>()) {
    //         Q_ASSERT(this->nSipCallMap.leftContains(sip_call_id));
    //     }
    // } else {
    //     cmi = this->nSipCallMap.findLeft(sip_call_id).value();
    // }
    // if (!this->nSkypeCallMap.leftContains(skype_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id mapped error skype call id???";
    //     // Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
    //     // cmi = this->find_call_meta_info_by_caller_name(QString(), -1, skype_call_id);
    //     if (cmi == boost::shared_ptr<call_meta_info>()) {
    //         Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
    //     }
    // } else {
    //     cmi = this->nSkypeCallMap.findLeft(skype_call_id).value();
    // }

    // if (!this->mSkypeSipCallMap.rightContains(sip_call_id)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id is lost???";        
    //     Q_ASSERT(this->mSkypeSipCallMap.rightContains(sip_call_id));
    // }
    // if (!this->mSkypeSipCallMap.findRight(sip_call_id).value() == skype_call_id) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why sip call id mapped error skype call id???";
    //     Q_ASSERT(this->mSkypeSipCallMap.findRight(sip_call_id).value() == skype_call_id);
    // }

    call_meta_info *cmi = this->find_call_meta_info_by_sip_call_id(sip_call_id);

    int ok = this->mSkype->setCallMediaOutputPort(QString::number(skype_call_id), port);

    QString caller_name;
    QString callee_name;

    caller_name = this->mSkype->callPartnerHandlerName(QString::number(skype_call_id));
    callee_name = this->mSkype->handlerName();

    ok = this->send_ws_command_114(caller_name, callee_name, skype_call_id, QString("may be mute 3-5s"));
    if (!ok) {}
}

// sscm value, 
// no pair  
// -888888  SSCM_WAIT_SIP_CALL_ID
// -666666  SSCM_SKYPE_HANGUP_WHEN_WAIT_SIP_CALL_ID
// -555555  SSCM_HANGUP_FROM_SIP
// >=0  got sip call id ok

void SkyServ::onProcessIncomingRpcCommand(int cmdlen, int cmdno, char *cmdbuf)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;

    QStringList args;
    int skype_call_id;
    int sip_call_id;;
    int sip_call_curr_id;
    int sip_call_state;
    int sip_media_state;
    unsigned short n_port;
    char *wbuf;
    QString caller_name, callee_name;
    bool ok = false;
    // boost::shared_ptr<call_meta_info> cmi;
    call_meta_info *cmi = NULL;

    args = QString(cmdbuf).split(",");
    switch (cmdno) {
    case 11:
        // skype_call_id,sip_call_id
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();

        ////////
        // Q_ASSERT(this->nSkypeCallMap.leftContains(skype_call_id));
        // cmi = this->nSkypeCallMap.findLeft(skype_call_id).value();
        cmi = this->find_call_meta_info_by_sip_call_id(skype_call_id);
        Q_ASSERT(cmi != NULL);
        sip_call_curr_id = cmi->sip_call_id;
        if (sip_call_curr_id == SSCM_WAIT_SIP_CALL_ID) {
            // sip call id 计数从0开始？已经确认是从0开始的, >=0 的值。
            if (sip_call_id >= 0) {
                //// this->mSkypeSipCallMap.insert(skype_call_id, sip_call_id);
                // this->nSipCallMap.insert(sip_call_id, cmi);
            } else {
                // should hangup skype now
                this->onSipCallFinished(SSCM_HANGUP_FROM_SIP, 0, skype_call_id);
            }
        } else if (sip_call_curr_id == SSCM_SKYPE_HANGUP_WHEN_WAIT_SIP_CALL_ID) {
            // send hangup cmd to sip proc
            cmdlen = 0;
            wbuf = new_rpc_command(12, &cmdlen, "dd", skype_call_id, sip_call_id);
            this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + cmdlen);
            free(wbuf);
        } else if (sip_call_curr_id == -1) {

        } else if (sip_call_curr_id >= 0) {
            // why recieve more than once??
        } else {
            Q_ASSERT(1 == 2);
        }
        

        break;
    case 13:
        // hangup call request from sip, assert sip is already hangup
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();

        // 挂断原因通知客户端。
        caller_name = this->mSkype->callPartnerHandlerName(QString::number(skype_call_id));
        callee_name = this->mSkype->handlerName();

        ok = this->send_ws_command_118(caller_name, callee_name, skype_call_id, args.at(2),
                                       QString("hangup by peer, reason: %1").arg(args.at(2)));
        if (!ok) {}
        this->onSipCallFinished(sip_call_id, 0, skype_call_id);

        ////////
        // Q_ASSERT(this->nSipCallMap.leftContains(sip_call_id));
        // cmi = this->nSkypeCallMap.findLeft(sip_call_id).value();
        // cmi = this->nSipCallMap.findLeft(sip_call_id).value();
        
        break;
    case 15:
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();
        n_port = args.at(2).toUShort();

        this->onSipCallIncomingMediaServerReady(sip_call_id, n_port, skype_call_id);

        break;
    case 17:
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();
        n_port = args.at(2).toUShort();

        this->onSipCallOutgoingMediaServerReady(sip_call_id, n_port, skype_call_id);

        ////////
        break;
    case 19: // sip call state
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();
        sip_call_state = args.at(2).toInt();

        if (sip_call_state == PJSIP_INV_STATE_CONFIRMED) {
            // send sip answer to ws
            caller_name = this->mSkype->callPartnerHandlerName(args.at(0));
            if (!caller_name.isEmpty()) {
                this->send_ws_command_112(caller_name, this->mSkype->handlerName(), skype_call_id, QString(""));
            } else {
                qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Can't find caller name for call:"<<skype_call_id;
            }
        }

        break;
    case 21:
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();
        sip_media_state = args.at(2).toInt();

        if (sip_media_state) {}

        break;
    default:
        qDebug()<<"Uknown cmdno:"<<cmdno<<cmdbuf;
        break;
    };
}

void SkyServ::onIncomingRpcConnection()
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;
    this->skype_sip_rpc_peer = this->skype_sip_rpc_serv->nextPendingConnection();

    QObject::connect(this->skype_sip_rpc_peer, SIGNAL(readyRead()), 
                     this, SLOT(onRpcServReadyRead()));

    QObject::connect(this->skype_sip_rpc_peer, SIGNAL(aboutToClose()), 
                     this, SLOT(onRpcServAboutToClose()));

    // for test, but it works fine
    int cmdlen = 0;
    char *wbuf = new_rpc_command(10, &cmdlen, "s", "yat-sen,99008665108013552776960,202.108.29.234:4060,5678");
    free(wbuf);

    // int cmdno = 10;
    // char cmdbuf[100] = {0};
    // strcpy(cmdbuf, "yat-sen,99008665108013552776960,202.108.29.234:4060,5678");
    // int cmdlen = strlen(cmdbuf);

    // char wbuf[100] = {0};
    // memcpy(wbuf, &cmdlen, sizeof(int));
    // memcpy(wbuf + sizeof(int), &cmdno, sizeof(int));
    // memcpy(wbuf + 2*sizeof(int), cmdbuf, strlen(cmdbuf));

    // this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + strlen(cmdbuf));
    // this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + strlen(cmdbuf));
}

void SkyServ::onRpcServReadyRead()
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;

    int cmdno;
    int cmdlen;
    char cmdbuf[200] = {0};
    int i = 0;

    while (i ++ < 9 && this->skype_sip_rpc_peer->bytesAvailable() >= 2*sizeof(int)) {
        this->skype_sip_rpc_peer->read((char*)&cmdlen, sizeof(int));
        this->skype_sip_rpc_peer->read((char*)&cmdno, sizeof(int));

        this->skype_sip_rpc_peer->read(cmdbuf, cmdlen);

        qDebug()<<"serv: got siproom cmd: "<<cmdno<<cmdbuf;
        this->onProcessIncomingRpcCommand(cmdlen, cmdno, cmdbuf);
    }
}
    
void SkyServ::onRpcServAboutToClose()
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;
}

void SkyServ::onNewWSConnection()
{
    // QTcpSocket *sock = NULL;
    qint64 cseq = 0;

    cseq = this->scn_ws_serv2->nextPendingConnection();
    if (cseq == 0) {
        // not possible, why??
        Q_ASSERT(cseq != 0);
        return;
    }

    // QObject::connect(sock, SIGNAL(disconnected()), this, SLOT(onWSClose()));

    // QString path = sock->property("payload_path").toString();
    QString path = this->scn_ws_serv2->conn_payload_path(cseq);

    this->onNewWSConnection(path, cseq);
}

void SkyServ::onWSConnectionClosed(qint64 cseq)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<cseq<<" websocket closed.";
    QString key;

    {
        // 必须删除，否则会导致调用无效的ws连接
        // Q_ASSERT(this->nWebSocketSeqMap.leftContains(cseq));
        // boost::shared_ptr<call_meta_info> cmi = this->nWebSocketSeqMap.findLeft(cseq).value();
        call_meta_info *cmi = this->find_call_meta_info_by_conn_seq(cseq);
        Q_ASSERT(cmi != NULL);

        cmi->conn_seq = -1;
        cmi->mtime = QDateTime::currentDateTime();

        // this->nWebSocketNameMap.removeLeft(cmi->caller_name);
        // this->nWebSocketSeqMap.removeLeft(cseq);

        this->remove_call_meta_info(cmi->caller_name);
    }
}

// path=/handle_name/phone
void SkyServ::onNewWSConnection(QString path, qint64 cseq)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;

    if (path.length() <= 2) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"invalid request:"<<path;
        // sock->close();
        // TODO close it        
        return;
    }
    QStringList elems = path.right(path.length() - 1).split('/');
    if (elems.count() < 2) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"invalid request:"<<path;
        // sock->close();
        // TODO close it
        return;
    }
    QString handle_name = elems.at(0);
    QString phone = elems.at(1);

    {
        // boost::shared_ptr<call_meta_info> cmi(new call_meta_info());
        // boost::shared_ptr<call_meta_info> cmi = boost::make_shared<call_meta_info>();
        call_meta_info *cmi = new call_meta_info();
        cmi->caller_name = handle_name;
        // cmi->callee_name = calleeName;
        // cmi->skype_call_id = skypeCallID;
        // cmi->callee_phone = calleePhone;
        cmi->conn_seq = cseq;
        cmi->call_state = CallState::CS_WS_CONNECTED;
        cmi->mtime = cmi->ctime = QDateTime::currentDateTime();

        this->add_call_meta_info(handle_name, cmi);

        // if (this->nWebSocketNameMap.leftContains(handle_name)) {
        //     // how to do
        // } else {
        //     this->nWebSocketNameMap.insert(handle_name, cmi);
        // }

        // if (this->nWebSocketSeqMap.leftContains(cseq)) {
        //     // how to do
        // } else {
        //     this->nWebSocketSeqMap.insert(cseq, cmi);
        // }
    }
}

void SkyServ::onRouterWSMessage(QByteArray msg, qint64 cseq)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<msg;
    QList<QByteArray> fields = msg.split('$');
    QString caller_name;
    QString callee_phone;
    QStringList router_names;
    auto routers = Configs().getSkypeRouters();

    if (fields.count() <= 2) {
        qDebug()<<"Invalid ws command:"<<msg;
        return;
    }

    std::for_each(routers.begin(), routers.end(),
                  [&](QPair<QString,QString> &elm) {
                      router_names << elm.first;
                  });

    switch (fields.at(0).trimmed().toInt()) {
    case 101:  // note pair and request main router
        if (fields.count() < 3 || fields.count() > 6) {
            qDebug()<<"Invalid ws command:"<<msg;
            return;
        }
        caller_name = fields.at(1).trimmed();
        callee_phone = fields.at(2).trimmed();
        // routers = Configs().getSkypeRouters();
        if (router_names.contains(this->mSkype->handlerName())) {
            // i'm router server
            // routers.clear();
            QString resp;
            // QString ipaddr = sock->peerAddress().toString();
            QString ipaddr = this->scn_ws_serv2->conn_get_peer_address(cseq);
            int iret = this->db->setCallPair(caller_name, callee_phone, ipaddr, router_names);
            if (iret == 200) {
                // 应该能解决ws服务器与router服务器不一致的问题
                // 只返回一个应该使用的router，不要让客户端做任何判断决定功能。
                if (!router_names.contains(this->mSkype->handlerName())) {
                    qDebug()<<"Why database router list doesn't contains me???";
                }
                router_names.clear();
                router_names << this->mSkype->handlerName();
                resp = QString("200 OK\n%1\n").arg(router_names.count());
                for (int i = 0 ; i < router_names.count() ; ++i) {
                    resp += QString("%1\n").arg(router_names.at(i));
                }
            } else if (iret == 503) {
                resp = QString("503 Update database faild.\n");
            } else if (iret == 504) {
                resp = QString("504 Select database faild.\n");
            } else {
                resp = QString("501 Internal error.\n");
            }
            resp = QString("102$%1$%2").arg(caller_name).arg(resp);
            this->send_ws_command_102(caller_name, resp);
        } else {
            qDebug()<<"I am not a router, i don't want this command.";
        }

        break;

    case 105:  // dtfm from WSC
    case 107:  // hangup request from WSC, for non-ie explorer
        // proxy to real ws server
        if (fields.count() < 3 || fields.count() > 6) {
            qDebug()<<"Invalid ws command:"<<msg;
            return;
        }
        caller_name = fields.at(1).trimmed();
        callee_phone = fields.at(2).trimmed();
        // routers = Configs().getSkypeRouters(); // no use
        if (router_names.contains(this->mSkype->handlerName())) {
            this->on_ws_proxy_send_message(caller_name, msg);
        } else {
            
        }        
        break;
    default:
      qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Unknown or unsupported cmd:"<<msg;
      break;
    };
}

void SkyServ::onForwardWSMessage(QByteArray msg, qint64 cseq)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<msg;
    QList<QByteArray> fields = msg.split('$');
    QString caller_name;
    QString callee_phone;
    QStringList routers;
    int skype_call_id;

    Q_UNUSED(cseq);

    if (fields.count() <= 2) {
        qDebug()<<"Invalid wsc command:"<<msg;
        return;
    }

    switch (fields.at(0).trimmed().toInt()) {
    case 105:  // dtfm from WSC
        skype_call_id = this->mSkype->getCallIdByPartnerName(fields.at(1).trimmed());
        if (skype_call_id > 0) {
        } else {
            qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Cannot find call id for:"<<fields.at(1);
            skype_call_id = fields.at(3).trimmed().toInt();
        }
        this->onSkypeForwardCallDtmfArrived(skype_call_id, fields.at(1),
                                            this->mSkype->handlerName(), fields.at(3).trimmed());
        break;
    case 107:  // hangup request from WSC, for non-ie explorer
        skype_call_id = this->mSkype->getCallIdByPartnerName(fields.at(1).trimmed());
        if (skype_call_id > 0) {
        } else {
            qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Cannot find call id for:"<<fields.at(1);
            skype_call_id = fields.at(3).trimmed().toInt();
        }
        this->mSkype->setCallHangup(QString::number(skype_call_id));
        break;
    default:
      qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Unknown or unsupported cmd:"<<msg;
      break;
    };
}

// this->send_ws_command_100(callerName, gateway, QString("202.108.12.212"), ws_port);
bool SkyServ::send_ws_command_100(QString caller_name, QString gateway, QString host, unsigned short port)
{
    QString cmdline = QString("%1$%2$%3$%4$%5")
        .arg(100).arg(caller_name).arg(gateway).arg(host).arg(port);
    qint64 cseq = 0;
    int ilen = 0;

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_102(QString caller_name, QString data)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = data;

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = data;
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }
    

    return true;
}

bool SkyServ::send_ws_command_104(QString caller_name, QString gateway, int skype_call_id, QString msg)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;


    cmdline = QString("104$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(msg);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = QString("104$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(msg);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_106(QString caller_name, QString gateway, int skype_call_id, QString msg)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("106$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(msg);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = QString("106$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(msg);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_108(QString caller_name, QString gateway, int skype_call_id, int hangupcause, QString reason)
{

    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("108$%1$%2$%3$%4$%5").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(hangupcause).arg(reason);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = QString("108$%1$%2$%3$%4$%5").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(hangupcause).arg(reason);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_110(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("110$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = QString("110$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(reason);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_112(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("112$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;


    //     cmdline = QString("112$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(reason);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_114(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("114$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = QString("114$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(reason);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_116(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("116$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;


    //     cmdline = QString("116$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(reason);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_117(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("117$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);
    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = QString("117$%1$%2$%3$%4").arg(caller_name).arg(gateway)
    //         .arg(skype_call_id).arg(reason);
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_118(QString caller_name, QString gateway, int skype_call_id,
                                  QString sip_code, QString reason)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    cmdline = QString("118$%1$%2$%3$%4$%5").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason).arg(sip_code);

    return this->send_ws_command_common(caller_name, cmdline);

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }

    return true;
}

bool SkyServ::send_ws_command_common(QString caller_name, QString cmdstr)
{
    QString cmdline;
    qint64 cseq = 0;
    int ilen = 0;

    // boost::shared_ptr<call_meta_info> cmi;
    // if (!this->nWebSocketNameMap.leftContains(caller_name)) {
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name<<cmdstr;
    //     // cmi = this->find_call_meta_info_by_caller_name(caller_name);
    //     if (cmi == boost::shared_ptr<call_meta_info>()) {
    //         qLogx()<<"still can not find call map in all state map???";
    //     } else {
    //         cseq = cmi->conn_seq;
    //         if (cseq == -1) {
    //             qLogx()<<"found call meta info, but websocket handle is invalid -1";
    //         } else {
    //             cmdline = cmdstr;
    //             ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //             if (ilen) {}
    //         }
    //     }
    // } else {
    //     cmi = this->nWebSocketNameMap.findLeft(caller_name).value();
    //     cseq = cmi->conn_seq;

    //     cmdline = cmdstr;
    //     ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
    //     if (ilen) {}
    // }
    
    call_meta_info *cmi = this->find_call_meta_info_by_caller_name(caller_name.trimmed());
    if (cmi != NULL) {
        cseq = cmi->conn_seq;

        cmdline = cmdstr;
        ilen = this->scn_ws_serv2->wssend(cseq, cmdline.toAscii().data(), cmdline.length());
        if (ilen) {}
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name<<cmdstr;
    }
    
    return true;
}


void SkyServ::on_ws_proxy_connected(QString rpath)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;    

    WebSocketClient *wsc = (WebSocketClient*)(sender());
    boost::shared_ptr<WebSocketClient> belm = wsc->shared_from_this();
    // 下面如果要用的话，就用belm，不再使用裸指针。

    QStringList elems = rpath.right(rpath.length() - 1).split('/');
    if (elems.count() < 2) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"ws proxy invalid request:"<<rpath;
        // wsc->deleteLater();
        return;
    }
    QString handle_name = elems.at(0);
    QString phone = elems.at(1);

    // Q_ASSERT(this->nWebSocketProxyMap.leftContains(belm));
    call_meta_info *cmi = this->find_call_meta_info_by_ws_client(wsc);
    Q_ASSERT(cmi != NULL);
}

// 之前的代码，不能再把这个指针初始化一次，否则，就会出问题了。
// TODO, hash的优势没了。
// 并且使用了在shared_ptr中的原始指针，不安全
void SkyServ::on_ws_proxy_message(QByteArray msg)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<msg;    

    WebSocketClient *wsc = (WebSocketClient*)(sender());
    bool in_list = false;
    boost::shared_ptr<WebSocketClient> aelm;
    boost::shared_ptr<WebSocketClient> belm = wsc->shared_from_this();

    // if shared_from_this ok, this lambda is unneeded, keep performance nice
    auto tmp_contains_func = [&] (const boost::shared_ptr<WebSocketClient> &elm) {
        if (elm.get() == wsc) {
            in_list = true;
            aelm = elm;
        }
    };


    // Q_ASSERT(this->nWebSocketProxyMap.leftContains(belm));
    // boost::shared_ptr<call_meta_info> cmi = this->nWebSocketProxyMap.findLeft(belm).value();
    call_meta_info *cmi = this->find_call_meta_info_by_ws_client(wsc);
    Q_ASSERT(cmi != NULL);

    QString caller_name;

    qint64 cseq = cmi->conn_seq;
    // send to proxy's client
    int iret = this->scn_ws_serv2->wssend(cseq, msg.data(), msg.length());
    if (iret) {}
}

void SkyServ::on_ws_proxy_send_message(QString caller_name, QString msg)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<caller_name<<msg;

    boost::shared_ptr<WebSocketClient> wsc;
    // boost::shared_ptr<call_meta_info> cmi;
    call_meta_info *cmi = this->find_call_meta_info_by_caller_name(caller_name);

    if (cmi != NULL) {
        // boost::shared_ptr<WebSocketClient> wsc = this->wsProxy.findLeft(caller_name).value();
        // Q_ASSERT(wsc != NULL);
        wsc = cmi->ws_proxy;
        if (!wsc) {
            // 为什么会有这种情况，这是在什么时候关闭的呢，还是开始的时候就没有创建成功？
            qLogx()<<"PWS disappeared already."<<caller_name;
            Q_ASSERT(wsc);
        }

        // send to proxy's server
        int iret = wsc->sendMessage(msg.toAscii());
        if (iret) {}

    }
}

// boost::shared_ptr<call_meta_info> SkyServ::find_call_meta_info_by_caller_name(QString caller_name, int sip_call_id, int skype_call_id)
// {
//     boost::shared_ptr<call_meta_info> ci;
//     boost::shared_ptr<call_meta_info> tci;
//     bool item_found = false;

//     auto iter_b = this->nSkypeCallMap.leftBegin();
//     auto iter_e = this->nSkypeCallMap.leftEnd();

//     auto lamba_ff = [&iter_b, &iter_e, &ci, &caller_name] () -> bool {
//         bool item_found = false;

//         for (;iter_b != iter_e; iter_b++) {
//             ci = iter_b.value();
//             if (ci->caller_name == caller_name) {
//                 item_found = true;
//                 break;
//             }
//         }

//         return item_found;
//     };

//     // call like this, 但是由于参数类型不一致的问题，这还不能用。
//     // if (lamba_ff()) {
//     //     return ci;
//     // }

//     for (;iter_b != iter_e; iter_b++) {
//         tci = iter_b.value();
//         if ((!caller_name.isEmpty() && tci->caller_name == caller_name)
//             || (sip_call_id != -1 && tci->sip_call_id == sip_call_id)
//             || (skype_call_id != -1 && tci->skype_call_id == skype_call_id)) {
//             ci = tci;
//             item_found = true;
//             break;
//         }
//     }
//     if (item_found) {
//         return ci;
//     }

//     auto iter_b2 = this->nSipCallMap.leftBegin();
//     auto iter_e2 = this->nSipCallMap.leftEnd();
//     for (;iter_b2 != iter_e2; iter_b2++) {
//         tci = iter_b2.value();
//         if ((!caller_name.isEmpty() && tci->caller_name == caller_name)
//             || (sip_call_id != -1 && tci->sip_call_id == sip_call_id)
//             || (skype_call_id != -1 && tci->skype_call_id == skype_call_id)) {

//             ci = tci;
//             item_found = true;
//             break;
//         }
//     }
//     if (item_found) {
//         return ci;
//     }

//     auto iter_b3 = this->nWebSocketNameMap.leftBegin();
//     auto iter_e3 = this->nWebSocketNameMap.leftEnd();
//     for (;iter_b3 != iter_e3; iter_b3++) {
//         tci = iter_b3.value();
//         if ((!caller_name.isEmpty() && tci->caller_name == caller_name)
//             || (sip_call_id != -1 && tci->sip_call_id == sip_call_id)
//             || (skype_call_id != -1 && tci->skype_call_id == skype_call_id)) {

//             ci = tci;
//             item_found = true;
//             break;
//         }
//     }
//     if (item_found) {
//         return ci;
//     }

//     auto iter_b4 = this->nWebSocketSeqMap.leftBegin();
//     auto iter_e4 = this->nWebSocketSeqMap.leftEnd();
//     for (;iter_b4 != iter_e4; iter_b3++) {
//         tci = iter_b4.value();
//         if ((!caller_name.isEmpty() && tci->caller_name == caller_name)
//             || (sip_call_id != -1 && tci->sip_call_id == sip_call_id)
//             || (skype_call_id != -1 && tci->skype_call_id == skype_call_id)) {

//             ci = tci;
//             item_found = true;
//             break;
//         }
//     }
//     if (item_found) {
//         return ci;
//     }

//     auto iter_b5 = this->nWebSocketProxyMap.leftBegin();
//     auto iter_e5 = this->nWebSocketProxyMap.leftEnd();
//     for (;iter_b5 != iter_e5; iter_b5++) {
//         tci = iter_b5.value();
//         if ((!caller_name.isEmpty() && tci->caller_name == caller_name)
//             || (sip_call_id != -1 && tci->sip_call_id == sip_call_id)
//             || (skype_call_id != -1 && tci->skype_call_id == skype_call_id)) {

//             ci = tci;
//             item_found = true;
//             break;
//         }
//     }
//     if (item_found) {
//         return ci;
//     }

//     return ci;
// }

call_meta_info *SkyServ::find_call_meta_info_by_caller_name(QString caller_name)
{
    call_meta_info *cmd = NULL;

    this->mutex_ncmi.lock();

    if (this->ncmis.contains(caller_name)) {
        cmd = this->ncmis.value(caller_name);
    }

    this->mutex_ncmi.unlock();
    return cmd;
}

call_meta_info *SkyServ::find_call_meta_info_by_skype_call_id(int skype_call_id)
{
    call_meta_info *cmd = NULL;
    call_meta_info *tcmd = NULL;
    
    
    this->mutex_ncmi.lock();

    auto cmd_iter = this->ncmis.begin();
    for (;cmd_iter != this->ncmis.end(); cmd_iter++) {
        tcmd = cmd_iter.value();
        if (tcmd->skype_call_id == skype_call_id) {
            cmd = tcmd;
            break;
        }
    }

    this->mutex_ncmi.unlock();

    return cmd;

}

call_meta_info *SkyServ::find_call_meta_info_by_sip_call_id(int sip_call_id)
{
    call_meta_info *cmd = NULL;
    call_meta_info *tcmd = NULL;
        
    this->mutex_ncmi.lock();

    auto cmd_iter = this->ncmis.begin();
    for (;cmd_iter != this->ncmis.end(); cmd_iter++) {
        tcmd = cmd_iter.value();
        if (tcmd->sip_call_id == sip_call_id) {
            cmd = tcmd;
            break;
        }
    }

    this->mutex_ncmi.unlock();

    return cmd;

}

call_meta_info *SkyServ::find_call_meta_info_by_conn_seq(int conn_seq)
{
    call_meta_info *cmd = NULL;
    call_meta_info *tcmd = NULL;
        
    this->mutex_ncmi.lock();

    auto cmd_iter = this->ncmis.begin();
    for (;cmd_iter != this->ncmis.end(); cmd_iter++) {
        tcmd = cmd_iter.value();
        if (tcmd->conn_seq == conn_seq) {
            cmd = tcmd;
            break;
        }
    }

    this->mutex_ncmi.unlock();

    return cmd;

}

call_meta_info *SkyServ::find_call_meta_info_by_ws_client(WebSocketClient *ws_client)
{
    call_meta_info *cmd = NULL;
    call_meta_info *tcmd = NULL;
        
    this->mutex_ncmi.lock();

    auto cmd_iter = this->ncmis.begin();
    for (;cmd_iter != this->ncmis.end(); cmd_iter++) {
        tcmd = cmd_iter.value();
        if (tcmd->ws_proxy.get() == ws_client) {
            cmd = tcmd;
            break;
        }
    }

    this->mutex_ncmi.unlock();

    return cmd;

}

bool SkyServ::remove_call_meta_info(QString caller_name)
{
    call_meta_info *cmi = NULL;
    call_meta_info *tcmi = NULL;
        
    this->mutex_ncmi.lock();

    if (this->ncmis.contains(caller_name)) {
        cmi = this->ncmis.value(caller_name);
        this->ncmis.remove(caller_name);
        delete cmi;
        cmi = NULL;
    } else {
        qLogx()<<"not in cmis:"<<caller_name;
    }

    this->mutex_ncmi.unlock();

    return true;
}

bool SkyServ::add_call_meta_info(QString caller_name, call_meta_info *cmi)
{
    call_meta_info *tcmi = NULL;
        
    this->mutex_ncmi.lock();

    if (this->ncmis.contains(caller_name)) {
        cmi = this->ncmis.value(caller_name);
        qLogx()<<"already has cmi:"<<caller_name<<" remove it first:"<<cmi;
        this->ncmis.remove(caller_name);
        delete cmi;
        cmi = NULL;
    } else {
    }
    this->ncmis.insert(caller_name, cmi);

    this->mutex_ncmi.unlock();

    return true;
}

