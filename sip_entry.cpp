// sip_vars.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-12 21:19:16 +0800
// Version: $Id: sip_entry.cpp 609 2010-12-06 07:24:32Z drswinghead $
// 

#include <sys/epoll.h>

#include <QtCore>
#include <QtNetwork>
#include <QtGui>

#include "pjsip.h"
#include "pjsua.h"
#include <pjmedia/wave.h>
#include <pjmedia/wav_port.h>

#include "wav_switcher.h"
#include "sua_switcher.h"
#include "sua_handle.h"

#include "PjCallback.h"

#include "sip_entry.h"

#define THIS_FILE __FILE__

PjsuaHandleThread *evht = NULL;

int _find_account_from_pjacc(QString acc_name)
{
  // QThread *curr_thread = this->thread();
    // this->moveToThread(::evht);
    int acc_count = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;

    acc_count = pjsua_acc_get_count();
    acc_count = sizeof(acc_ids)/sizeof(pjsua_acc_id);
    qDebug()<<"before enumed_accs:"<<acc_count;
    status = pjsua_enum_accs(acc_ids, (unsigned int*)&acc_count);
    qDebug()<<"enumed_accs:"<<acc_count<<status;
    if (status != PJ_SUCCESS) {
        // this->moveToThread(curr_thread);
        return -1;
    }
    for (int i = 0 ; i < acc_count ; ++i) {
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
            // this->moveToThread(curr_thread);
            return acc_info.id;
        }
    }

    // this->moveToThread(curr_thread);
    return -1;
}


int _delete_account_by_active(int cnt)
{
    int acc_count = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;
    pjsua_acc_id old_acc_id = -1;
    unsigned old_acc_time = UINT_MAX;

    acc_count = pjsua_acc_get_count();
    acc_count = sizeof(acc_ids)/sizeof(pjsua_acc_id);
    status = pjsua_enum_accs(acc_ids, (unsigned int*)&acc_count);
    if (status != PJ_SUCCESS) {
        return -1;
    }
    for (int i = 0 ; i < acc_count ; ++i) {
        acc_id = acc_ids[i];
        if (pjsua_get_var()->acc[acc_id].cfg.unreg_timeout < old_acc_time) {
            old_acc_id = acc_ids[i];
        }
    }

    if (old_acc_id != -1) {
        if (pjsua_acc_get_default() == old_acc_id) {
        } else {
        }
        status = pjsua_acc_del(old_acc_id);
        return 1;
    }
    
    return 0;
}

struct acc_proc_args {
  char caller_name[60];
  char phone_number[80];
  char serv_addr[60];
  int skypeCallID;
};

// pthread_t acc_mod_thread;

