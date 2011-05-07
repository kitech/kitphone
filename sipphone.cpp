// kitsip.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-20 17:23:07 +0800
// Version: $Id: sipphone.cpp 869 2011-05-07 09:41:17Z drswinghead $
// 

#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <QtCore>
#include <QtNetwork>

#include "pjsip.h"
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include "pjsua.h"
#include <pjmedia/wave.h>
#include <pjmedia/wav_port.h>

#include "simplelog.h"
#include "ui_sipphone.h"
#include "sipphone.h"

#include "PjCallback.h"

#include "sipaccount.h"
#include "sipaccountswindow.h"

/* global callback/logger object */
extern void *globalPjCallback;

#define SIP_DOMAIN      "example.com"
// #define SIP_USER        "alice"
#define SIP_USER        "liuguangzhao"
#define SIP_PASSWD      "secret"

SipPhone::SipPhone(QWidget *parent)
    : QWidget(parent)
    ,uiw(new Ui::SipPhone())
{
    this->uiw->setupUi(this);

    this->first_paint_event = true;
    this->acc_list = NULL;

    QObject::connect(this->uiw->pushButton_5, SIGNAL(clicked()), 
                     this, SLOT(onManageSipAccounts()));

    this->defaultSipInit();
}

SipPhone::~SipPhone()
{
    delete this->acc_list;
    delete uiw;
}

void SipPhone::main_ui_draw_complete()
{
    qDebug()<<"draw complete, load dynamic data now";
    this->acc_list = SipAccountList::instance();

    QString user_account;
    QVector<SipAccount> accs = this->acc_list->loadAccounts();
    for (int i = accs.count()-1; i >= 0; i--) {
        user_account = QString("%1@%2").arg(accs.at(i).userName).arg(accs.at(i).domain);
        this->uiw->comboBox_6->insertItem(0, user_account);
    }

    this->uiw->comboBox_6->view()->setFixedWidth(300);
}

void SipPhone::paintEvent ( QPaintEvent * event )
{
    QWidget::paintEvent(event);
    // qDebug()<<"parintttttt"<<event<<event->type();
    if (this->first_paint_event) {
        this->first_paint_event = false;
		QTimer::singleShot(50, this, SLOT(main_ui_draw_complete()));
    }
}
void SipPhone::showEvent ( QShowEvent * event )
{
    QWidget::showEvent(event);
    // qDebug()<<"showwwwwwwwwwwww"<<event<<event->type();
}

// /* Callback called by the library upon receiving incoming call */
// static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
//                              pjsip_rx_data *rdata)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     pjsua_call_info ci;
 
//     PJ_UNUSED_ARG(acc_id);
//     PJ_UNUSED_ARG(rdata);
 
//     pjsua_call_get_info(call_id, &ci);
 
//     PJ_LOG(3,(__FILE__, "Incoming call from %.*s!!",
//               (int)ci.remote_info.slen,
//               ci.remote_info.ptr));
 
//     /* Automatically answer incoming calls with 200/OK */
//     pjsua_call_answer(call_id, 200, NULL, NULL);
// }

// /* Callback called by the library when call's state has changed */
// static void on1_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     pjsua_call_info ci;
 
//     PJ_UNUSED_ARG(e);
 
//     pjsua_call_get_info(call_id, &ci);
//     PJ_LOG(3, (__FILE__, "Call %d state=%.*s", call_id,
//               (int)ci.state_text.slen,
//               ci.state_text.ptr));
// }

// /* Callback called by the library when call's media state has changed */
// static void on1_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     pjsua_call_info ci;
//     pj_status_t status;
 
//     pjsua_call_get_info(call_id, &ci);
 
//     if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
//         // When media is active, connect call to sound device.
//         // pjsua_conf_connect(ci.conf_slot, 0);
//         pjsua_conf_connect(0, ci.conf_slot);

//         // test port_info
//         pjsua_conf_port_info cpi;
//         pjsua_conf_get_port_info(ci.conf_slot, &cpi);
//         qDebug()<<"conf port info: port number="<<cpi.slot_id<<",name='"<<cpi.name.ptr<<"', chan cnt="<<cpi.channel_count
//                 <<", clock rate="<<cpi.clock_rate;

//         pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(g_rec_id));

//         pjmedia_transport *mtp = pjsua_call_get_media_transport(call_id);
//         pjmedia_session *ms = pjsua_call_get_media_session(call_id);
//         pjmedia_port *mp = NULL;
//         unsigned int stream_count = 100;
//         pjmedia_stream_info msi[100];
//         pjmedia_frame from_lang_frame;
//         int i = 0;
//         unsigned short n_port = 0;

//         // pjsua_switcher_get_port(g_rec_id, &mp);
//         // n_port = pjsua_switcher_get_net_port(mp);
//         qDebug()<<"got wav server port :"<<n_port;
//         // wc->setPort(n_port);
//         // wc->arun();
        
//         if (ms == NULL) {
//             qDebug()<<"pjmedia_session is null.\n";
//         } else {
//             status = pjmedia_session_enum_streams(ms, &stream_count, msi);
//             qDebug()<<"enum stream count:"<<stream_count;
            
//             status = pjmedia_session_get_port(ms, 0, &mp);
//             Q_ASSERT(status == PJ_SUCCESS);

//             QFile wfp("/tmp/arec.wav");
//             if (!wfp.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) {
//                 exit(-1);
//             }
//             // why kitphone: ../src/pjmedia/plc_common.c：106：pjmedia_plc_generate: 断言“plc && frame”失败。
//             // while ((status = pjmedia_port_get_frame(mp, &from_lang_frame)) == PJ_SUCCESS
//             //        && i ++ < 10000) {
//             //     if (from_lang_frame.size > 0) {
//             //         wfp.write((char*)(from_lang_frame.buf), from_lang_frame.size);
//             //     } else {
//             //         qDebug()<<"got 0 frame. stop.";
//             //         break;
//             //     }
//             // }
//             wfp.close();
            
//         }
//     }

//     // pjsua_dump(PJ_TRUE);
//     // pjsua_call_dump(call_id, ...);
// }

