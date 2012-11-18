// sip_vars.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-12 21:19:11 +0800
// Version: $Id: sip_entry.h 609 2010-12-06 07:24:32Z drswinghead $
// 

#ifndef _SIP_VARS_H_
#define _SIP_VARS_H_

#include <QtCore>
#include <QTcpSocket>
#include <QtGui/QApplication>

#include "../libng/qbihash.h"

#include <pjsip.h>
#include <pjsua.h>
#include <pjmedia/wave.h>
#include <pjmedia/wav_port.h>

class SipVarSet : public QThread
{
    Q_OBJECT;
public:
    SipVarSet(QObject *parent = 0);
    virtual ~SipVarSet();

    void run();
    void run1();

public:
    void *callbackObject;
    pjsua_recorder_id rec_id;
    // pjsua_call_id curr_call_id;
    bool use_tcp_transport;

    pjsua_config         ua_cfg;
    pjsua_logging_config log_cfg;
    pjsua_media_config   media_cfg;

    pjsua_transport_config tp_cfg;

    pjsua_acc_config acc_cfg;

    pjsua_call_info ci;
    pjsua_conf_port_info cpi;

    KBiHash<int, int> mSkypeSipCallMap; // skype call id -> sip call id

    KBiHash<QString, void *> transportPool;

    // int eph;
    // bool quit_thread;

public:
    void dump_pjthread_info(pj_thread_t *thread);
private:
    bool quit_pjthread_loop;

public:
    pj_status_t sip_app_init();
    // serv_addr=host:port
    pj_status_t call_phone(QString caller_name, QString phone_number, QString serv_addr, int skypeCallID);

public slots:
    // sip
    void on1_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci);
    void on1_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci);
    void on1_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);

    void on1_user_added(int skype_call_id, pjsua_call_id sip_call_id, int tid);
    void on1_exceed_max_call_count(int skype_call_id);

    void on1_new_connection(void *m_port);
    void on1_new_incoming_connection(void *m_port);

    void on1_put_frame(QTcpSocket *sock, QByteArray fba);
    // skype
    void hangup_call(int callID);
    void hangup_all_call();

private:
    void registerSipTypes();
    int _find_account_from_pjacc(QString acc_name);
    int _delete_account_by_active(int cnt);

signals:
    // sip
    void sip_call_finished(int sip_call_id, int reason);
    void sip_call_exceed_max_count(int skype_call_id);
    void sip_call_media_server_ready(int sip_call_id, unsigned short port);
    void sip_call_incoming_media_server_ready(int sip_call_id, unsigned short port);
};

/* global callback/logger object */
extern void *globalPjCallback;

#endif /* _SIP_VARS_H_ */