void * create_pjsua_acc_proc(void *args, SipVarSet *_this)
{
    qDebug()<<"oncall slot returned2";

    pjsua_acc_id acc_id;
    pjsua_acc_info acc_info;
    pj_status_t status;
    char ubuf[256] = {0};
    struct acc_proc_args *targs = (struct acc_proc_args *)args;

    pj_thread_desc initdec;
    pj_thread_t *thread = 0;
    char pj_tname[100] = {0};

//     snprintf(pj_tname, sizeof(pj_tname) - 1, "simpleaddpjthread%d", getpid());
//     if (!pj_thread_is_registered()) {
//       // status = pj_thread_register("simpleaddpjthread", initdec, &thread);
//       status = pj_thread_register(pj_tname, initdec, &thread);
//       if (status != PJ_SUCCESS) {
// 	qDebug()<<"register thread error:"<<status;
// 	return 0;
//       }
//     }
//     PJ_CHECK_STACK();

    qDebug()<<"max allowed call count:"<<pjsua_call_get_max_count()
	    <<"current call count:"<<pjsua_call_get_count();

    if (pjsua_call_get_count() > 0) {
      pjsua_call_hangup_all();
    }

    if (pjsua_call_get_count() >= pjsua_call_get_max_count()) {
      PjCallback::on_exceed_max_call_count_wrapper(targs->skypeCallID);
      free(targs);
      return 0;
    }

    static char *SIP_FROM_DOMAIN = "sips.qtchina.net:5060";


    acc_id = _find_account_from_pjacc(targs->caller_name);
    if (acc_id != -1) {
        qDebug()<<"found account in pool, do not need create a new one.";
        pjsua_get_var()->acc[acc_id].cfg.unreg_timeout = time(NULL);
        pjsua_acc_set_default(acc_id);
    }
    else
    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg;

        pjsua_acc_config_default(&cfg);
        // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
        // cfg.id = pj_str(SIP_USER " <sip:" SIP_USER "@"  "192.168.15.53:5678>");
        snprintf(ubuf, sizeof(ubuf) - 1, "%s <sip:%s@%s>",
                 targs->caller_name, targs->caller_name, SIP_FROM_DOMAIN);
        cfg.id = pj_str(ubuf);
        // cfg.id = pj_str(QString("%1 <sip:%1@%2>").arg(caller_name).arg(SIP_DOMAIN).toAscii().data());
        // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
        // cfg.reg_timeout = 800000000;
        // cfg.publish_enabled = PJ_FALSE;
        // cfg.reg_retry_interval = 0;
        cfg.cred_count = 1;
        // cfg.cred_info[0].realm = pj_str(SIP_FROM_DOMAIN);
        cfg.cred_info[0].realm = pj_str("*");
        // cfg.cred_info[0].realm = pj_str("192.168.15.53:5678");
        cfg.cred_info[0].scheme = pj_str("digest");
        // cfg.cred_info[0].username = pj_str(SIP_USER);
        cfg.cred_info[0].username = pj_str(targs->caller_name);
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        // cfg.cred_info[0].data = pj_str(SIP_PASSWD);
        cfg.cred_info[0].data = pj_str("88888888");
        cfg.unreg_timeout = time(NULL);

        if (pjsua_acc_get_count() >= PJSUA_MAX_ACC) {
            qDebug()<<"Account count will exseed max if add new one: max/now"
                    <<PJSUA_MAX_ACC<<"/"<<pjsua_acc_get_count();
            // delete one and add new
            status = _delete_account_by_active(1);
            qDebug()<<"A2ccount count will exseed max if add new one2: max/now"
                    <<PJSUA_MAX_ACC<<"/"<<pjsua_acc_get_count();
	}
        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error adding account", status);
            //   error_exit("Error adding account", status);
            qDebug()<<__FILE__<<__LINE__<<"Error adding account"<<status
                    <<QString::fromAscii(cfg.id.ptr, cfg.id.slen);
            return (void*)status;
        }
        status = pjsua_acc_set_default(acc_id);
    }
    QString callee_phone = targs->phone_number;
    QString sip_server = targs->serv_addr; // "122.228.202.105:4060"; // this->ui->comboBox_2->currentText();
    // char *sipu = "<SIP:99008668056013552776960@122.228.202.105:4060;transport=UDP>";
    // char *sipu = strdup(QString("<SIP:%1@%2;transport=TCP>")
    //                        .arg(callee_phone).arg(sip_server) .toAscii().data());
    snprintf(ubuf, sizeof(ubuf) - 1, "<SIP:%s@%s;transport=TCP>",
             callee_phone.toAscii().data(), sip_server.toAscii().data());
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"call peer: "<<ubuf;
    // char *sipu = "<SIP:99008668056013552776960@202.108.29.234:5060;transport=UDP>";

    pjsua_call_id call_id = -555555;
    pj_str_t uri = pj_str(ubuf);
    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, &call_id);
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error making call", status);
        qDebug()<<__FILE__<<__LINE__<<"Error making call"<<status;
        //error_exit("Error making call", status);
    }

    // PjCallback::on_user_added_wrapper(targs->skypeCallID, call_id, pthread_self());    
    // this->mSkypeSipCallMap.insert(skypeCallID, call_id);
    _this->mSkypeSipCallMap.insert(targs->skypeCallID, call_id);
    free(targs);

    qDebug()<<"oncall slot returned";

    // sleep(20);
    // pthread_detach(pthread_self());

    status = PJ_SUCCESS;
    return (void*)status;     
}


SipVarSet::SipVarSet(QObject *parent)
    : QThread(parent)
{
    this->registerSipTypes();
    this->use_tcp_transport = true;
    this->quit_pjthread_loop = false;

    // ::evht = new PjsuaHandleThread();

    ::globalPjCallback = this->callbackObject = new PjCallback();
    PjCallback *pjcb = (PjCallback *)this->callbackObject;

	QObject::connect(pjcb, SIGNAL(sig_call_state(pjsua_call_id, pjsip_event *, pjsua_call_info *)),
                     this, SLOT(on1_call_state(pjsua_call_id, pjsip_event *, pjsua_call_info *)),
                     // Qt::DirectConnection);
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_call_media_state(pjsua_call_id, pjsua_call_info*)),
                     this, SLOT(on1_call_media_state(pjsua_call_id, pjsua_call_info*)),
                     // Qt::DirectConnection);
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_incoming_call(pjsua_acc_id, pjsua_call_id, pjsip_rx_data *)),
                     this, SLOT(on1_incoming_call(pjsua_acc_id, pjsua_call_id, pjsip_rx_data *)),
                     // Qt::DirectConnection);
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_user_added(int, pjsua_call_id, int)),
			 this, SLOT(on1_user_added(int, pjsua_call_id, int)),
                     // Qt::DirectConnection);
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_exceed_max_call_count(int)),
			 this, SLOT(on1_exceed_max_call_count(int)),
			 // Qt::DirectConnection);
			 Qt::QueuedConnection);
	

	QObject::connect(pjcb, SIGNAL(sig_new_connection(void *)),
                     this, SLOT(on1_new_connection(void *)),
                     Qt::QueuedConnection);
	QObject::connect(pjcb, SIGNAL(sig_new_incoming_connection(void *)),
                     this, SLOT(on1_new_incoming_connection(void *)),
                     Qt::QueuedConnection);

	QObject::connect(pjcb, SIGNAL(sig_put_frame(QTcpSocket *, QByteArray)),
                     this, SLOT(on1_put_frame(QTcpSocket *, QByteArray)),
                     Qt::QueuedConnection);
}