// 
// pj_thread_desc g_td;
// static pj_status_t app_init(void)
// {
//    pjsua_config         ua_cfg;
//    pjsua_logging_config log_cfg;
//    pjsua_media_config   media_cfg;
//    pj_status_t status;

//    // Must create pjsua before anything else!
//    status = pjsua_create();
//    if (status != PJ_SUCCESS) {
//        pjsua_perror(__FILE__, "Error initializing pjsua", status);
//        return status;
//    }

//    // Initialize configs with default settings.
//    pjsua_config_default(&ua_cfg);
//    pjsua_logging_config_default(&log_cfg);
//    pjsua_media_config_default(&media_cfg);

//    // ua_cfg.thread_cnt = 0;
//    // At the very least, application would want to override
//    // the call callbacks in pjsua_config:
//    // ua_cfg.cb.on_incoming_call = ...
//    // ua_cfg.cb.on_call_state = ..
//    // ua_cfg.cb.on_incoming_call = &on_incoming_call;
//    // ua_cfg.cb.on_call_state = &on_call_state;
//    // ua_cfg.cb.on_call_media_state = &on_call_media_state;
//    ua_cfg.cb.on_incoming_call = PjCallback::on_incoming_call_wrapper;
//    ua_cfg.cb.on_call_state = PjCallback::on_call_state_wrapper;
//    ua_cfg.cb.on_call_media_state = PjCallback::on_call_media_state_wrapper;
//    ua_cfg.nat_type_in_sdp = 1;

//    media_cfg.snd_auto_close_time = 1;
//    // Customize other settings (or initialize them from application specific
//    // configuration file):

//    // Initialize pjsua
//    status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
//    if (status != PJ_SUCCESS) {
//          pjsua_perror(__FILE__, "Error initializing pjsua", status);
//          return status;
//    }
//    pjsua_var.mconf_cfg.samples_per_frame = 8000;

//    /* Add UDP transport. */
//    {
//        pjsua_transport_config cfg;

//        pjsua_transport_config_default(&cfg);
//        cfg.port = 15678; // if not set , use random big port 
//        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
//        if (status != PJ_SUCCESS)
//            pjsua_perror(__FILE__, "Error creating transport", status);
//        // error_exit("Error creating transport", status);
//    }

//    /* Initialization is done, now start pjsua */
//    status = pjsua_start();
//    if (status != PJ_SUCCESS) {
//        pjsua_perror(__FILE__, "Error starting pjsua", status);
//    }
//    // error_exit("Error starting pjsua", status);

//    pjsua_set_null_snd_dev();

//    pjsua_recorder_id rec_id;                   
//    char *rec_file_name = "/tmp/abcd.wav";
//    pj_str_t pj_rec_file_name = pj_str(rec_file_name);
//    unsigned short n_port = 0;
//    // status = pjsua_recorder_create(&pj_rec_file_name, 0, NULL, -1, 0, &rec_id);
//    status = pjsua_switcher_create(SUA_SWITCHER_SERVER, &n_port, 0, NULL, 0, &rec_id);
//    qDebug()<<"record to :"<<rec_id<<n_port;
//    g_rec_id = rec_id;

//    return status;
// }


void init_sip_client () {
    // int qid_pjsua_call_id = qRegisterMetaType<pjsua_call_id>("pjsua_call_id");
    // int qid_pjsua_acc_id = qRegisterMetaType<pjsua_acc_id>("pjsua_acc_id");

    // globalPjCallback = new PjCallback();
    // PjCallback *pjcb = (PjCallback *)globalPjCallback;
	// QObject::connect(pjcb, SIGNAL(sig_call_state(pjsua_call_id, pjsip_event *)),
    //                  this, SLOT(on1_call_state(pjsua_call_id, pjsip_event *)),
    //                  Qt::QueuedConnection);
	// QObject::connect(pjcb, SIGNAL(sig_call_media_state(pjsua_call_id)),
    //                  this, SLOT(on1_call_media_state(pjsua_call_id)),
    //                  Qt::QueuedConnection);
	// QObject::connect(pjcb, SIGNAL(sig_incoming_call(pjsua_acc_id, pjsua_call_id, pjsip_rx_data *)),
    //                  this, SLOT(on1_incoming_call(pjsua_acc_id, pjsua_call_id, pjsip_rx_data *)),
    //                  Qt::QueuedConnection);

	// QObject::connect(pjcb, SIGNAL(sig_new_connection(void *)),
    //                  this, SLOT(on1_new_connection(void *)),
    //                  Qt::QueuedConnection);

	// QObject::connect(pjcb, SIGNAL(sig_put_frame(QTcpSocket *, QByteArray)),
    //                  this, SLOT(on1_put_frame(QTcpSocket *, QByteArray)),
    //                  Qt::QueuedConnection);

    // wc = new WavClient(0);
    // app_init();
    // QObject::connect(this->ui->pushButton, SIGNAL(clicked()), this, SLOT(onCall()));
    // QObject::connect(this->ui->pushButton, SIGNAL(clicked()), this, SLOT(onCallSip()));
    // QObject::connect(this->ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onHangup()));
    // QObject::connect(this->ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onHangupSip()));
}

