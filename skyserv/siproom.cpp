// siproom.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-07 11:06:59 +0800
// Version: $Id: siproom.cpp 908 2011-06-02 09:37:45Z drswinghead $
// 

#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <QtCore>
#include <QtNetwork>

#include <pjmedia/wave.h>
#include <pjmedia/wav_port.h>

#include "../utils.h"
#include "../simplelog.h"

#include "websocket.h"

#include "sip_proc.h"
#include "siproom.h"
#include "sxs_switcher.h"

#define THIS_FILE __FILE__

bool quit_sip_proc = false;
pjsua_recorder_id g_rec_id = -1;
sip_proc_param_t *g_param = NULL;
QQueue<char*> sock_out_data_queue;
QHash<int, sip_call_prop_t*> sip_calls;  // sip_call_id => prop

int set_sock_nonblock(int sock)
{
    unsigned long flags = 0;
#ifdef WIN32
    flags = 1;
    return (ioctlsocket(sock, FIONBIO, &flags) != SOCKET_ERROR);
#else
    flags = fcntl(sock, F_GETFL, NULL);
    flags |= O_NONBLOCK;
    return (fcntl(sock, F_SETFL, flags) >= 0);
#endif
}

int set_sock_block(int sock)
{
    unsigned long flags = 0;
#ifdef WIN32

#else
    flags = fcntl(sock, F_GETFL, NULL);
    assert(flags >= 0);
    flags |= (~O_NONBLOCK);
    return (fcntl(sock, F_SETFL, flags) >=0);
#endif
    return 1;
}


// sip
void on1_call_state(pjsua_call_id sip_call_id, pjsip_event *e)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id<<pjsua_call_has_media(sip_call_id);
    pj_status_t status;
    pjsua_call_info ci;
    pjsua_acc_id acc_id;
    int skype_call_id;
    sip_call_prop_t *scp = 0;
 
    PJ_UNUSED_ARG(e);

    status = pjsua_call_get_info(sip_call_id, &ci);

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        qDebug()<<__FILE__<<__LINE__<<"call disconnected";
        assert(sip_calls.contains(sip_call_id));
        scp = sip_calls.value(sip_call_id);
        skype_call_id = scp->skype_call_id;
        if (scp->hangup_point == INIT_HANGUP_FROM_SKYPE) {
            sip_hangup_call(skype_call_id, sip_call_id, 0, 0);
        } else {
            sip_hangup_call(skype_call_id, sip_call_id, 1, ci.last_status);
        }
    } else {
        if (ci.state == PJSIP_INV_STATE_CALLING) {
        }

        if (ci.state == PJSIP_INV_STATE_EARLY) {
            // only can ring, can not connect tow conf peer
            qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"How deal early media?";
        }

        if (ci.state == PJSIP_INV_STATE_CONFIRMED) {

        }
    }

    ////////// send call state command
    if (sip_calls.contains(sip_call_id)) {
        assert(sip_calls.contains(sip_call_id));
        scp = sip_calls.value(sip_call_id);
        skype_call_id = scp->skype_call_id;

        int cmdlen = 0;
        char *wbuf = new_rpc_command(19, &cmdlen, "ddu", skype_call_id, sip_call_id, ci.state);
        sock_out_data_queue.enqueue(wbuf);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = g_param->peer_fd;
        cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Ooops... relation map broken.";
    }
    
    //// loging
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id<<ci.state
            <<QString::fromAscii(ci.state_text.ptr, ci.state_text.slen)
            <<ci.last_status
            <<QString::fromAscii(ci.last_status_text.ptr, ci.last_status_text.slen)
            <<ci.media_status
            <<"has media:"<<pjsua_call_has_media(sip_call_id);
    PJ_LOG(3, (__FILE__, "Call %d state=%d %.*s, sipcode=%d,%.*s", sip_call_id,
               ci.state,
               (int)ci.state_text.slen,
               ci.state_text.ptr,
               ci.last_status,
               ci.last_status_text.slen,
               ci.last_status_text.ptr)
           );
}