SipVarSet::~SipVarSet()
{

}


QMutex mt1;
QWaitCondition cond1;
QQueue<void *> qu1;

void SipVarSet::run()
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"runing now";
    pj_thread_desc initdec;
    pj_thread_t* thread = 0;
    pj_status_t status;
    int evt_cnt = 0;
    void *targs = 0;

    qDebug()<<"ready register pjsip thread by Qt";
    if (!pj_thread_is_registered()) {
        status = pj_thread_register("PjsipPollThread_run", initdec, &thread);
        if (status != PJ_SUCCESS) {
            qDebug()<<"pj_thread_register faild:"<<status;
            return;
        }
    }
    PJ_CHECK_STACK();
    qDebug()<<"registerred pjsip thread:"<<thread;

    this->dump_pjthread_info(thread);

    while (true) {
      if (qu1.isEmpty()) {
	qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"before mutex lock...";
	mt1.lock();
	qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"before cond wait...";
	cond1.wait(&mt1);
	qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"after cond wait...";
	mt1.unlock();
	qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"after mutex lock...";
      } else {
	targs = qu1.dequeue();
	create_pjsua_acc_proc(targs, this);
      }
    }
}


void SipVarSet::run1()
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"runing now";
    pj_thread_desc initdec;
    pj_thread_t* thread = 0;
    pj_status_t status;
    int evt_cnt = 0;

    qDebug()<<"ready register pjsip thread by Qt";
    if (!pj_thread_is_registered()) {
        status = pj_thread_register("PjsipPollThread_run", initdec, &thread);
        if (status != PJ_SUCCESS) {
            qDebug()<<"pj_thread_register faild:"<<status;
            return;
        }
    }
    PJ_CHECK_STACK();
    qDebug()<<"registerred pjsip thread:"<<thread;

    this->dump_pjthread_info(thread);
    // while(isStackInit()) {
    //     pjsua_handle_events(10);
    // }
    qDebug()<<"enter pjsua thread loop supported by Qt";
    while (!quit_pjthread_loop) {
        evt_cnt = pjsua_handle_events(20);
        if (qrand() % 150 == 1 || evt_cnt > 0) {
            qDebug()<<"pjsua pool event in thread supported by Qt, proccess count:"<<evt_cnt;
        }
    }
}

void SipVarSet::dump_pjthread_info(pj_thread_t *thread)
{
    Q_ASSERT(thread != NULL);

    qDebug()<<"pj_thread_is_registered:"<<pj_thread_is_registered();
    qDebug()<<"pj_thread_get_prio:"<<pj_thread_get_prio(thread);
    qDebug()<<"pj_thread_get_prio_min:"<<pj_thread_get_prio_min(thread);
    qDebug()<<"pj_thread_get_prio_max:"<<pj_thread_get_prio_max(thread);
    qDebug()<<"pj_thread_get_name:"<<pj_thread_get_name(thread);
    qDebug()<<"pj_getpid:"<<pj_getpid();
}


void SipVarSet::registerSipTypes()
{
    int qid_pjsua_call_id = qRegisterMetaType<pjsua_call_id>("pjsua_call_id");
    int qid_pjsua_acc_id = qRegisterMetaType<pjsua_acc_id>("pjsua_acc_id");
    // pjsip_status_code
    int qid_pjsip_status_code = qRegisterMetaType<pjsip_status_code>("pjsip_status_code");
    Q_UNUSED(qid_pjsua_acc_id);
    Q_UNUSED(qid_pjsua_call_id);
    Q_UNUSED(qid_pjsip_status_code);
}

