// skypephone.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-18 21:23:19 +0800
// Version: $Id: skypephone.h 854 2011-04-26 15:08:58Z drswinghead $
// 

#ifndef _SKYPEPHONE_H_
#define _SKYPEPHONE_H_

#include <QtCore>
#include <QtNetwork>
#include <QtGui>

#include "boost/shared_ptr.hpp"

class Skype;
class SkypeTunnel;
class SkypeTracer;

class WebSocketClient;
class AsyncDatabase;

namespace Ui {
    class SkypePhone;
};

class SkypePhone : public QWidget
{
    Q_OBJECT;
public:
    explicit SkypePhone(QWidget *parent = 0);
    virtual ~SkypePhone();

public slots:    // pstn
    void defaultPstnInit();
    void onInitPstnClient();
    void onCallPstn();
    void onHangupPstn();
    void onShowSkypeTracer();

    void onAddContact();

    void onConnectSkype();
    void onConnectApp2App();

    void onSkypeConnected(QString user_name);
    void onSkypeRealConnected(QString user_name);

    void onSkypeUserStatus(QString str_status, int int_status);
    void onSkypeCallArrived(QString callerName, QString calleeName, int callID);

    void onWSConnected(QString path);
    void onWSError(int error, const QString &errmsg);
    void onWSDisconnected();
    void onWSMessage(QByteArray msg);

    void onCalcWSServByNetworkType(QHostInfo hi);

    void log_output(int type, const QString &log);

public:
    enum LOGTYPE {
        LT_USER = 0,
        LT_DEBUG = 1
    };

protected:
    bool first_paint_event;
    virtual void 	paintEvent ( QPaintEvent * event );
    virtual void 	showEvent ( QShowEvent * event );

private: // pstn
    Skype *mSkype;
    SkypeTunnel *mtun;
    SkypeTracer *mSkypeTracer;

    AsyncDatabase *m_adb;
    int m_curr_skype_call_id;
    QString m_curr_skype_call_peer;
    boost::shared_ptr<WebSocketClient> wscli;
    
    #ifdef WIN32
    static const int m_conn_ws_max_retry_times;
    #else
    static constexpr int m_conn_ws_max_retry_times = 3;
    #endif
    int m_conn_ws_retry_times;
    QString m_ws_serv_ipaddr;

private:
    Ui::SkypePhone *uiw;
};

#endif /* _SKYPEPHONE_H_ */