void on1_call_media_state(pjsua_call_id sip_call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id;
    pjsua_call_info ci;
    pj_status_t status;
    int skype_call_id;
    sip_call_prop_t *scp = 0;

    status = pjsua_call_get_info(sip_call_id, &ci);

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
        pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(g_rec_id));
        pjsua_conf_connect(pjsua_recorder_get_conf_port(g_rec_id), ci.conf_slot);

        pjmedia_transport *mtp = pjsua_call_get_media_transport(sip_call_id);
        pjmedia_port *mp = NULL;
        int i = 0;
        unsigned short n_port = 0, incoming_n_port = 0;

        pjsua_sxs_switcher_get_port(g_rec_id, &mp);
        // n_port = pjsua_switcher_get_outgoing_net_port(mp);
        // incoming_n_port = pjsua_switcher_get_incoming_net_port(mp);
        n_port = pjmedia_sxs_port_outgoing_server_get_port(mp, 0);
        incoming_n_port = pjmedia_sxs_port_incoming_server_get_port(mp, 0);
        qDebug()<<__FILE__<<__LINE__<<"got wav server port :"<<n_port<<incoming_n_port;

        scp = sip_calls.value(sip_call_id);

        if (ci.state != PJSIP_INV_STATE_CONFIRMED) {
            int cmdlen = 0;
            char *wbuf = new_rpc_command(15, &cmdlen, "ddu", scp->skype_call_id, sip_call_id, incoming_n_port);
            sock_out_data_queue.enqueue(wbuf);

            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLOUT;
            ev.data.fd = g_param->peer_fd;
            cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);

            //// 
            cmdlen = 0;
            wbuf = new_rpc_command(17, &cmdlen, "ddu", scp->skype_call_id, sip_call_id, n_port);
            sock_out_data_queue.enqueue(wbuf);
        
            cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);
        }
    }

    ////////// send media status command
    if (sip_calls.contains(sip_call_id) & ci.state != PJSIP_INV_STATE_CONFIRMED) {
        assert(sip_calls.contains(sip_call_id));
        scp = sip_calls.value(sip_call_id);
        skype_call_id = scp->skype_call_id;

        int cmdlen = 0;
        char *wbuf = new_rpc_command(21, &cmdlen, "ddu", skype_call_id, sip_call_id, ci.media_status);

        sock_out_data_queue.enqueue(wbuf);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = g_param->peer_fd;
        cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);
    }

    /* Handle media status */
    switch (ci.media_status) {
    case PJSUA_CALL_MEDIA_ACTIVE:
        PJ_LOG(3,(THIS_FILE, "Media for call %d is active", sip_call_id));
        break;

    case PJSUA_CALL_MEDIA_LOCAL_HOLD:
        PJ_LOG(3,(THIS_FILE, "Media for call %d is suspended (hold) by local",
                  sip_call_id));
        break;

    case PJSUA_CALL_MEDIA_REMOTE_HOLD:
        PJ_LOG(3,(THIS_FILE,
                  "Media for call %d is suspended (hold) by remote",
                  sip_call_id));
        break;

    case PJSUA_CALL_MEDIA_ERROR:
        PJ_LOG(3,(THIS_FILE,
                  "Media has reported error, disconnecting call"));
        {
            pj_str_t reason = pj_str("ICE negotiation failed");
            pjsua_call_hangup(sip_call_id, 500, &reason, NULL);
        }
        break;

    case PJSUA_CALL_MEDIA_NONE:
        PJ_LOG(3,(THIS_FILE,
                  "Media for call %d is inactive",
                  sip_call_id));
        break;

    default:
        pj_assert(!"Unhandled media status");
        break;
    }
    // pjsua_dump(PJ_TRUE);
    // pjsua_call_dump(call_id, ...);
}

void on1_dtmf_digit(pjsua_call_id sip_call_id, int digit)
{
  // 
}

static int sr_find_account_from_pjacc(char * acc_name)
{
    int acc_count = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;

    acc_count = pjsua_acc_get_count();
    acc_count = sizeof(acc_ids)/sizeof(pjsua_acc_id);
    // qDebug()<<"before enumed_accs:"<<acc_count;
    status = pjsua_enum_accs(acc_ids, (unsigned int*)&acc_count);
    // qDebug()<<"enumed_accs:"<<acc_count<<status;
    if (status != PJ_SUCCESS) {
        return -1;
    }
    for (int i = 0 ; i < acc_count ; ++i) {
        status = pjsua_acc_get_info(acc_ids[i], &acc_info);
        if (status != PJ_SUCCESS) {
            continue;
        }

        acc_uri = QString::fromAscii(acc_info.acc_uri.ptr, acc_info.acc_uri.slen);
        qDebug()<<"acc_uri for "<<acc_id<<" " << acc_uri;
        if (acc_uri.split(" ").at(0) == acc_name) {
            if (!pjsua_acc_is_valid(acc_info.id)) {
                qDebug()<<"Warning: acc id is not valid:"<<acc_info.id<<" "<<acc_uri;
            }
            return acc_info.id;
        }
    }

    return -1;
}


