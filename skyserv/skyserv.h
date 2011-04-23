// skyserv.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-03 15:35:54 +0800
// Version: $Id: skyserv.h 847 2011-04-23 04:46:25Z drswinghead $
// 

#ifndef SKYSERV_H
#define SKYSERV_H

#include <QMainWindow>
#include <QtCore>
#include <QTcpServer>
#include <QTcpSocket>

#include <atomic>

#include "../libng/qbihash.h"

#include <tr1/memory>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

#define  SSCM_WAIT_SIP_CALL_ID -888888 
#define  SSCM_SKYPE_HANGUP_WHEN_WAIT_SIP_CALL_ID -666666 
#define  SSCM_HANGUP_FROM_SIP -555555 

class SkypePackage;
class Skype;
class Database;
class WebSocketServer;
class WebSocketClient;
class WebSocketServer2;
class LimitDetector;
class NLBridge;

// 统一的呼叫元数据类型，管理呼叫的进度、状态如何。
// 用于switcher和router两种模式，状态机有所不同。
enum class CallState {
    CS_NONE = 0,
        CS_CALL_ARRIVED,
        CS_NO_CALL_PAIR,
        CS_CALL_TRANSFERED,
        CS_CALL_MISSED,
        CS_CALL_REFUSED,
        CS_CALL_FINISHED,
        CS_CALL_ANSWERED,
        CS_WS_CONNECTED,

        CS_ROUTER_CONNECTING_SWITCHER,
        CS_ROUTER_CONNECTED_SWITCHER,
        CS_ROUTER_ALL_LINE_BUSY,
        CS_ROUTER_END,

        CS_SWITCHER_SIP_HANGUP,
        CS_SWITCHER_END,
        CS_MAX
        };
class call_meta_info : public boost::enable_shared_from_this<call_meta_info> {
public:
    call_meta_info() {
        this->skype_call_id = -1;
        this->sip_call_id = -1;
        this->conn_seq = -1;
        this->call_state = CallState::CS_NONE;
        this->ctime = this->mtime = QDateTime::currentDateTime();
    }
    virtual ~call_meta_info() {

    }
    QString callee_name;
    QString caller_name;
    QString callee_phone;
    QString switcher_name;    // 对于router,需要知道把这个呼叫转到哪个switcher上了。
    int skype_call_id;
    int sip_call_id;
    qint64 conn_seq; // 替代裸指针 libwebsocket *,由于会客户端先触发关闭，所以收到关闭消息再处理不迟。
    boost::shared_ptr<WebSocketClient> ws_proxy;
    enum CallState call_state;
    
    // for debug
    QDateTime ctime;  // 创建时间
    QDateTime mtime;  // 修改时间
    QDateTime etime;  // 结束时间，这个可能没用吧。
    QVector<boost::tuple<QDateTime,QString,int,int> > cmd_list; // int:SEND/RECV, int:RPC/WSS/WSP
    // QVector<boost::tuple<QDateTime,QString,int> > ws_cmd_list;
    // QVector<boost::tuple<QDateTime,QString,int> > ws_proxy_cmd_list; // int FROM_CLIENT/FROM_BACKEND
};

// for KBiHash<QString, boost:shared_ptr<WebSocketClient> > type
inline uint qHash(const boost::shared_ptr<WebSocketClient> &key)
{
    return qHash(key.get());
}

inline uint qHash(const boost::shared_ptr<call_meta_info> &key)
{
    return qHash(key->caller_name);
    // return qHash(QString::number(key->skype_call_id) + key->caller_name);
    // return qHash(key->skype_call_id);
}

////////////////////
//////
////////////////////////
class SkyServ : public QObject
{
    Q_OBJECT;
public:
    SkyServ(QObject *parent = 0);
    virtual ~SkyServ();

    // 处理unix信号
public:
    void init_unix_signal_handlers();

    // http://doc.qt.nokia.com/4.7-snapshot/unix-signals.html
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
    static void intSignalHandler(int unused);
    static void quitSignalHandler(int unused);