pj_status_t SipVarSet::sip_app_init()
{
   pjsua_config         ua_cfg;
   pjsua_logging_config log_cfg;
   pjsua_media_config   media_cfg;
   pj_status_t status;

   // Must create pjsua before anything else!
   status = pjsua_create();
   if (status != PJ_SUCCESS) {
       pjsua_perror(__FILE__, "Error initializing pjsua", status);
       return status;
   }

   // ::evht->start(); ///////////// integration qt and pjsip event
   this->start();

   // Initialize configs with default settings.
   pjsua_config_default(&ua_cfg);
   pjsua_logging_config_default(&log_cfg);
   pjsua_media_config_default(&media_cfg);

   char agent_str[100] = {0};
   snprintf(agent_str, sizeof(agent_str)-1, "PJSUA v%s/%s", pj_get_version(), PJ_OS_NAME);
   ua_cfg.user_agent = pj_str(agent_str);
   ua_cfg.thread_cnt = 1;
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

   media_cfg.has_ioqueue = PJ_TRUE;
   media_cfg.quality = PJSUA_DEFAULT_CODEC_QUALITY;
   media_cfg.jb_init = 200;
   media_cfg.jb_min_pre = 80; 
   media_cfg.jb_max_pre = 330;
   media_cfg.jb_max = 400;
   media_cfg.snd_auto_close_time = 1;
   // Customize other settings (or initialize them from application specific
   // configuration file):

   // Initialize pjsua
   status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
   if (status != PJ_SUCCESS) {
         pjsua_perror(__FILE__, "Error initializing pjsua", status);
         return status;
   }
   pjsua_var.mconf_cfg.samples_per_frame = 8000;

   /* Add TCP transport. */
   {
       pjsua_transport_config cfg;

       pjsua_transport_config_default(&cfg);
       cfg.port = 0;// 15678; // if not set , use random big port 
       // status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
       status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, NULL);
       if (status != PJ_SUCCESS) {
           pjsua_perror(__FILE__, "Error creating transport", status);
           // error_exit("Error creating transport", status);
           return status;
       }
   }

   /* Initialization is done, now start pjsua */
   status = pjsua_start();
   if (status != PJ_SUCCESS) {
       pjsua_perror(__FILE__, "Error starting pjsua", status);
       return status;
       // error_exit("Error starting pjsua", status);
   }

   pjsua_set_null_snd_dev();

   pjsua_recorder_id rec_id;                   
   char *rec_file_name = "/tmp/abcd.wav";
   pj_str_t pj_rec_file_name = pj_str(rec_file_name);
   unsigned short n_port = 0;
   // status = pjsua_recorder_create(&pj_rec_file_name, 0, NULL, -1, 0, &rec_id);
   status = pjsua_switcher_create(SUA_SWITCHER_SERVER, &n_port, 0, NULL, 0, &rec_id);
   qDebug()<<"record to :"<<rec_id<<n_port;
   this->rec_id = rec_id;

   // signals this event
   // emit this->sip_call_media_server_ready(n_port);
   // emit this->sip_call_media_actived(n_port);

   return status;
    
}

int SipVarSet::_find_account_from_pjacc(QString acc_name)
{
    QThread *curr_thread = this->thread();
    // this->moveToThread(::evht);
    int acc_count = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;

    acc_count = pjsua_acc_get_count();
    acc_count = sizeof(acc_ids)/sizeof(pjsua_acc_id);
    qDebug()<<"before enumed_accs:"<<acc_count;
    status = pjsua_enum_accs(acc_ids, (unsigned int*)&acc_count);
    qDebug()<<"enumed_accs:"<<acc_count<<status;
    if (status != PJ_SUCCESS) {
        // this->moveToThread(curr_thread);
        return -1;
    }
    for (int i = 0 ; i < acc_count ; ++i) {
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
            // this->moveToThread(curr_thread);
            return acc_info.id;
        }
    }

    // this->moveToThread(curr_thread);
    return -1;
}


int SipVarSet::_delete_account_by_active(int cnt)
{
    int acc_count = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;
    pjsua_acc_id old_acc_id = -1;
    unsigned old_acc_time = UINT_MAX;

    acc_count = pjsua_acc_get_count();
    acc_count = sizeof(acc_ids)/sizeof(pjsua_acc_id);
    status = pjsua_enum_accs(acc_ids, (unsigned int*)&acc_count);
    if (status != PJ_SUCCESS) {
        return -1;
    }
    for (int i = 0 ; i < acc_count ; ++i) {
        acc_id = acc_ids[i];
        if (pjsua_get_var()->acc[acc_id].cfg.unreg_timeout < old_acc_time) {
            old_acc_id = acc_ids[i];
        }
    }

    if (old_acc_id != -1) {
        if (pjsua_acc_get_default() == old_acc_id) {
        } else {
        }
        status = pjsua_acc_del(old_acc_id);
        return 1;
    }
    
    return 0;
}