static int sr_delete_account_by_active(int cnt)
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

static int sip_check_transport()
{
    pjsua_transport_id ids[20];
    unsigned int cnt;
    pj_status_t status;
    pjsua_transport_info info;
    char buf[100] = {0};
    char buf2[100] = {0};
    
    status = pjsua_enum_transports(ids, &cnt);
    assert(status == PJ_SUCCESS);

    fprintf(stderr, "Got transport count: %d\n", cnt);
    for (int i = 0; i < cnt; ++i) {
        status = pjsua_transport_get_info(ids[i], &info);
        assert(status == PJ_SUCCESS);

        memcpy(buf, info.type_name.ptr, info.type_name.slen);
        memcpy(buf2, info.local_name.host.ptr, info.local_name.host.slen);
        fprintf(stderr, "Transport info: id=%d, type=%s, ref_count=%d, addr=%s:%d\n",
                info.id, buf, info.usage_count, buf2, info.local_name.port);
    }

    return 0;
}

static int sip_app_init()
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
    ua_cfg.cb.on_call_state = &on1_call_state;
    ua_cfg.cb.on_call_media_state = &on1_call_media_state;

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
        pjsua_transport_config rtp_cfg;
        pjsua_transport_config cfg;

        QString inter_ipaddr = WebSocketServer2::serverIpAddr(0);

        // 创建RTP语音流传输层
        pjsua_transport_config_default(&rtp_cfg);
        rtp_cfg.port = 4000; //
        rtp_cfg.public_addr = pj_str(strdup(inter_ipaddr.toAscii().data()));
        rtp_cfg.bound_addr = pj_str(strdup(inter_ipaddr.toAscii().data()));
        status = pjsua_media_transports_create(&rtp_cfg);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error creating RTP media transport", status);
            return status;
        }
        
        // 创建SIP协议传输层,使用TCP传输协议。
        pjsua_transport_config_default(&cfg);
        cfg.port = 0;// 15678; // if not set , use random big port 
        // 默认的cfg会使用公网IP，这是否可以指定内网IP呢。
        // cfg.public_addr = pj_str(strdup("aaaaaaa"));
        // status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, NULL);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error creating transport", status);
            // error_exit("Error creating transport", status);
            return status;
        }
        qLogx()<<"Transport created::"<<QByteArray(cfg.public_addr.ptr, cfg.public_addr.slen)
               <<QByteArray(cfg.bound_addr.ptr, cfg.bound_addr.slen)
               <<inter_ipaddr;
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error starting pjsua", status);
        return status;
        // error_exit("Error starting pjsua", status);
    }

    pjsua_set_null_snd_dev();

    // create pool
    g_param->pool = pjsua_pool_create("poooooooooooooool", 1000, 1000);
    assert(g_param->pool);

    pjsua_recorder_id rec_id;                   
    char *rec_file_name = "/tmp/abcd.wav";
    pj_str_t pj_rec_file_name = pj_str(rec_file_name);
    unsigned short n_port = 0;
    // status = pjsua_switcher_create(SUA_SWITCHER_SERVER, &n_port, 0, NULL, 0, &rec_id);
    status = pjsua_sxs_switcher_create(SUA_SWITCHER_SERVER, 0, NULL, 0, &rec_id);
    // qDebug()<<"record to :"<<rec_id<<n_port;
    g_rec_id = rec_id;

    status = pjsua_sxs_switcher_get_port(rec_id, &g_param->mport);

    // check transport
    sip_check_transport();
   
    return 0;
}