void SipPhone::defaultSipInit()
{
    // should do sth. here
    int qid_pjsua_call_id = qRegisterMetaType<pjsua_call_id>("pjsua_call_id");
    int qid_pjsua_acc_id = qRegisterMetaType<pjsua_acc_id>("pjsua_acc_id");
    int qid_pjsua_transport_id = qRegisterMetaType<pjsua_transport_id>("pjsua_transport_id");
    Q_UNUSED(qid_pjsua_acc_id);
    Q_UNUSED(qid_pjsua_call_id);
    Q_UNUSED(qid_pjsua_transport_id);

   pjsua_config         ua_cfg;
   pjsua_logging_config log_cfg;
   pjsua_media_config   media_cfg;
   pj_status_t status;

   // Must create pjsua before anything else!
   status = pjsua_create();
   if (status != PJ_SUCCESS) {
       pjsua_perror(__FILE__, "Error initializing pjsua", status);
       return;
   }

   PJSipEventThread *thread = new PJSipEventThread();
   thread->start();
   // TODO should detect if thread start successful, if not, below operation is not usable

   // 
   this->init_sip_client_ui_element();

   // Initialize configs with default settings.
   pjsua_config_default(&ua_cfg);
   pjsua_logging_config_default(&log_cfg);
   pjsua_media_config_default(&media_cfg);

   // ua_cfg.thread_cnt = 0;
   // At the very least, application would want to override
   // the call callbacks in pjsua_config:
   // ua_cfg.cb.on_incoming_call = ...
   // ua_cfg.cb.on_call_state = ..
   // ua_cfg.cb.on_incoming_call = &on_incoming_call;
   // ua_cfg.cb.on_call_state = &on_call_state;
   // ua_cfg.cb.on_call_media_state = &on_call_media_state;
   ua_cfg.cb.on_incoming_call = PjCallback::on_incoming_call_wrapper;
   ua_cfg.cb.on_call_state = PjCallback::on_call_state_wrapper;
   ua_cfg.cb.on_call_media_state = PjCallback::on_call_media_state_wrapper;
   ua_cfg.nat_type_in_sdp = 1;

   ua_cfg.cb.on_nat_detect = PjCallback::on_nat_detect_wrapper;

   ua_cfg.stun_host = pj_str(TURN_SERVER ":" TURN_PORT);
   ua_cfg.stun_srv[ua_cfg.stun_srv_cnt++] = pj_str(TURN_SERVER ":" TURN_PORT);

   /////// media config 
   media_cfg.snd_auto_close_time = 1;
   // Customize other settings (or initialize them from application specific
   // configuration file):

   // media_cfg.enable_turn = PJ_TRUE;
   // media_cfg.turn_server = pj_str("turn.qtchina.net:34780");
   // media_cfg.turn_conn_type = PJ_TURN_TP_UDP;

   // media_cfg.turn_auth_cred.type = PJ_STUN_AUTH_CRED_STATIC;
   // media_cfg.turn_auth_cred.data.static_cred.realm = pj_str("pjsip.org");
   // media_cfg.turn_auth_cred.data.static_cred.username = pj_str("100");

   // media_cfg.turn_auth_cred.data.static_cred.data_type = PJ_STUN_PASSWD_PLAIN;
   // media_cfg.turn_auth_cred.data.static_cred.data = pj_str("100");

   

   // media_cfg.

   // Initialize pjsua
   status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
   if (status != PJ_SUCCESS) {
         pjsua_perror(__FILE__, "Error initializing pjsua", status);
         return;
   }

   pjsua_var.mconf_cfg.samples_per_frame = 8000; // pjsua_var from 

   /* Add UDP transport. */
   {
       pjsua_transport_config cfg;
       pjsua_transport_config rtp_cfg;
       
       // 创建指定端口的RTP/RTCP层media后端
       pjsua_transport_config_default(&rtp_cfg);
       rtp_cfg.port = 8050;
       status = pjsua_media_transports_create(&rtp_cfg);

       // SIP 层初始化，可指定端口
       pjsua_transport_config_default(&cfg);
       cfg.port = 15678; // if not set , use random big port 
       // cfg.public_addr = pj_str("123.1.2.3"); // 与上面的port一起可用于穿透，指定特定的公共端口!!!
       status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, &this->udp_tpid);
       if (status != PJ_SUCCESS) {
           pjsua_perror(__FILE__, "Error creating udp transport", status);
           // error_exit("Error creating transport", status);
       }

       // TCP transport
       pjsua_transport_config_default(&cfg);
       cfg.port = 56789;
       status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, &this->tcp_tpid);
       if (status != PJ_SUCCESS) {
           pjsua_perror(__FILE__, "Error creating tcp transport", status);
           // error_exit("Error creating transport", status);
       }
   }

   /* Initialization is done, now start pjsua */
   status = pjsua_start();
   if (status != PJ_SUCCESS) {
       pjsua_perror(__FILE__, "Error starting pjsua", status);
   }
   // error_exit("Error starting pjsua", status);

   // NON-standard SIP Extension
   pjsip_cfg()->endpt.allow_port_in_fromto_hdr = PJ_TRUE;

   status = pjsua_detect_nat_type();
   if (status != PJ_SUCCESS) {
       pjsua_perror(__FILE__, "Error starting pjsua", status);
       qLogx()<<status;
   }
   
   // 
    // 
    pjsua_codec_info infos[20];
    unsigned int info_count;

    info_count = 20;
    
    status = pjsua_enum_codecs(infos, &info_count);
    if (status == PJ_SUCCESS) {
        for (int i = 0; i < info_count; i ++) {
            QString codec_idname = QString((infos[i].codec_id).ptr);
            qLogx()<<"codec info:"<<"p="<<infos[i].priority<<" id="<<(infos[i].codec_id).ptr;
            this->uiw->comboBox->insertItem(this->uiw->comboBox->count(), codec_idname);
        }
    }

    pjmedia_aud_dev_info auids[128];
    pjmedia_snd_dev_info sndids[128];
    unsigned int auid_count = 128;
    unsigned int sndid_count = 128;

    status = pjsua_enum_aud_devs(auids, &auid_count);
    qDebug()<<"found aud dev count:"<<auid_count;
    status = pjsua_enum_snd_devs(sndids, &sndid_count);
    qDebug()<<"found snd dev count:"<<sndid_count;

    for (int i = 0 ; i < sndid_count; i ++) {
        QString name;
        qDebug()<<"aud:"<<QString(auids[i].name)<<" snd:"<<QString(sndids[i].name);
    }
    
    int cap_dev = -1, pb_dev = -1;
    status = pjsua_get_snd_dev(&cap_dev, &pb_dev);
    qDebug()<<"curr snd dev:"<<"status="<<status<<" cap="<<cap_dev<<" pb="<<pb_dev;
    qDebug()<<"snd ok?"<<pjsua_snd_is_active();

    // status = pjsua_set_snd_dev(0, 0);
    qDebug()<<"snd ok?"<<pjsua_snd_is_active();
}