pj_status_t SipVarSet::call_phone(QString caller_name, QString phone_number,
                                  QString serv_addr, int skypeCallID)
{
    pjsua_acc_id acc_id;
    pjsua_acc_info acc_info;
    pj_status_t status;
    char ubuf[256] = {0};

    static char *SIP_FROM_DOMAIN = "sips.qtchina.net:5060";

    this->mSkypeSipCallMap.insert(skypeCallID, -888888);

    struct acc_proc_args *targs = (struct acc_proc_args*)calloc(1, sizeof(struct acc_proc_args));
    strncpy(targs->caller_name, caller_name.toAscii().data(), caller_name.length());
    strncpy(targs->phone_number, phone_number.toAscii().data(), phone_number.length());
    strncpy(targs->serv_addr, serv_addr.toAscii().data(), serv_addr.length());
    targs->skypeCallID = skypeCallID;

//     pthread_t acc_mod_thread;
//     pthread_attr_t thread_attr;
//     pthread_attr_init(&thread_attr);
//     pthread_create(&acc_mod_thread, &thread_attr, &create_pjsua_acc_proc, targs);
//     pthread_detach(acc_mod_thread);

    qu1.enqueue(targs);
    cond1.wakeOne();
    
    return PJ_SUCCESS;

    acc_id = this->_find_account_from_pjacc(caller_name);
    if (0 && acc_id != -1) {
        qDebug()<<"found account in pool, do not need create a new one.";
        pjsua_get_var()->acc[acc_id].cfg.unreg_timeout = time(NULL);
        pjsua_acc_set_default(acc_id);
    }
    else
    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg;

        pjsua_acc_config_default(&cfg);
        // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
        // cfg.id = pj_str(SIP_USER " <sip:" SIP_USER "@"  "192.168.15.53:5678>");
        snprintf(ubuf, sizeof(ubuf) - 1, "%s <sip:%s@%s>",
                 caller_name.toAscii().data(), caller_name.toAscii().data(), SIP_FROM_DOMAIN);
        cfg.id = pj_str(ubuf);
        // cfg.id = pj_str(QString("%1 <sip:%1@%2>").arg(caller_name).arg(SIP_DOMAIN).toAscii().data());
        // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
        // cfg.reg_timeout = 800000000;
        // cfg.publish_enabled = PJ_FALSE;
        // cfg.reg_retry_interval = 0;
        cfg.cred_count = 1;
        // cfg.cred_info[0].realm = pj_str(SIP_FROM_DOMAIN);
        cfg.cred_info[0].realm = pj_str("*");
        // cfg.cred_info[0].realm = pj_str("192.168.15.53:5678");
        cfg.cred_info[0].scheme = pj_str("digest");
        // cfg.cred_info[0].username = pj_str(SIP_USER);
        cfg.cred_info[0].username = pj_str(caller_name.toAscii().data());
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        // cfg.cred_info[0].data = pj_str(SIP_PASSWD);
        cfg.cred_info[0].data = pj_str("88888888");
        cfg.unreg_timeout = time(NULL);

//         if (pjsua_acc_get_count() >= PJSUA_MAX_ACC) {
//             qDebug()<<"Account count will exseed max if add new one: max/now"
//                     <<PJSUA_MAX_ACC<<"/"<<pjsua_acc_get_count();
//             // delete one and add new
//             status = this->_delete_account_by_active(1);
//             qDebug()<<"A2ccount count will exseed max if add new one2: max/now"
//                     <<PJSUA_MAX_ACC<<"/"<<pjsua_acc_get_count();
//         }
	// PJSUA_LOCK();
        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error adding account", status);
            //   error_exit("Error adding account", status);
            qDebug()<<__FILE__<<__LINE__<<"Error adding account"<<status
                    <<QString::fromAscii(cfg.id.ptr, cfg.id.slen);
	    // PJSUA_UNLOCK();
            return status;
        }
        status = pjsua_acc_set_default(acc_id);
	// PJSUA_UNLOCK();
    }
    QString callee_phone = phone_number;
    QString sip_server = serv_addr; // "122.228.202.105:4060"; // this->ui->comboBox_2->currentText();
    // char *sipu = "<SIP:99008668056013552776960@122.228.202.105:4060;transport=UDP>";
    // char *sipu = strdup(QString("<SIP:%1@%2;transport=TCP>")
    //                        .arg(callee_phone).arg(sip_server) .toAscii().data());
    snprintf(ubuf, sizeof(ubuf) - 1, "<SIP:%s@%s;transport=TCP>",
             callee_phone.toAscii().data(), sip_server.toAscii().data());
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"call peer: "<<ubuf;

    pjsua_call_id call_id;
    pj_str_t uri = pj_str(ubuf);
    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, &call_id);
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error making call", status);
        qDebug()<<__FILE__<<__LINE__<<"Error making call"<<status;
        //error_exit("Error making call", status);
    }
    
    this->mSkypeSipCallMap.insert(skypeCallID, call_id);
    qDebug()<<"oncall slot returned";

    status = PJ_SUCCESS;
    return status;
}