// caller_name, phone_number, serv_addr, skype_call_id, caller_ipaddr
int sip_call_phone(char *arg_str)
{
    qDebug()<<"oncall slot returned2";

    pjsua_acc_id acc_id;
    pjsua_acc_info acc_info;
    pj_status_t status;
    char ubuf[256] = {0};
    QStringList args = QString(arg_str).split(",");

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

    // check transport num
    sip_check_transport();

    qDebug()<<"max allowed call count:"<<pjsua_call_get_max_count()
            <<"current call count:"<<pjsua_call_get_count();

    if (pjsua_call_get_count() > 0) {
        pjsua_call_hangup_all();
    }

    if (pjsua_call_get_count() >= pjsua_call_get_max_count()) {
        // PjCallback::on_exceed_max_call_count_wrapper(targs->skypeCallID);
        return 0;
    }

    // static char *SIP_FROM_DOMAIN = "sips.qtchina.net:5060";
    // memset(ubuf, 0, sizeof(ubuf));
    // snprintf(ubuf, sizeof(ubuf) - 1, "sips.qtchina.net-%s:5060", args.at(4).toAscii().data());
    // SIP_FROM_DOMAIN = ubuf;

    acc_id = sr_find_account_from_pjacc(args.at(0).toAscii().data());
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
            memset(ubuf, 0, sizeof(ubuf));
            // snprintf(ubuf, sizeof(ubuf) - 1, "%s <sip:%s@%s>",
            //          args.at(0).toAscii().data(), args.at(0).toAscii().data(), SIP_FROM_DOMAIN);
            snprintf(ubuf, sizeof(ubuf) - 1, "%s <sip:%s@sips.qtchina.net-%s-at-%s:5060>",
                     args.at(0).toAscii().data(), args.at(0).toAscii().data(), args.at(0).toAscii().data(),
                     args.at(4).toAscii().data());
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
            cfg.cred_info[0].username = pj_str(args.at(0).toAscii().data());
            cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
            // cfg.cred_info[0].data = pj_str(SIP_PASSWD);
            cfg.cred_info[0].data = pj_str("88888888");
            cfg.unreg_timeout = time(NULL);

            if (pjsua_acc_get_count() >= PJSUA_MAX_ACC) {
                qDebug()<<"Account count will exseed max if add new one: max/now"
                        <<PJSUA_MAX_ACC<<"/"<<pjsua_acc_get_count();
                // delete one and add new
                status = sr_delete_account_by_active(1);
                qDebug()<<"A2ccount count will exseed max if add new one2: max/now"
                        <<PJSUA_MAX_ACC<<"/"<<pjsua_acc_get_count();
            }
            status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
            if (status != PJ_SUCCESS) {
                pjsua_perror(__FILE__, "Error adding account", status);
                //   error_exit("Error adding account", status);
                qDebug()<<__FILE__<<__LINE__<<"Error adding account"<<status
                        <<QString::fromAscii(cfg.id.ptr, cfg.id.slen);
                return status;
            }
            status = pjsua_acc_set_default(acc_id);
        }
    QString callee_phone = args.at(1);
    QString sip_server = args.at(2); // "122.228.202.105:4060"; // this->ui->comboBox_2->currentText();
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
        return -1;
    }

    int cmdlen = 0;
    char *wbuf = new_rpc_command(11, &cmdlen, "dd", args.at(3).toInt(), call_id);
    sock_out_data_queue.enqueue(wbuf);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = g_param->peer_fd;
    cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);

    // PjCallback::on_user_added_wrapper(targs->skypeCallID, call_id, pthread_self());    
    // this->mSkypeSipCallMap.insert(skypeCallID, call_id);
    // _this->mSkypeSipCallMap.insert(targs->skypeCallID, call_id);

    qDebug()<<"oncall slot returned"<<cmdlen<<strerror(errno);

    sip_call_prop_t *scp = (sip_call_prop_t*)calloc(1, sizeof(sip_call_prop_t));
    memset(scp, 0, sizeof(sip_call_prop_t));
    scp->sip_call_id = call_id;
    scp->skype_call_id = args.at(3).toInt();
    scp->rec_id = g_rec_id;

    sip_calls.insert((int)call_id, scp);

    // sleep(20);
    // pthread_detach(pthread_self());

    status = PJ_SUCCESS;
    return status;     
}

/*
  hangup_skype, 是否要向skype发送挂断命令，可能挂断是从skype发起的。
 */