void SipPhone::init_sip_client_ui_element()
{
    int qid_pjsua_call_id = qRegisterMetaType<pjsua_call_id>("pjsua_call_id");
    int qid_pjsua_acc_id = qRegisterMetaType<pjsua_acc_id>("pjsua_acc_id");
    Q_UNUSED(qid_pjsua_call_id);
    Q_UNUSED(qid_pjsua_acc_id);

    ::globalPjCallback = new PjCallback();
    PjCallback *pjcb = (PjCallback *)globalPjCallback;
	QObject::connect(pjcb, SIGNAL(sig_call_state(pjsua_call_id, pjsip_event *, pjsua_call_info *)),
                     this, SLOT(on1_call_state(pjsua_call_id, pjsip_event *, pjsua_call_info *)),
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_call_media_state(pjsua_call_id, pjsua_call_info *)),
                     this, SLOT(on1_call_media_state(pjsua_call_id, pjsua_call_info *)),
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_incoming_call(pjsua_acc_id, pjsua_call_id, pjsip_rx_data *)),
                     this, SLOT(on1_incoming_call(pjsua_acc_id, pjsua_call_id, pjsip_rx_data *)),
                     Qt::QueuedConnection);

	// QObject::connect(pjcb, SIGNAL(sig_new_connection(void *)),
    //                  this, SLOT(on1_new_connection(void *)),
    //                  Qt::QueuedConnection);

	// QObject::connect(pjcb, SIGNAL(sig_put_frame(QTcpSocket *, QByteArray)),
    //                  this, SLOT(on1_put_frame(QTcpSocket *, QByteArray)),
    //                  Qt::QueuedConnection);

    // wc = new WavClient(0);
    // app_init();
    // QObject::connect(this->ui->pushButton, SIGNAL(clicked()), this, SLOT(onCall()));
    // QObject::connect(this->uiw->pushButton, SIGNAL(clicked()), this, SLOT(onCallSip()));
    // QObject::connect(this->ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onHangup()));
    // QObject::connect(this->uiw->pushButton_2, SIGNAL(clicked()), this, SLOT(onHangupSip()));

    QObject::connect(this->uiw->pushButton_11, SIGNAL(clicked()), this, SLOT(onCallSipNew()));
    QObject::connect(this->uiw->toolButton_10, SIGNAL(clicked()), this, SLOT(onHangupSipNew()));

    QObject::connect(this->uiw->comboBox_6, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(onSelectedUserAccountChanged(int)));

    QObject::connect(this->uiw->toolButton_14, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_15, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_16, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_17, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_18, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_19, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_20, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_21, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_22, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_23, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_24, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_25, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));

}

void SipPhone::onManageSipAccounts()
{
    SipAccountsWindow *acc_win = new SipAccountsWindow();
    QObject::connect(acc_win, SIGNAL(accountWantRegister(QString, bool)),
                     this, SLOT(onRegisterAccount(QString, bool)));
    QObject::connect(acc_win, SIGNAL(accountWantRemove(QString)),
                     this, SLOT(onRemoveAccount(QString)));
    if (acc_win->exec() == QDialog::Accepted) {

    }
    delete acc_win;
}

int SipPhone::_find_account_from_pjacc(QString acc_name)
{
    unsigned int acc_cnt = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;

    acc_cnt = pjsua_acc_get_count();
    acc_cnt = 100;
    status = pjsua_enum_accs(acc_ids, (unsigned int*)&acc_cnt);
    if (status != PJ_SUCCESS) {
        return -1;
    }
    for (int i = 0 ; i < acc_cnt ; ++i) {
        status = pjsua_acc_get_info(acc_ids[i], &acc_info);
        if (status != PJ_SUCCESS) {
            continue;
        }
        acc_uri = QString::fromAscii(acc_info.acc_uri.ptr, acc_info.acc_uri.slen);
        // qDebug()<<"acc_uri for "<<acc_id<<" " << acc_uri;
        if (acc_uri.split(" ").at(0) == acc_name) {
            if (!pjsua_acc_is_valid(acc_info.id)) {
                qDebug()<<"Warning: acc id is not valid:"<<acc_info.id<<" "<<acc_uri;
            }
            return acc_info.id;
        }
    }

    return -1;
}

void SipPhone::onRegisterAccount(QString user_name, bool reg)
{
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    unsigned int acc_cnt;
    pjsua_acc_info acc_info;
    pj_status_t status;

    /* Register to SIP server by creating SIP account. */
    pjsua_acc_config cfg;
    SipAccount sip_acc;
    QString acc_uri;

    if (reg) {
        acc_id = this->_find_account_from_pjacc(user_name);
        sip_acc = sip_acc.getAccount(user_name);
        Q_ASSERT(sip_acc.userName.isEmpty() == false);

        pjsua_acc_config_default(&cfg);
        // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
        cfg.id = pj_str(SIP_USER " <sip:" SIP_USER "@"  "sips.qtchina.net:15678>");
        // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
        cfg.reg_uri = pj_str(QString("SIP:%1").arg(sip_acc.domain).toAscii().data());
        // cfg.reg_timeout = 800000000;
        // cfg.publish_enabled = PJ_FALSE;
        // cfg.auth_pref.initial_auth = 0; // no use
        // cfg.reg_retry_interval = 0;
        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str(sip_acc.domain.toAscii().data());
        cfg.cred_info[0].scheme = pj_str("digest");
        cfg.cred_info[0].username = pj_str(sip_acc.userName.toAscii().data());
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(sip_acc.password.toAscii().data());

        if (acc_id == -1) {
            status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
            if (status != PJ_SUCCESS) {
                pjsua_perror(__FILE__, "Error adding account", status);
                //   error_exit("Error adding account", status);
            }
        } else {
            status = pjsua_acc_modify(acc_id, &cfg);
            status = pjsua_acc_set_registration(acc_id, 1);
        }
    } else {
        acc_id = this->_find_account_from_pjacc(user_name);
        if (acc_id != -1) {
            status = pjsua_acc_set_registration(acc_id, 0); // unregister
        } else {
            qDebug()<<"User not exist, or not valid";
        }
        // acc_cnt = sizeof(acc_ids);
        // status = pjsua_enum_accs(acc_ids, &acc_cnt);
        // if (status == PJ_SUCCESS) {
        //     for (int i = 0; i < acc_cnt; ++i) {
        //         acc_id = acc_ids[i];
        //         status = pjsua_acc_get_info(acc_id, &acc_info);
        //         if (status != PJ_SUCCESS) {
        //             continue;
        //         }
        //         acc_uri = QString::fromAscii(acc_info.acc_uri.ptr, acc_info.acc_uri.slen);
        //         QString sip_uri_exp_str = "SIP:([a-z].*)@([0-9a-z-_\\.].*):([0-9]{0,6})";
        //         QRegExp sip_uri_exp(sip_uri_exp_str, Qt::CaseInsensitive);
        //         if (sip_uri_exp.exactMatch(acc_uri)) {
        //             qDebug()<<sip_uri_exp.capturedTexts();
        //             if (sip_uri_exp.cap(1) == user_name) {
        //                 status = pjsua_acc_set_registration(acc_id, 0); // unregister
        //                 break;
        //             }
        //         }
        //     }
        // }
    }
}