// sip
void SipVarSet::on1_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id<<pjsua_call_has_media(call_id);
    pj_status_t status;
    pjsua_call_info ci;
    pjsua_acc_id acc_id;
 
    PJ_UNUSED_ARG(e);

    memcpy(&ci, pci, sizeof(ci));
    free(pci); pci = NULL;

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        qDebug()<<__FILE__<<__LINE__<<"call disconnected";

	//	acc_id = pjsua_acc_get_default();
	// status = pjsua_acc_del(acc_id);

        int i_call_id = call_id;
        emit this->sip_call_finished(i_call_id, ci.last_status);

    } else {
        if (ci.state == PJSIP_INV_STATE_CALLING) {
        }

        if (ci.state == PJSIP_INV_STATE_EARLY) {
            // only can ring, can not connect tow conf peer
        }

        if (ci.state == PJSIP_INV_STATE_CONFIRMED) {

        }
    }
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id<<ci.state
            <<QString::fromAscii(ci.state_text.ptr, ci.state_text.slen)
            <<"has media:"<<pjsua_call_has_media(call_id);
    PJ_LOG(3, (__FILE__, "Call %d state=%d %.*s", call_id,
               ci.state,
              (int)ci.state_text.slen,
              ci.state_text.ptr));

}

void SipVarSet::on1_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pjsua_call_info ci;
    pj_status_t status;

    memcpy(&ci, pci, sizeof(ci));
    free(pci); pci = NULL;    
    // return; // we need early media, early call this
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        // pjsua_conf_connect(ci.conf_slot, 0);
        // pjsua_conf_connect(0, ci.conf_slot);

        // test port_info
        pjsua_conf_port_info cpi;
        pjsua_conf_get_port_info(ci.conf_slot, &cpi);
        qDebug()<<"conf port info: port number="<<cpi.slot_id<<",name='"<<cpi.name.ptr
                <<"', chan cnt="<<cpi.channel_count
                <<", clock rate="<<cpi.clock_rate;

        // put frame from pjmedia to standard tcp port: this->rec_id
        pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(this->rec_id));
        pjsua_conf_connect(pjsua_recorder_get_conf_port(this->rec_id), ci.conf_slot);

        pjmedia_transport *mtp = pjsua_call_get_media_transport(call_id);
        pjmedia_session *ms = pjsua_call_get_media_session(call_id);
        pjmedia_port *mp = NULL;
        unsigned int stream_count = 100;
        pjmedia_stream_info msi[100];
        pjmedia_frame from_lang_frame;
        int i = 0;
        unsigned short n_port = 0, incoming_n_port = 0;

        pjsua_switcher_get_port(this->rec_id, &mp);
        n_port = pjsua_switcher_get_outgoing_net_port(mp);
        incoming_n_port = pjsua_switcher_get_incoming_net_port(mp);
        qDebug()<<__FILE__<<__LINE__<<"got wav server port :"<<n_port<<incoming_n_port;

        // ip -> pstn first
        emit this->sip_call_incoming_media_server_ready(call_id, incoming_n_port);        
        emit this->sip_call_media_server_ready(call_id, n_port);
        
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

    /* Handle media status */
    switch (ci.media_status) {
    case PJSUA_CALL_MEDIA_ACTIVE:
        PJ_LOG(3,(THIS_FILE, "Media for call %d is active", call_id));
        break;

    case PJSUA_CALL_MEDIA_LOCAL_HOLD:
        PJ_LOG(3,(THIS_FILE, "Media for call %d is suspended (hold) by local",
                  call_id));
        break;

    case PJSUA_CALL_MEDIA_REMOTE_HOLD:
        PJ_LOG(3,(THIS_FILE,
                  "Media for call %d is suspended (hold) by remote",
                  call_id));
        break;

    case PJSUA_CALL_MEDIA_ERROR:
        PJ_LOG(3,(THIS_FILE,
                  "Media has reported error, disconnecting call"));
        {
            pj_str_t reason = pj_str("ICE negotiation failed");
            pjsua_call_hangup(call_id, 500, &reason, NULL);
        }
        break;

    case PJSUA_CALL_MEDIA_NONE:
        PJ_LOG(3,(THIS_FILE,
                  "Media for call %d is inactive",
                  call_id));
        break;

    default:
        pj_assert(!"Unhandled media status");
        break;
    }
    // pjsua_dump(PJ_TRUE);
    // pjsua_call_dump(call_id, ...);
}