    static int sighupFd[2];
    static int sigtermFd[2];
    static int sigintFd[2];
    static int sigquitFd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;
    QSocketNotifier *snInt;
    QSocketNotifier *snQuit;

public slots:
    // app
    void handleSigHup();
    void handleSigTerm();
    void handleSigInt();
    void handleSigQuit();
    void on_app_want_quit();

    // from skype
    void onSkypeRawMessage(QString skypeName, QString msg);
    void onSkypeError(int errNo, QString msg, QString cmd);
    void onSkypeConnected(QString skypeName);
    void onSkypeRealConnected(QString skypeName);
    void onSkypeDisconnected(QString skypeName);
    void onNewStreamCreated(QString contactName, int stream);
    void onSkypePackageArrived(QString contactName, int stream, QString data);
    void onNewRouteCallArrived(QString callerName, QString calleeName, QString calleePhone, int skypeCallID);
    void onRouteCallTransferred(int skypeCallID, QString callerName, QString calleeName, QString lastStatus);
    void onRouteCallMissed(int skypeCallID, QString callerName, QString calleeName);
    void onRouteCallRefused(int skypeCallID, QString callerName, QString calleeName);
    void onNewForwardCallArrived(QString callerName, QString calleeName, int skypeCallID);
    void onSkypeForwardCallAnswered(int skypeCallID, QString callerName, QString calleeName);
    void onSkypeForwardCallHold(int skypeCallID, QString callerName, QString calleeName);
    void onSkypeForwardCallUnhold(int skypeCallID, QString callerName, QString calleeName);
    void onSkypeForwardCallDtmfArrived(int skypeCallID, QString callerName, QString calleeName, QString dtmf);
    // void onSkypeCallParticipantArrived(int skypeCallID, QString callerName,
    //                                    QString callee_name, QString participant);
    // void onSkypeForwardCallPstnArrived(int skypeCallID, QString callerName,
    //                                    QString callee_name, QString pstn);
    void onSkypeCallHangup(QString contactName, QString calleeName, int skypeCallID);

    void processRequest(QString contactName, int stream, SkypePackage *sp);

    // from sip
    void onSipCallFinished(int sip_call_id, int status_code, int skype_call_id);
    void onSipCallExceedMaxCount(int skype_call_id);
    void onSipCallOutgoingMediaServerReady(int sip_call_id, unsigned short port, int skype_call_id);
    void onSipCallIncomingMediaServerReady(int sip_call_id, unsigned short port, int skype_call_id);

    void onProcessIncomingRpcCommand(int cmdlen, int comno, char *cmdbuf);

    // from websock
    void onNewWSConnection();
    // void onNewWSConnection(QString path, QTcpSocket *sock);
    // void onRouterWSMessage(QByteArray msg, QTcpSocket *sock);
    // void onForwardWSMessage(QByteArray msg, QTcpSocket *sock);
    void onNewWSConnection(QString path, qint64 conn_seq);
    void onWSConnectionClosed(qint64 conn_seq);
    void onRouterWSMessage(QByteArray msg, qint64 conn_seq);
    void onForwardWSMessage(QByteArray msg, qint64 conn_seq);

public slots:
    void onLimitationExceptionDetected(int r);

private slots:
    void onReconnSkypeTimeout();
    void clear_skype_history();
    void clear_skype_history_sched();

    //////////
    virtual void onIncomingRpcConnection();
    void onRpcServReadyRead();
    void onRpcServAboutToClose();