void SipPhone::onRemoveAccount(QString user_name)
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    acc_id = this->_find_account_from_pjacc(user_name);
    if (acc_id != -1) {
        status = pjsua_acc_set_registration(acc_id, 0);
        status = pjsua_acc_del(acc_id);
    } else {
        qLogx()<<"Can not find pjsip account:"<<user_name;
    }
}

void SipPhone::onCallSip()
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    // set codec
    pjsua_codec_info cids[100];
    unsigned int cid_cnt = 100;
    QString selected_codec;
    
    // selected_codec = this->uiw->comboBox_11->currentText();
    status = pjsua_enum_codecs(cids, &cid_cnt);
    for (int i = 0; i < cid_cnt; i++) {
        if (QString(cids[i].codec_id.ptr) == selected_codec) {
            status = pjsua_codec_set_priority(&cids[i].codec_id, 200);
            qLogx()<<"Using codec for this call,"<<selected_codec;
        } else {
            status = pjsua_codec_set_priority(&cids[i].codec_id, 0);
        }
    }

    // if (this->uiw->comboBox_12->currentIndex() == 0) {
    //     // UDP mode
    //     status = pjsua_transport_set_enable(this->tcp_tpid, PJ_FALSE);
    //     status = pjsua_transport_set_enable(this->udp_tpid, PJ_TRUE);
    // } else {
    //     // TCP mode
    //     status = pjsua_transport_set_enable(this->tcp_tpid, PJ_TRUE);
    //     status = pjsua_transport_set_enable(this->udp_tpid, PJ_FALSE);
    // }
    // qDebug()<<"Using transport: "<< this->uiw->comboBox_12->currentText();

    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg;

        pjsua_acc_config_default(&cfg);
        // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
        cfg.id = pj_str(SIP_USER " <sip:" SIP_USER "@"  "192.168.15.53:5678>");
        // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
        // cfg.reg_timeout = 800000000;
        // cfg.publish_enabled = PJ_FALSE;
        // cfg.auth_pref.initial_auth = 0; // no use
        // cfg.reg_retry_interval = 0;
        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str(SIP_DOMAIN);
        cfg.cred_info[0].scheme = pj_str("digest");
        cfg.cred_info[0].username = pj_str(SIP_USER);
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(SIP_PASSWD);

        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error adding account", status);
            //   error_exit("Error adding account", status);
        }
    }
    QString callee_phone;// = this->uiw->comboBox->currentText();
    QString sip_server;// = this->uiw->comboBox_2->currentText();
    // char *sipu = "<SIP:99008668056013552776960@122.228.202.105:4060;transport=UDP>";
    char *sipu = strdup(QString("<SIP:%1@%2;transport=UDP>")
                        .arg(callee_phone).arg(sip_server) .toAscii().data());
    qLogx()<<"call peer: "<<sipu;
    // char *sipu = "<SIP:99008668056013552776960@202.108.29.234:5060;transport=UDP>";
    pj_str_t uri = pj_str(sipu);
    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error making call", status);
        //error_exit("Error making call", status);
    }
    free(sipu);

    qLogx()<<"oncall slot returned";
}

void SipPhone::onHangupSip()
{
    pj_status_t status;

    // 枚举当前的 call
    if (pjsua_call_get_count() == 1) {
        pjsua_call_id cids[100];
        unsigned int call_count = 0;

        status = pjsua_enum_calls(cids, &call_count);
        qLogx()<<"active calls:"<<call_count;

        status = pjsua_call_hangup(cids[0], 0, NULL, NULL);
    } else if (pjsua_call_get_count() == 0) {
        qLogx()<<"No active call";
    } else {
        qLogx()<<"Why has more than 1 calls";
    }

    // pjsua_call_hangup_all();
}

// sip
void SipPhone::on1_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pjsua_call_info ci;
 
    PJ_UNUSED_ARG(e);
 
    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3, (__FILE__, "Call %d state=%.*s", call_id,
              (int)ci.state_text.slen,
              ci.state_text.ptr));

}

void SipPhone::on1_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pjsua_call_info ci;
    pj_status_t status;
 
    pjsua_call_get_info(call_id, &ci);
 
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);

        // test port_info
        pjsua_conf_port_info cpi;
        pjsua_conf_get_port_info(ci.conf_slot, &cpi);
        qDebug()<<"conf port info: port number="<<cpi.slot_id<<",name='"<<cpi.name.ptr<<"', chan cnt="<<cpi.channel_count
                <<", clock rate="<<cpi.clock_rate;

        // pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(g_rec_id));

        pjmedia_transport *mtp = pjsua_call_get_media_transport(call_id);
        pjmedia_session *ms = pjsua_call_get_media_session(call_id);
        pjmedia_port *mp = NULL;
        unsigned int stream_count = 100;
        pjmedia_stream_info msi[100];
        pjmedia_frame from_lang_frame;
        int i = 0;
        unsigned short n_port = 0;

        // pjsua_switcher_get_port(g_rec_id, &mp);
        // n_port = pjsua_switcher_get_net_port(mp);
        qDebug()<<"got wav server port :"<<n_port;
        // wc->setPort(n_port);
        // wc->arun();
        
        if (ms == NULL) {
            qDebug()<<"pjmedia_session is null.\n";
        } else {
            status = pjmedia_session_enum_streams(ms, &stream_count, msi);
            qDebug()<<"enum stream count:"<<stream_count;
            
            status = pjmedia_session_get_port(ms, 0, &mp);
            Q_ASSERT(status == PJ_SUCCESS);

            QFile wfp("/tmp/arec.wav");
            if (!wfp.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) {
                exit(-1);
            }
            // why kitphone: ../src/pjmedia/plc_common.c：106：pjmedia_plc_generate: 断言“plc && frame”失败。
            // while ((status = pjmedia_port_get_frame(mp, &from_lang_frame)) == PJ_SUCCESS
            //        && i ++ < 10000) {
            //     if (from_lang_frame.size > 0) {
            //         wfp.write((char*)(from_lang_frame.buf), from_lang_frame.size);
            //     } else {
            //         qDebug()<<"got 0 frame. stop.";
            //         break;
            //     }
            // }
            wfp.close();
            
        }
    }

    // pjsua_dump(PJ_TRUE);
    // pjsua_call_dump(call_id, ...);
}