void sip_hangup_call(int skype_call_id, int sip_call_id, int hangup_skype, int last_status)
{
    // pjsua_call_hangup (pjsua_call_id call_id, unsigned code, const pj_str_t *reason, const pjsua_msg_data *msg_data)
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skype_call_id<<sip_call_id;

    pj_status_t status;
    unsigned code = 0;
    pj_str_t reason = pj_str("finished");
    pjsua_msg_data msg;

    if (pjsua_call_is_active(sip_call_id)) {
        status = pjsua_call_hangup(sip_call_id, code, NULL, NULL);
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"sip call hangup:"<<status;
    } else {
        // why this call is not active????
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"sip call not exist, or already hangup:"<<sip_call_id;
    }

    if (hangup_skype) {
        int cmdlen = 0;
        char *wbuf = new_rpc_command(13, &cmdlen, "ddd", skype_call_id, sip_call_id, last_status);
        sock_out_data_queue.enqueue(wbuf);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = g_param->peer_fd;
        cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);
    }
}


// cmdlen/cmdno/cmdbuf
int process_incoming_command(int fd)
{
  pj_status_t status;
    int cmdlen = 0;
    int cmdno = 0;
    char cmdbuf[200] = {0};
    int rlen = 0;
    QStringList args;
    int skype_call_id;
    int sip_call_id;
    sip_call_prop_t *scp = 0;
    pj_str_t str;  // maybe 
    pjsua_call_info ci;

    rlen = ::read(fd, &cmdlen, sizeof(int));
    rlen = ::read(fd, &cmdno, sizeof(int));
    rlen = ::read(fd, cmdbuf, cmdlen);

    fprintf(stderr, "%s %d recv cmd, len: %d, no: %d, buf: '%s'\n", __FILE__, __LINE__,
            cmdlen, cmdno, cmdbuf);

    if (rlen == 0) {
        // maybe close event
        return 1;
    }

    args = QString(cmdbuf).split(",");

    switch (cmdno) {
    case 10: // init sip call
        sip_call_phone(cmdbuf);
        break;
    case 12: // hangup call
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();
        assert(sip_calls.contains(sip_call_id));
        scp = sip_calls.value(sip_call_id);
        scp->hangup_point = INIT_HANGUP_FROM_SKYPE;
        sip_hangup_call(args.at(0).toInt(), args.at(1).toInt(), 0, 0);
        break;
    case 16: // recieve dtmf
        skype_call_id = args.at(0).toInt();
        sip_call_id = args.at(1).toInt();
        assert(sip_calls.contains(sip_call_id));
        // str maybe 1 digit, or more digits
        str = pj_str(args.at(2).toAscii().data());
        status = pjsua_call_dial_dtmf(sip_call_id, &str);
        break;
    case 18: // hold/unhold call
        sip_call_id = args.at(1).toInt();
        assert(sip_calls.contains(sip_call_id));
        status = pjsua_call_get_info(sip_call_id, &ci);
        rlen = args.at(2).toInt();
        if (rlen == 1) {
            if (ci.media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD 
                || ci.media_status == PJSUA_CALL_MEDIA_REMOTE_HOLD) {
                fprintf(stderr, "Call %d media state is already in hold, %d\n",
                        sip_call_id, ci.media_status);
            } else {
                status = pjsua_call_set_hold(sip_call_id, NULL);
            }
        } else {
            if (ci.media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD
                || ci.media_status == PJSUA_CALL_MEDIA_REMOTE_HOLD) {
                status = pjsua_call_reinvite(sip_call_id, PJ_TRUE, NULL);
            } else {
                fprintf(stderr, "Call %d media state is already in unhold, %d\n",
                        sip_call_id, ci.media_status);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

int process_output_command(int fd)
{
    int ret, i = 0;
    int cmdno = 0;
    int cmdlen = -1;
    char *wbuf = NULL;
    struct epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.fd = fd;

    epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, fd, &ev);

    // flush twice if has
    while (!sock_out_data_queue.isEmpty() && (i++ < 9)) {
        wbuf = sock_out_data_queue.dequeue();
        memcpy(&cmdlen, wbuf, sizeof(int));
	memcpy(&cmdno, wbuf + sizeof(int), sizeof(int));
    
        ret = ::write(fd, wbuf, cmdlen + 2*sizeof(int));
        fprintf(stderr, "siproom: write back: %d, %d, %d, %s\n", cmdlen, ret, cmdno, wbuf + 2*sizeof(int));

        free(wbuf);
    }

    return 0;
}

#define MAX_EVENTS 60
void *sip_main_proc(void *args)
{
    // sip_proc_param_t *param = (sip_proc_param_t *)args;
    unsigned short *p_peer_port = (unsigned short*)args;
    unsigned short peer_port = (unsigned short)(*p_peer_port);
    free(p_peer_port); p_peer_port = NULL;
    args = NULL;

    int ret;
    socklen_t slen;
    int epollfd, nfds, n;
    struct epoll_event ev, events[60];

    sip_proc_param_t *param = (sip_proc_param_t *)calloc(1, sizeof(sip_proc_param_t));
    memset(param, 0, sizeof(sip_proc_param_t));
    param->peer_port = peer_port;

    g_param = param;
    epollfd = epoll_create(20);
    if (epollfd == -1) {
        perror("epoll_create");
        assert(epollfd != -1);
    }
    param->epfd = epollfd;

    sip_app_init();

    srandom(time(NULL));
    fprintf(stderr, "Enter pjapp thread loop, rpc with: %u\n", param->peer_port);
    
    // connect to com serv
    param->peer_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(param->peer_port);
    inet_pton(AF_INET, "127.0.0.1", &sock_addr.sin_addr.s_addr);
    slen = sizeof(sock_addr);

    fprintf(stderr, "connect to : %d\n", param->peer_port);
    ret = ::connect(param->peer_fd, (struct sockaddr*)&sock_addr, slen);
    if (ret != 0) {
        perror(strerror(errno));
        assert(ret == 0);
    }

    set_sock_nonblock(param->peer_fd);

    ev.events = EPOLLIN;
    ev.data.fd = param->peer_fd;
    if ((ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, param->peer_fd, &ev)) == -1) {
        perror("epoll_ctl: add");
        assert(ret != -1);
    }

    for (;!quit_sip_proc;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            // perror("why, epoll_pwait");
            qlog("epoll_pwait: %d, %s\n", errno, strerror(errno));
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].events & EPOLLIN) {
                if (events[n].data.fd == param->peer_fd) {
                    process_incoming_command(param->peer_fd);
                    fprintf(stderr, "process incomong command done\n");
                } else if(events[n].data.fd == param->mpo_serv_sock) {
                    param->mpo_cli_sock = ::accept(param->mpo_serv_sock, (struct sockaddr*)&sock_addr,
                                                   &slen);
                    qDebug()<<"accepted out client connection: "<<param->mpo_cli_sock
                            <<ntohs(sock_addr.sin_port);
                    if (param->mpo_cli_sock == -1) {
                        perror("accept out serv");
                        exit(-2);
                    }

                    // set_sock_nonblock(param->mpo_cli_sock);

                    ev.data.fd = param->mpo_cli_sock;
                    ev.events = EPOLLIN;
                    if ((ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, param->mpo_cli_sock, &ev)) != 0) {
                        perror("epoll add out cli read event");
                        exit(-3);
                    }
                    pjmedia_sxs_outgoing_sock_connected(param->mport, param->mpo_cli_sock);
                    // this->out_write_start_time = time(NULL);
                    // fport->base.get_frame = &file_get_frame;
                    // this->base.put_frame = &file_put_frame;
                } else if (events[n].data.fd == param->mpo_cli_sock) {
                    // close it
                    // should be close event
                    pjmedia_sxs_close_outgoing_cli(param->mport, param->mpo_cli_sock);
                    ret = param->mpo_cli_sock; // 
                    param->mpo_cli_sock = -1;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, ret, NULL);
                    close(ret);
                    qDebug()<<__FILE__<<__LINE__<<"close out client socket";
                    // this->self_ref.base.put_frame = NULL;

                } else if (events[n].data.fd == param->mpi_serv_sock) {
                    param->mpi_cli_sock = ::accept(param->mpi_serv_sock, (struct sockaddr*)&sock_addr, &slen);
                    qDebug()<<"accepted in client connection: "<<param->mpi_cli_sock
                            <<ntohs(sock_addr.sin_port);
                    if (param->mpi_cli_sock == -1) {
                        perror("accept incoming serv");
                        exit(-2);
                    }
		    
                    set_sock_nonblock(param->mpi_cli_sock);

                    pjmedia_sxs_incoming_sock_connected(param->mport, param->mpi_cli_sock);
                    // this->in_read_start_time = time(NULL);
                    // this->new_incoming_connect_arrived();
                    // if (this->use_frame_buff == true) {
                    //     setev.data.fd = this->in_cli_fd;
                    //     setev.events = EPOLLIN;
                    //     if ((ret = epoll_ctl(this->eph, EPOLL_CTL_ADD, this->in_cli_fd, &setev)) != 0) {
                    //         perror("epoll add in cli read event");
                    //         exit(-3);
                    //     }
                    // }

                    // fport->base.put_frame = &file_put_frame;

                } else if (events[n].data.fd == param->mpi_cli_sock) {
                    pjmedia_sxs_incoming_sock_ready_read(param->mport, param->mpi_cli_sock);
                } else {
                    // do_use_fd(events[n].data.fd);
                }
            }
            if (events[n].events & EPOLLOUT) {
                if (events[n].data.fd == param->peer_fd) {
                    process_output_command(param->peer_fd);
                    fprintf(stderr, "processed out data command done\n");
                } else {
                    // do_use_fd(events[n].data.fd);
                }
            }
        }
    }

    for (;!quit_sip_proc;) {
        sleep(1);
        fprintf(stderr, "wake le..\n");
    }

    free(param);
    g_param = param = NULL;

    if (quit_sip_proc) {
        qDebug()<<"sip man proc exited.";
    }

    return 0;
}


