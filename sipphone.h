// sipphone.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-18 21:30:38 +0800
// Version: $Id: sipphone.h 869 2011-05-07 09:41:17Z drswinghead $
// 

#ifndef _SIPPHONE_H_
#define _SIPPHONE_H_

#include <QtCore>
#include <QtNetwork>
#include <QtGui>

#include "boost/signals2.hpp"
#include "boost/smart_ptr.hpp"

extern "C" {
	#include <pjlib.h>
	#include <pjlib-util.h>
	#include <pjmedia.h>
	#include <pjmedia-codec.h>
	#include <pjsip.h>
	#include <pjsip_simple.h>
	#include <pjsip_ua.h>
	#include <pjsua-lib/pjsua.h>
}

#define VOIP_SERVER "voip.qtchina.net"
#define SIP_SERVER "sip.qtchina.net"
// #define TURN_SERVER "turn.qtchina.net"
#define TURN_SERVER "turn.qtchina.net"
#define TURN_PORT "34780"

class SipAccountList;
class PjsipCallFront;
class AsyncDatabase;

namespace Ui {
    class SipPhone;
};

// 还要让它处理异步来自主界面线程的sip调用
class PJSipEventThread : public QThread
{
    Q_OBJECT;
public: 
    explicit PJSipEventThread(QObject *parent = 0);
    virtual ~PJSipEventThread();
    
    void run();

    void dump_info(pj_thread_t *thread);

private:
    bool quit_loop;
};

class SipPhone : public QWidget
{
    Q_OBJECT;
public:
    explicit SipPhone(QWidget *parent = 0);
    virtual ~SipPhone();

public slots: // sip
    void defaultSipInit();
    void on3_invoker_started(pj_status_t status);

    // ui
    void main_ui_draw_complete();

    void init_sip_client_ui_element();
    void onManageSipAccounts();
    void onRegisterAccount(QString user_name, bool reg);
    void onRemoveAccount(QString user_name);
    void onCallSip();
    void onHangupSip();
    
    void onCallSipNew();
    void onHangupSipNew();

    void onSelectedUserAccountChanged(int idx);
    void onDigitButtonClicked();
    
    // sip
    void on1_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci);
    void on1_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci);
    void on1_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);

    // void on1_new_connection(void *m_port);

    // void on1_put_frame(QTcpSocket *sock, QByteArray fba);
    
    void on2_pjsua_start_done(int seqno, pj_status_t status);
    void on2_make_call_done(int seqno, pj_status_t status, pjsua_call_id call_id);

    void log_output(int type, const QString &log);

private slots:
    void onFCMakeCallFinished();
    void set_custom_sip_config();

public:
    enum LOGTYPE {
        LT_USER = 0,
        LT_DEBUG = 1
    };

private:
    pjsua_acc_id _create_sip_account(QString acc_name);
    QString _get_sip_from_domain();
    int _find_account_from_pjacc(QString acc_name);

protected:
    bool first_paint_event;
    virtual void 	paintEvent ( QPaintEvent * event );
    virtual void 	showEvent ( QShowEvent * event );

private: // sip
    SipAccountList *acc_list;
    pjsua_config m_ua_cfg;
    pjsua_logging_config m_log_cfg;
    pjsua_media_config m_media_cfg;
    pjsua_transport_config m_tcp_tp_cfg;
    pjsua_transport_config m_udp_tp_cfg;
    pjsua_transport_config m_ipv6_tp_cfg;
    pjsua_transport_config m_tls_tp_cfg;
    pjsua_transport_config m_tcp6_tp_cfg;
    pjsua_transport_config m_udp6_tp_cfg;

    pjsua_transport_id udp_tpid;
    pjsua_transport_id tcp_tpid;
    pjsua_transport_id tls_tpid;
    pjsua_transport_id ipv6_tpid;
    pjsua_transport_id udp6_tpid;
    pjsua_transport_id tcp6_tpid;
    pjsua_transport_id user_tpid;
    pjsua_transport_id sctp_tpid;

    PjsipCallFront *m_invoker;
    pjsua_call_id m_curr_call_id;
private:
    Ui::SipPhone *uiw;
};

#endif /* _SIPPHONE_H_ */