void SipPhone::on1_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pjsua_call_info ci;
 
    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);
 
    pjsua_call_get_info(call_id, &ci);
 
    PJ_LOG(3,(__FILE__, "Incoming call from %.*s!!",
              (int)ci.remote_info.slen,
              ci.remote_info.ptr));
 
    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);

}

// void SipPhone::on1_new_connection(void *m_port)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
//     pj_status_t status;
//     // tcp_port *f_port = (tcp_port*)m_port;
//     // tcp_port *fport = f_port;

//     /*
//                                                     pjsua_var.media_cfg.clock_rate, 
//                                                     pjsua_var.mconf_cfg.channel_count,
//                                                     pjsua_var.mconf_cfg.samples_per_frame,
//                                                     pjsua_var.mconf_cfg.bits_per_sample, 

//      */
    
//     // pj_int16_t channel_count = pjsua_var.mconf_cfg.channel_count;
//     // unsigned sampling_rate = pjsua_var.mconf_cfg.samples_per_frame;
    
//     // f_port->cli_handle = f_port->serv_handle->nextPendingConnection();
//     // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port<<f_port->cli_handle->peerPort();

//     // write file need header, redirect to skype, not need header
//     bool need_wav_header = true;
//     if (!need_wav_header) {
//         return;
//     }
//     // try write wav header
//     pjmedia_wave_hdr wave_hdr;
//     pj_ssize_t size;

//     /* Initialize WAVE header */
//     pj_bzero(&wave_hdr, sizeof(pjmedia_wave_hdr));
//     wave_hdr.riff_hdr.riff = PJMEDIA_RIFF_TAG;
//     wave_hdr.riff_hdr.file_len = 1024*1024*500; /* will be filled later */
//     wave_hdr.riff_hdr.wave = PJMEDIA_WAVE_TAG;

//     wave_hdr.fmt_hdr.fmt = PJMEDIA_FMT_TAG;
//     wave_hdr.fmt_hdr.len = 16;
//     // wave_hdr.fmt_hdr.fmt_tag = (pj_uint16_t)fport->fmt_tag;
//     // wave_hdr.fmt_hdr.nchan = (pj_int16_t)channel_count;
//     // wave_hdr.fmt_hdr.sample_rate = sampling_rate;
//     // wave_hdr.fmt_hdr.bytes_per_sec = sampling_rate * channel_count *
//     // fport->bytes_per_sample;
//     // wave_hdr.fmt_hdr.block_align = (pj_uint16_t)
//     //     (fport->bytes_per_sample * channel_count);
//     // wave_hdr.fmt_hdr.bits_per_sample = (pj_uint16_t)
//     //     (fport->bytes_per_sample * 8);

//     wave_hdr.data_hdr.data = PJMEDIA_DATA_TAG;
//     wave_hdr.data_hdr.len = 0;      /* will be filled later */


//     /* Convert WAVE header from host byte order to little endian
//      * before writing the header.
//      */
//     pjmedia_wave_hdr_host_to_file(&wave_hdr);

//     /* Write WAVE header */
//     // if (fport->fmt_tag != PJMEDIA_WAVE_FMT_TAG_PCM) {
//     //     pjmedia_wave_subchunk fact_chunk;
//     //     pj_uint32_t tmp = 0;

//     //     fact_chunk.id = PJMEDIA_FACT_TAG;
//     //     fact_chunk.len = 4;

//     //     PJMEDIA_WAVE_NORMALIZE_SUBCHUNK(&fact_chunk);

//     //     /* Write WAVE header without DATA chunk header */
//     //     size = sizeof(pjmedia_wave_hdr) - sizeof(wave_hdr.data_hdr);
//     //     // status = pj_file_write(fport->fd, &wave_hdr, &size);
//     //     // if (status != PJ_SUCCESS) {
//     //     //     pj_file_close(fport->fd);
//     //     //     return status;
//     //     // }
//     //     // f_port->cli_handle->write((char*)&wave_hdr, size);

//     //     /* Write FACT chunk if it stores compressed data */
//     //     size = sizeof(fact_chunk);
//     //     // status = pj_file_write(fport->fd, &fact_chunk, &size);
//     //     // if (status != PJ_SUCCESS) {
//     //     //     pj_file_close(fport->fd);
//     //     //     return status;
//     //     // }
//     //     // f_port->cli_handle->write((char*)&fact_chunk, size);

//     //     size = 4;
//     //     // status = pj_file_write(fport->fd, &tmp, &size);
//     //     // if (status != PJ_SUCCESS) {
//     //     //     pj_file_close(fport->fd);
//     //     //     return status;
//     //     // }
//     //     // f_port->cli_handle->write((char*)&tmp, size);

//     //     /* Write DATA chunk header */
//     //     size = sizeof(wave_hdr.data_hdr);
//     //     // status = pj_file_write(fport->fd, &wave_hdr.data_hdr, &size);
//     //     // if (status != PJ_SUCCESS) {
//     //     //     pj_file_close(fport->fd);
//     //     //     return status;
//     //     // }
//     //     // f_port->cli_handle->write((char*)&wave_hdr.data_hdr, size);
//     // } else {
//     //     size = sizeof(pjmedia_wave_hdr);
//     //     // status = pj_file_write(fport->fd, &wave_hdr, &size);
//     //     // if (status != PJ_SUCCESS) {
//     //     //     pj_file_close(fport->fd);
//     //     //     return status;
//     //     // }
//     //     // f_port->cli_handle->write((char*)&wave_hdr, size);
//     // }