int create_tcp_server()
{
    // socket, bind, listen, accept
    int ret = 0;
    int sock = 0;

    sock = ::socket(AF_INET, SOCK_STREAM, 0);

    ret = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int*)&ret, sizeof(ret));

    struct sockaddr_in serv_addr;
    // inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr.s_addr);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = 0;
    // serv_addr.sin_port = htons(12345);
    serv_addr.sin_family = AF_INET;
    ret = ::bind(sock, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr));
    ret = ::listen(sock, 10);

    // where to accept
    bool ok = !ret;
    socklen_t addr_size = sizeof(serv_addr);
    ::getsockname(sock, (struct sockaddr*)&serv_addr, &addr_size);
    PJ_LOG(4,(THIS_FILE, 
              "New outgoing server listened: serv port=%d, ok=%d, isListen=%d",
              ntohs(serv_addr.sin_port), ok, 1));

    set_sock_nonblock(sock);

    struct epoll_event ctl_evt;
    ctl_evt.events = EPOLLIN;
    ctl_evt.data.fd = sock;
    ret = epoll_ctl(g_param->epfd, EPOLL_CTL_ADD, sock, &ctl_evt);
    qDebug()<<"add swich fd to epoll:"<<g_param->epfd<<" fd:"<<sock<<" ret:"<<ret<<strerror(errno);

    return sock;
}