void SipVarSet::on1_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
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

void SipVarSet::on1_user_added(int skype_call_id, pjsua_call_id sip_call_id, int tid)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skype_call_id<<sip_call_id<<tid;

    pj_status_t status;
    unsigned code = 0;
    pj_str_t reason = pj_str("finished");
    pjsua_msg_data msg;
    pjsua_call_id call_id;

    if (sip_call_id == -555555) {
      // make call error;
      
    }

    if (!this->mSkypeSipCallMap.leftContains(skype_call_id)) {
      qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Can not found map skype call id";
      // not possible
    } else {
      call_id = this->mSkypeSipCallMap.findLeft(skype_call_id).value();
      if (call_id == -888888) {
	// wait for sip return;
	this->mSkypeSipCallMap.insert(skype_call_id, sip_call_id);  
      } if (call_id == -666666) {
	this->mSkypeSipCallMap.insert(skype_call_id, sip_call_id);  
	this->hangup_call(skype_call_id);
      } else {
	// status = pjsua_call_hangup(call_id, code, NULL, NULL);
      }
    }
}

void SipVarSet::on1_exceed_max_call_count(int skype_call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skype_call_id;

    pj_status_t status;
    unsigned code = 0;
    pj_str_t reason = pj_str("finished");
    pjsua_msg_data msg;
    pjsua_call_id sip_call_id;

    if (!this->mSkypeSipCallMap.leftContains(skype_call_id)) {
      // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Can not found map skype call id";
      // not possible
    } else {
      sip_call_id = this->mSkypeSipCallMap.findLeft(skype_call_id).value();
      if (sip_call_id == -888888) {
	// wait for sip return;
	// this->mSkypeSipCallMap.insert(skype_call_id, sip_call_id);  
      } if (sip_call_id == -666666) {

      } else {
	// status = pjsua_call_hangup(call_id, code, NULL, NULL);
      }
      this->mSkypeSipCallMap.removeLeft(skype_call_id);
      emit this->sip_call_exceed_max_count(skype_call_id);
    }
    
}

void SipVarSet::on1_new_connection(void *m_port)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
    pj_status_t status;
    tcp_port *f_port = (tcp_port*)m_port;
    tcp_port *fport = f_port;

    /*
                                                    pjsua_var.media_cfg.clock_rate, 
                                                    pjsua_var.mconf_cfg.channel_count,
                                                    pjsua_var.mconf_cfg.samples_per_frame,
                                                    pjsua_var.mconf_cfg.bits_per_sample, 

     */
    
    pj_int16_t channel_count = pjsua_var.mconf_cfg.channel_count;
    unsigned sampling_rate = pjsua_var.mconf_cfg.samples_per_frame;
    
    // f_port->cli_handle = f_port->serv_handle->nextPendingConnection();
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;// <<f_port->cli_handle->peerPort();

    // write file need header, redirect to skype, not need header
    bool need_wav_header = true;
    if (!need_wav_header) {
        return;
    }
    // try write wav header
    pjmedia_wave_hdr wave_hdr;
    pj_ssize_t size;

    /* Initialize WAVE header */
    pj_bzero(&wave_hdr, sizeof(pjmedia_wave_hdr));
    wave_hdr.riff_hdr.riff = PJMEDIA_RIFF_TAG;
    wave_hdr.riff_hdr.file_len = 1024*1024*500; /* will be filled later */
    wave_hdr.riff_hdr.wave = PJMEDIA_WAVE_TAG;

    wave_hdr.fmt_hdr.fmt = PJMEDIA_FMT_TAG;
    wave_hdr.fmt_hdr.len = 16;
    wave_hdr.fmt_hdr.fmt_tag = (pj_uint16_t)fport->fmt_tag;
    wave_hdr.fmt_hdr.nchan = (pj_int16_t)channel_count;
    wave_hdr.fmt_hdr.sample_rate = sampling_rate;
    wave_hdr.fmt_hdr.bytes_per_sec = sampling_rate * channel_count *
        fport->bytes_per_sample;
    wave_hdr.fmt_hdr.block_align = (pj_uint16_t)
        (fport->bytes_per_sample * channel_count);
    wave_hdr.fmt_hdr.bits_per_sample = (pj_uint16_t)
        (fport->bytes_per_sample * 8);

    wave_hdr.data_hdr.data = PJMEDIA_DATA_TAG;
    wave_hdr.data_hdr.len = 0;      /* will be filled later */


    /* Convert WAVE header from host byte order to little endian
     * before writing the header.
     */
    pjmedia_wave_hdr_host_to_file(&wave_hdr);

    /* Write WAVE header */
    if (fport->fmt_tag != PJMEDIA_WAVE_FMT_TAG_PCM) {
        pjmedia_wave_subchunk fact_chunk;
        pj_uint32_t tmp = 0;

        fact_chunk.id = PJMEDIA_FACT_TAG;
        fact_chunk.len = 4;

        PJMEDIA_WAVE_NORMALIZE_SUBCHUNK(&fact_chunk);

        /* Write WAVE header without DATA chunk header */
        size = sizeof(pjmedia_wave_hdr) - sizeof(wave_hdr.data_hdr);
        // status = pj_file_write(fport->fd, &wave_hdr, &size);
        // if (status != PJ_SUCCESS) {
        //     pj_file_close(fport->fd);
        //     return status;
        // }
        // f_port->cli_handle->write((char*)&wave_hdr, size);

        /* Write FACT chunk if it stores compressed data */
        size = sizeof(fact_chunk);
        // status = pj_file_write(fport->fd, &fact_chunk, &size);
        // if (status != PJ_SUCCESS) {
        //     pj_file_close(fport->fd);
        //     return status;
        // }
        // f_port->cli_handle->write((char*)&fact_chunk, size);

        size = 4;
        // status = pj_file_write(fport->fd, &tmp, &size);
        // if (status != PJ_SUCCESS) {
        //     pj_file_close(fport->fd);
        //     return status;
        // }
        // f_port->cli_handle->write((char*)&tmp, size);

        /* Write DATA chunk header */
        size = sizeof(wave_hdr.data_hdr);
        // status = pj_file_write(fport->fd, &wave_hdr.data_hdr, &size);
        // if (status != PJ_SUCCESS) {
        //     pj_file_close(fport->fd);
        //     return status;
        // }
        // f_port->cli_handle->write((char*)&wave_hdr.data_hdr, size);
    } else {
        size = sizeof(pjmedia_wave_hdr);
        // status = pj_file_write(fport->fd, &wave_hdr, &size);
        // if (status != PJ_SUCCESS) {
        //     pj_file_close(fport->fd);
        //     return status;
        // }
        // f_port->cli_handle->write((char*)&wave_hdr, size);
    }

}