// }

// void SipPhone::on1_put_frame(QTcpSocket *sock, QByteArray fba)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sock<<fba.length();
//     sock->write(fba);
// }

//////
void SipPhone::onCallSipNew()
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    // set codec
    pjsua_codec_info cids[100];
    unsigned int cid_cnt = 100;
    QString selected_codec;
    char tbuf[200];
    
    selected_codec = this->uiw->comboBox->currentText();
    status = pjsua_enum_codecs(cids, &cid_cnt);
    for (int i = 0; i < cid_cnt; i++) {
        if (QString(cids[i].codec_id.ptr) == selected_codec) {
            status = pjsua_codec_set_priority(&cids[i].codec_id, 200);
            qLogx()<<"Using codec for this call,"<<selected_codec;
        } else {
            status = pjsua_codec_set_priority(&cids[i].codec_id, 0);
        }
    }

    switch (this->uiw->comboBox_2->currentIndex()) {
    case 0:
        // UDP mode
        status = pjsua_transport_set_enable(this->tcp_tpid, PJ_FALSE);
        status = pjsua_transport_set_enable(this->udp_tpid, PJ_TRUE);
        break;
    case 1:
        // TCP mode
        status = pjsua_transport_set_enable(this->tcp_tpid, PJ_TRUE);
        status = pjsua_transport_set_enable(this->udp_tpid, PJ_FALSE);
        break;
    default:
        Q_ASSERT(1==2);
        break;
    }
    qLogx()<<"Using transport: "<< this->uiw->comboBox_2->currentText();

    //////////
    acc_id = this->_create_sip_account(QString());
    if (acc_id == -1) {
        return;
    }

    QString callee_phone = this->uiw->comboBox_7->currentText();
    QString sip_server;// = this->uiw->comboBox_2->currentText();
    sip_server = "202.108.29.234:4060";
    // char *sipu = "<SIP:99008668056013552776960@122.228.202.105:4060;transport=UDP>";
    char *sipu = strdup(QString("<SIP:%1@%2;transport=UDP>")
                        .arg(callee_phone).arg(sip_server) .toAscii().data());
    qLogx()<<"call peer: "<<sipu;
    // char *sipu = "<SIP:99008668056013552776960@202.108.29.234:5060;transport=UDP>";
    pj_str_t uri = pj_str(sipu);
    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
    if (status != PJ_SUCCESS) {
        if (status == 469996) {
            this->log_output(LT_USER, "无法打开声音设备。");
        } else {
            pjsua_perror(__FILE__, "Error making call", status);
            //error_exit("Error making call", status);
        }
    }
    free(sipu);

    qLogx()<<"oncall slot returned";
}

void SipPhone::onHangupSipNew()
{
    pjsua_call_hangup_all();    
}

pjsua_acc_id SipPhone::_create_sip_account(QString acc_name)
{
    pj_status_t status;
    pjsua_acc_id acc_id = -1;
    char tbuf[200];

    /* Register to SIP server by creating SIP account. */

    pjsua_acc_config cfg;
    QString caller_sip_user = this->uiw->comboBox_6->currentText();
    QString caller_from_domain = this->_get_sip_from_domain();

    if (this->uiw->comboBox_6->currentIndex() == this->uiw->comboBox_6->count()-1) {
        qLogx()<<"select a host please";
        return acc_id;
    }

    pjsua_acc_config_default(&cfg);
    // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
    // cfg.id = pj_str(SIP_USER " <sip:" SIP_USER "@"  "192.168.15.53:5678>");
    // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
    memset(tbuf, 0, sizeof(tbuf));
    QString reg_uri_str = QString("%1 <sip:%1@%2>").arg(caller_sip_user.split("@").at(0))
        .arg(caller_from_domain);
    strncpy(tbuf, reg_uri_str.toAscii().data(), sizeof(tbuf)-1);
    qLogx()<<reg_uri_str<<tbuf;
    cfg.id = pj_str(tbuf);

    // cfg.reg_timeout = 800000000;
    // cfg.publish_enabled = PJ_FALSE;
    // cfg.auth_pref.initial_auth = 0; // no use
    // cfg.reg_retry_interval = 0;
    cfg.cred_count = 1;
    cfg.cred_info[0].realm = pj_str("*");
    cfg.cred_info[0].scheme = pj_str("digest");
    cfg.cred_info[0].username = pj_str(caller_sip_user.split("@").at(0).toAscii().data());
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].data = pj_str(SIP_PASSWD);

    status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error adding account", status);
        //   error_exit("Error adding account", status);
    }

    return acc_id;
}

QString SipPhone::_get_sip_from_domain()
{
    QString from_domain;

    QString ipaddr;
    QHostAddress addr;
    QList<QHostAddress> addr_list;
    QList<QString> addr_str_list;
    
    addr_list = QNetworkInterface::allAddresses();
    // qDebug()<<addr_list;
    for (int i = 0 ; i < addr_list.count() ; i ++) {
        addr_str_list.append(addr_list.at(i).toString());
    }
    // qSort(list.begin(), list.end(), qGreater<int>());
    qSort(addr_str_list.begin(), addr_str_list.end(), qGreater<QString>());

    if (addr_str_list.count() == 0) {
        ipaddr = "0.0.0.0";
    } else if (addr_str_list.count() == 1) {
        // must be 127.0.0.1
        ipaddr = addr_str_list.at(0);
    } else {
        for (int i = 0 ; i < addr_str_list.count(); i ++) {
            // addr = addr_list.at(i);
            ipaddr = addr_str_list.at(i);
            if (ipaddr.indexOf(":") != -1) {
                // ipv6 addr
                ipaddr = QString();
                continue;
            } else {
                if (ipaddr.startsWith("127.0")) {
                    ipaddr = QString();
                    continue;
                } else if (!ipaddr.startsWith("172.24.")
                           &&!ipaddr.startsWith("192.168.")
                           &&!ipaddr.startsWith("10.10.")) {
                    // should a big ip addr
                    qDebug()<<"break big ip";
                    break;
                } else if (ipaddr.startsWith("172.24.")) {
                    qDebug()<<"break 172.2";
                    break;
                } else if (ipaddr.startsWith("10.10.")) {
                    break;
                } else if (ipaddr.startsWith("192.168.")) {
                    break;
                } else {
                    // do not want go here
                    Q_ASSERT(1 == 2);
                    break;
                }
            }
            ipaddr = QString();
        }
    }

    pjsua_transport_config cfg;
    pjsua_transport_info tinfo;
    if (this->uiw->comboBox_2->currentIndex() == 0) {
        // UDP mode
        pjsua_transport_get_info(this->udp_tpid, &tinfo);
    } else {
        // TCP mode
        pjsua_transport_get_info(this->tcp_tpid, &tinfo);
    }
    qLogx()<<"Using transport: "<< this->uiw->comboBox_2->currentText();

    unsigned short port = ntohs(tinfo.local_addr.ipv4.sin_port);
    
    from_domain = QString("KP-%1-%2:%2").arg(ipaddr).arg(port);
    // from_domain = QString("%1:%2").arg(ipaddr).arg(port);
    return from_domain;
}

