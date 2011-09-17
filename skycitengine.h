// skycitengine.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-15 09:32:07 +0000
// Version: $Id: skycitengine.h 998 2011-09-17 11:03:58Z drswinghead $
// 

#ifndef _SKYCITENGINE_H_
#define _SKYCITENGINE_H_

#include <assert.h>
#include <boost/thread/thread.hpp>
// #include "boost/smart_ptr/enable_shared_from_this2.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/bimap.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>

#include <QtCore>

class Skycit;
class WebSocketServer3;

class SCBSignalBridge;

class SkycitEngine : public QThread
{
    Q_OBJECT;
protected:
    explicit SkycitEngine(QObject * parent = 0);
    static SkycitEngine *m_inst;
public:
    static SkycitEngine *instance();
    virtual ~SkycitEngine();

public slots:
    // void runin();
    void onSCEThreadStarted();

    // qt slot
    void onWS_websocket_error(int eno);
    void onWS_websocket_started();
    void onWS_new_websocket_connection();
    void onWS_websocket_message(const QString &msg, int cseq);
    void onWS_websocket_connection_closed(int cseq);
    // boost::signals2::signal<void(int)> sip_engine_error;
    // boost::signals2::signal<void()> sip_engine_started;

    bool process_ctrl_message(const std::string &msg, int cseq);

public:
    virtual void run();

protected slots:
    void onConnectSkype();
    void onConnectApp2App();

    void onSkypeNotFound();
    void onSkypeConnected(QString user_name);
    void onSkypeRealConnected(QString user_name);
    void onSkypeUserStatus(QString str_status, int int_status);
    void onSkypeError(int code, QString msg, QString cmd);

    void onSkypeCallArrived(QString callerName, QString calleeName, int callID);
    void onSkypeCallHangup(QString callerName, QString calleeName, int callID);

signals:
    void runin_it();

protected:
    Skycit *sct;
    SCBSignalBridge *sigbr;

    QThread *old_thread;

    WebSocketServer3 *mws;

    class SessionUnit {
    public:
        SessionUnit();
        ~SessionUnit();

        // libwebsocket *wsi;
        int cseq;

        int acc_id;
        int call_id;
        
        std::string mc_str;
        // CmdMakeCall cmd_mc;

        std::string reg_str;
        // CmdRegister cmd_reg;
    };

    // 只接受一个ws客户端。
    SessionUnit su1;
    
    std::vector<SessionUnit*> scs;

protected:
};

class SCBSignalBridge : public QThread
{
    Q_OBJECT;
public:
    explicit SCBSignalBridge(QObject *parent = 0);
    virtual ~SCBSignalBridge();

    // virtual void run();

    // boost slot
    void on_websocket_error(int eno);
    void on_websocket_started();
    void on_new_websocket_connection();
    void on_websocket_message(const std::string &msg, int cseq);
    void on_websocket_connection_closed(int cseq);

signals:
    void websocket_error(int eno);
    void websocket_started();
    void new_websocket_connection();
    void websocket_message(const QString &msg, int cseq);
    void websocket_connection_closed(int cseq);

};


///////////////////

#endif /* _SKYCITENGINE_H_ */