void SipVarSet::on1_new_incoming_connection(void *m_port)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
    pj_status_t status;
    tcp_port *f_port = (tcp_port*)m_port;
    tcp_port *fport = f_port;

    /*
                                                    pjsua_var.media_cfg.clock_rate, 
                                                    pjsua_var.mconf_cfg.channel_count,
                                                    pjsua_var.mconf_cfg.samples_per_frame,
                                                    pjsua_var.mconf_cfg.bits_per_sample, 

     */
    
    pj_int16_t channel_count = pjsua_var.mconf_cfg.channel_count;
    unsigned sampling_rate = pjsua_var.mconf_cfg.samples_per_frame;
    
    // f_port->incoming_cli_handle = f_port->incoming_serv_handle->nextPendingConnection();
    // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port<<f_port->incoming_cli_handle->peerPort();
    // QObject::connect(f_port->incoming_cli_handle, SIGNAL(readyRead()),
    // f_port, SLOT(onIncomingReadyRead()));
}

// depcreated
void SipVarSet::on1_put_frame(QTcpSocket *sock, QByteArray fba)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sock<<fba.length();
    sock->write(fba);
}

void SipVarSet::hangup_call(int skypeCallID)
{
    // pjsua_call_hangup (pjsua_call_id call_id, unsigned code, const pj_str_t *reason, const pjsua_msg_data *msg_data)
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;

    pj_status_t status;
    unsigned code = 0;
    pj_str_t reason = pj_str("finished");
    pjsua_msg_data msg;
    pjsua_call_id sip_call_id;

    if (!this->mSkypeSipCallMap.leftContains(skypeCallID)) {
      qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Can not found map skype call id";
    } else {
      sip_call_id = this->mSkypeSipCallMap.findLeft(skypeCallID).value();
      qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"fetch call pair:"<<skypeCallID<<sip_call_id;
      if (sip_call_id == -888888) {
	// wait for sip return;
	this->mSkypeSipCallMap.insert(skypeCallID, -666666);
      } else if (sip_call_id == -666666) {
	
      } else if (sip_call_id >= 0) {
	status = pjsua_call_hangup(sip_call_id, code, NULL, NULL);
	qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"sip call hangup:"<<status;
      } else {
	qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"sip call id error:"<<sip_call_id;
      }
    }
}

void SipVarSet::hangup_all_call()
{
    pjsua_call_hangup_all();
}