void SipPhone::onSelectedUserAccountChanged(int idx)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<idx;
    qLogx()<<this->uiw->comboBox_6->currentText();
}

void SipPhone::onDigitButtonClicked()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<(sender());
    QToolButton *btn = static_cast<QToolButton*>(sender());

    QString digit;
    QHash<QToolButton*, char> bdmap;
    bdmap[this->uiw->toolButton_14] = '0';
    bdmap[this->uiw->toolButton_15] = '1';
    bdmap[this->uiw->toolButton_16] = '2';
    bdmap[this->uiw->toolButton_17] = '3';
    bdmap[this->uiw->toolButton_18] = '4';
    bdmap[this->uiw->toolButton_19] = '5';
    bdmap[this->uiw->toolButton_20] = '6';
    bdmap[this->uiw->toolButton_21] = '7';
    bdmap[this->uiw->toolButton_22] = '8';
    bdmap[this->uiw->toolButton_23] = '9';
    bdmap[this->uiw->toolButton_24] = '*';
    bdmap[this->uiw->toolButton_25] = '#';

    Q_ASSERT(bdmap.contains(btn));

    QString num = this->uiw->comboBox_7->currentText();
    this->uiw->comboBox_7->setEditText(num + bdmap[btn]);
    
    // if (in call state) it's dtmf digit
}


////////////////////////////////
PJSipEventThread::PJSipEventThread(QObject *parent)
    : QThread(parent)
{
    this->quit_loop = false;
}

PJSipEventThread::~PJSipEventThread()
{
    
}

void PJSipEventThread::run()
{
    pj_thread_desc initdec;
    pj_thread_t* thread = 0;
    pj_status_t status;
    int evt_cnt = 0;

    qLogx()<<"ready register pjsip thread by Qt";
    if (!pj_thread_is_registered()) {
        status = pj_thread_register("PjsipPollThread_run", initdec, &thread);
        if (status != PJ_SUCCESS) {
            qLogx()<<"pj_thread_register faild:"<<status;
            return;
        }
    }
    PJ_CHECK_STACK();
    qLogx()<<"registerred pjsip thread:"<<thread;

    this->dump_info(thread);
    // while(isStackInit()) {
    //     pjsua_handle_events(10);
    // }
    qLogx()<<"enter pjsua thread loop supported by Qt";
    while (!quit_loop) {
        evt_cnt = pjsua_handle_events(20);
        // qDebug()<<"pjsua pool event in thread supported by Qt, proccess count:"<<evt_cnt;
        if (qrand() % 100 == 1) {
            fprintf(stdout, "."); fflush(stdout);
        }
    }
}

void PJSipEventThread::dump_info(pj_thread_t *thread)
{
    Q_ASSERT(thread != NULL);

    qLogx()<<"pj_thread_is_registered:"<<pj_thread_is_registered();
    qLogx()<<"pj_thread_get_prio:"<<pj_thread_get_prio(thread);
    qLogx()<<"pj_thread_get_prio_min:"<<pj_thread_get_prio_min(thread);
    qLogx()<<"pj_thread_get_prio_max:"<<pj_thread_get_prio_max(thread);
    qLogx()<<"pj_thread_get_name:"<<pj_thread_get_name(thread);
    qLogx()<<"pj_getpid:"<<pj_getpid();
}

// TODO 所有明文字符串需要使用翻译方式获取，而不是直接写在源代码中
// log is utf8 codec
void SipPhone::log_output(int type, const QString &log)
{
    QListWidgetItem *witem = nullptr;
    QString log_time = QDateTime::currentDateTime().toString("hh:mm:ss");

    int debug = 1;

    QTextCodec *u8codec = QTextCodec::codecForName("UTF-8");
    QString u16_log = log_time + " " + u8codec->toUnicode(log.toAscii());

    if (type == LT_USER) {
        // TODO 怎么确定是属于呼叫日志呢。恐怕还是得在相应的地方执行才行。
        this->uiw->label_11->setText(u16_log);
        witem = new QListWidgetItem(QIcon(":/skins/default/info.png"), u16_log);
        this->uiw->listWidget->addItem(witem);
    } else if (type == LT_DEBUG && debug) {
        witem = new QListWidgetItem(QIcon(":/skins/default/info.png"), u16_log);
        this->uiw->listWidget->addItem(witem);
    } else {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<type<<log;
    }

    // 清除多余日志
    static int max_log_count = 30;
    if (debug == 1) {
        max_log_count += 200;
    }
    if (this->uiw->listWidget->count() > max_log_count) {
        int rm_count = this->uiw->listWidget->count() - max_log_count;
        // 从最老的日志开始删除
        for (int i = 0; i < rm_count; i++) {
            witem = this->uiw->listWidget->takeItem(i);
            delete witem;
        }
        // 从最新的开始
        // for (int i = rm_count - 1; i >= 0; i--) {
        //     witem = this->uiw->listWidget->takeItem(max_log_count+i);
        //     delete witem;
        // }
    }

    qLogx()<<type<<u16_log;
}