// new_rpc_command(123, "dscu", 1, "abcd", 'C', 65535);
char *new_rpc_command(int cmdno, int *cmdlen, const char *fmt, ...)
{
#define BUF_SIZE 200
    char cmdbuf[BUF_SIZE] = {0};
    char *wbuf = (char*)calloc(1, BUF_SIZE);

    va_list ap;
    int d;
    char c, *s;
    const char *ptr = fmt;

    va_start(ap, fmt);
    
    while (*ptr) {
        switch (*ptr++) {
        case 's':
            s = va_arg(ap, char *);
            strcat(cmdbuf, s);
            strcat(cmdbuf, ",");
            break;
        case 'd':
            d = va_arg(ap, int);
            memset(wbuf, 0, BUF_SIZE);
            sprintf(wbuf, "%d", d);
            strcat(cmdbuf, wbuf);
            strcat(cmdbuf, ",");
            break;
        case 'c':
            c = va_arg(ap, int);
            *cmdlen = strlen(cmdbuf);
            cmdbuf[*cmdlen] = c;
            cmdbuf[*cmdlen+1] = ',';
            cmdbuf[*cmdlen+2] = '\0';
            break;
        case 'u':
            d = va_arg(ap, int);
            memset(wbuf, 0, BUF_SIZE);
            sprintf(wbuf, "%u", d);
            strcat(cmdbuf, wbuf);
            strcat(cmdbuf, ",");
            break;
        default:
            d = va_arg(ap, int);
            break;
        }
    }

    va_end(ap);

    *cmdlen = strlen(cmdbuf);

    memset(wbuf, 0, BUF_SIZE);

    memcpy(wbuf, cmdlen, sizeof(int));
    memcpy(wbuf + sizeof(int), &cmdno, sizeof(int));
    memcpy(wbuf + 2*sizeof(int), cmdbuf, strlen(cmdbuf));

    return wbuf;
}