    bool init_ws_serv(QString handle_name);
    bool send_ws_command_100(QString caller_name, QString gateway, QString host, unsigned short port);
    bool send_ws_command_102(QString caller_name, QString data);
    bool send_ws_command_104(QString caller_name, QString gateway, int skype_call_id, QString msg);
    bool send_ws_command_106(QString caller_name, QString gateway, int skype_call_id, QString msg);
    bool send_ws_command_108(QString caller_name, QString gateway, int skype_call_id, int hangupcause, QString reason);
    bool send_ws_command_110(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_112(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_114(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_116(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_117(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_118(QString caller_name, QString gateway, int skype_call_id, 
                             QString sip_code, QString reason);

    // 
    void on_ws_proxy_connected(QString rpath);
    void on_ws_proxy_message(QByteArray msg);
    void on_ws_proxy_send_message(QString caller_name, QString msg);

private:
    boost::shared_ptr<call_meta_info> find_call_meta_info_by_caller_name(QString caller_name);

private:
    QDateTime start_time; // 启动时间
    Database *db;
    boost::shared_ptr<Database> smt_db;
    Skype *mSkype;
    boost::shared_ptr<Skype> smt_mSkype;
    // QTimer *reconnSkypeTimer;
    boost::shared_ptr<QTimer> reconnSkypeTimer;
    bool first_connected;
    int first_reconnect_times;
    QMap<QString, QString> ccMap; // caller -> callee, caller -> gateway // depcreated
    // QHash<int, QString> activeGateways; // skype call id -> gatewary
    
    // KBiHash<int, int> mSkypeSipCallMap; // skype call id -> sip call id
    // // QHash<QString, QTcpSocket *> wsMap;// caller -> web socket client connection
    // KBiHash<QString, qint64> wsMap; // caller -> web socket client connection seq
    // 应该是每次都会使用新连接，path不同的问题。
    // // KBiHash<QString, WebSocketClient *> wsProxy; // forwarder -> another ws server
    // KBiHash<QString, boost::shared_ptr<WebSocketClient> > wsProxy; // forwarder -> another ws server

    // 新的处理呼叫元信息的变量。
    /*
      skype_call_id      sip_call_id     
               call_meta_info      <---------- ws_prox
      caller_name         conn_seq      

      几个key都指向同一个call_meta_info
      这个call_meta_info最先创建的位置，对于router是在新的ws创建时
      对于switcher也是在ws创建时。
      switcher中不会用到proxy成员
      router中不会用到sipcall成员，
      此call_meta_info的销毁，对于router，必须是在proxy关闭时，或者中间出错没有呼叫到switcher而中断时
      对于switcher,其是在任何出错，结束时销毁。一般会收到skype hangup,在这关闭
     */
    KBiHash<int, boost::shared_ptr<call_meta_info> > nSkypeCallMap;  // skype call id -> meta info
    KBiHash<int, boost::shared_ptr<call_meta_info> > nSipCallMap; // sip call id -> meta info
    KBiHash<QString, boost::shared_ptr<call_meta_info> > nWebSocketNameMap;  // caller name -> meta info 
    KBiHash<qint64, boost::shared_ptr<call_meta_info> > nWebSocketSeqMap; // ws conn seq -> meta info
    // KBiHash<QString, boost::shared_ptr<call_meta_info> > nWebSocketProxyMap; // caller name -> meta info // 与上一个重复
    KBiHash<boost::shared_ptr<WebSocketClient>, boost::shared_ptr<call_meta_info> > nWebSocketProxyMap; // ws ->meta info
    
    // next next way, 再下一种试用的方法
    boost::tuple<QString, QString, QString, int, int, qint64, boost::shared_ptr<WebSocketClient> > call_meta_info_b;

    // 
    // WebSocketServer *scn_ws_serv; // skype call notice websocket server
    // test, but it works fine
    QTcpServer *skype_sip_rpc_serv;
    QTcpSocket *skype_sip_rpc_peer;

    pthread_t sip_main_thread;
    // bool quit_cleaning; // 退出清理过程，不再处理新的请求，如果有的话。
    std::atomic<bool> quit_cleaning;

    WebSocketServer2 *scn_ws_serv2; // skype call notice websocket server

    LimitDetector *lder;
    // want it to be powerful a logic robot
    NLBridge *nlbr;
};

#endif // SKYSERV_H
