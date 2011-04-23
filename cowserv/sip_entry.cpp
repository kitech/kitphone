// sip_entry.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-25 11:38:29 +0800
// Version: $Id$
// 

#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/epoll.h>

#include <QtCore>

#include "sxs_switcher.h"
#include "sip_entry.h"

#include "kitserv.h"

#define THIS_FILE __FILE__

// sip
static void on1_call_state(pjsua_call_id sip_call_id, pjsip_event *e)
{
    // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id<<pjsua_call_has_media(sip_call_id);
    pj_status_t status;
    pjsua_call_info ci;
    pjsua_acc_id acc_id;
    int skype_call_id;
    // sip_call_prop_t *scp = 0;
    ServContex *ctx = NULL;
 
    PJ_UNUSED_ARG(e);

    status = pjsua_call_get_info(sip_call_id, &ci);

    ctx = (ServContex*)(pjsua_call_get_user_data(sip_call_id));
    assert(ctx != NULL);
    ctx->m_sip_runtime_thread_id = syscall(__NR_gettid);
    ctx->on_sip_call_state_changed(sip_call_id);

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        // qDebug()<<__FILE__<<__LINE__<<"call disconnected";
        // assert(sip_calls.contains(sip_call_id));
        // scp = sip_calls.value(sip_call_id);
        // skype_call_id = scp->skype_call_id;
	// if (scp->hangup_point == INIT_HANGUP_FROM_SKYPE) {
    //     sip_hangup_call(skype_call_id, sip_call_id, 0);
	// } else {
        // sip_hangup_call(skype_call_id, sip_call_id, 1);
        // }
    } else {
        if (ci.state == PJSIP_INV_STATE_CALLING) {
        }

        if (ci.state == PJSIP_INV_STATE_EARLY) {
            // only can ring, can not connect tow conf peer
            // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"How deal early media?";
        }

        if (ci.state == PJSIP_INV_STATE_CONFIRMED) {

        }
    }

    ////////// send state command
    // if (sip_calls.contains(sip_call_id)) {
    //   assert(sip_calls.contains(sip_call_id));
    //   scp = sip_calls.value(sip_call_id);
    //   skype_call_id = scp->skype_call_id;

    //   int cmdno = 19;
    //   int cmdlen = 0;
    //   char cmdbuf[200] = {0};
    //   char *wbuf = (char*)calloc(1, 200);
        
    //   memset(wbuf, 0, 200);
    //   sprintf(cmdbuf, "%d,%d,%u", skype_call_id, sip_call_id, ci.state);
    //   cmdlen = strlen(cmdbuf);

    //   memcpy(wbuf, &cmdlen, sizeof(int));
    //   memcpy(wbuf + sizeof(int), &cmdno, sizeof(int));
    //   memcpy(wbuf + 2*sizeof(int), cmdbuf, strlen(cmdbuf));

    //   sock_out_data_queue.enqueue(wbuf);

    //   struct epoll_event ev;
    //   ev.events = EPOLLIN | EPOLLOUT;
    //   ev.data.fd = g_param->peer_fd;
    //   cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);
    // }
    
    //// loging
    // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id<<ci.state
    //         <<QString::fromAscii(ci.state_text.ptr, ci.state_text.slen)
	//     <<ci.last_status
	//     <<QString::fromAscii(ci.last_status_text.ptr, ci.last_status_text.slen)
	//     <<ci.media_status
    //         <<"has media:"<<pjsua_call_has_media(sip_call_id);
    PJ_LOG(3, (__FILE__, "Call %d state=%d %.*s, sipcode=%d,%.*s", sip_call_id,
               ci.state,
               (int)ci.state_text.slen,
               ci.state_text.ptr,
               ci.last_status,
               ci.last_status_text.slen,
               ci.last_status_text.ptr)
           );
}

static void on1_call_media_state(pjsua_call_id sip_call_id)
{
    // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id;
    pjsua_call_info ci;
    pj_status_t status;
    // sip_call_prop_t *sip_prop;
    ServContex *ctx = NULL;
    pjsua_recorder_id rec_id = -1;

    ctx = (ServContex*)(pjsua_call_get_user_data(sip_call_id));
    assert(ctx != NULL);
    ctx->m_sip_runtime_thread_id = syscall(__NR_gettid);
    ctx->on_sip_call_media_state_changed(sip_call_id);

    status = pjsua_call_get_info(sip_call_id, &ci);

    // return; // we need early media, early call this
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        // pjsua_conf_connect(ci.conf_slot, 0);
        // pjsua_conf_connect(0, ci.conf_slot);

        // test port_info
        pjsua_conf_port_info cpi;
        pjsua_conf_get_port_info(ci.conf_slot, &cpi);
        // qDebug()<<"conf port info: port number="<<cpi.slot_id<<",name='"<<cpi.name.ptr
        //         <<"', chan cnt="<<cpi.channel_count
        //         <<", clock rate="<<cpi.clock_rate;

        rec_id = ctx->m_rec_id;
        // put frame from pjmedia to standard tcp port: this->rec_id
        pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(rec_id));
        pjsua_conf_connect(pjsua_recorder_get_conf_port(rec_id), ci.conf_slot);

        pjmedia_transport *mtp = pjsua_call_get_media_transport(sip_call_id);
        pjmedia_port *mp = NULL;
        // unsigned int stream_count = 100;
        int i = 0;
        // unsigned short n_port = 0, incoming_n_port = 0;

        mp = (pjmedia_port*)ctx->m_port;
        pjmedia_sxs_incoming_media_connected(mp);
        pjmedia_sxs_outgoing_media_connected(mp);

        // pjsua_sxs_switcher_get_port(g_rec_id, &mp);
        // n_port = pjsua_switcher_get_outgoing_net_port(mp);
        // incoming_n_port = pjsua_switcher_get_incoming_net_port(mp);
        // n_port = pjmedia_sxs_port_outgoing_server_get_port(mp, 0);
        // incoming_n_port = pjmedia_sxs_port_incoming_server_get_port(mp, 0);
        // qDebug()<<__FILE__<<__LINE__<<"got wav server port :"<<n_port<<incoming_n_port;

        // sip_prop = sip_calls.value(sip_call_id);

        // int cmdno = 15;
        // int cmdlen = 0;
        // char cmdbuf[200] = {0};
        // char *wbuf = (char*)calloc(1, 200);
        
        // memset(wbuf, 0, 200);
        // sprintf(cmdbuf, "%d,%d,%u", sip_prop->skype_call_id, sip_call_id, incoming_n_port);
        // cmdlen = strlen(cmdbuf);

        // memcpy(wbuf, &cmdlen, sizeof(int));
        // memcpy(wbuf + sizeof(int), &cmdno, sizeof(int));
        // memcpy(wbuf + 2*sizeof(int), cmdbuf, strlen(cmdbuf));

        // sock_out_data_queue.enqueue(wbuf);

        // struct epoll_event ev;
        // ev.events = EPOLLIN | EPOLLOUT;
        // ev.data.fd = g_param->peer_fd;
        // cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);

        // //// 
        // cmdno = 17;
        // wbuf = (char*)calloc(1, 200);
        // memset(wbuf, 0, 200);
        // // cmdbuf = {0}; // >= gcc 4.5.0
        // memset(cmdbuf, 0, 200);
        // sprintf(cmdbuf, "%d,%d,%u", sip_prop->skype_call_id, sip_call_id, n_port);
        // cmdlen = strlen(cmdbuf);

        // memcpy(wbuf, &cmdlen, sizeof(int));
        // memcpy(wbuf + sizeof(int), &cmdno, sizeof(int));
        // memcpy(wbuf + 2*sizeof(int), cmdbuf, strlen(cmdbuf));

        // sock_out_data_queue.enqueue(wbuf);
        
        // cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);
    } else {
        pjmedia_port *mp = NULL;
        mp = (pjmedia_port*)ctx->m_port;
        pjmedia_sxs_incoming_media_disconnected(mp);
        pjmedia_sxs_outgoing_media_disconnected(mp);
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
    // qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<sip_call_id;
    pjsua_call_info ci;
    pj_status_t status;
    ServContex *ctx = NULL;
    pjsua_recorder_id rec_id = -1;

    ctx = (ServContex*)(pjsua_call_get_user_data(sip_call_id));
    assert(ctx != NULL);
    ctx->m_sip_runtime_thread_id = syscall(__NR_gettid);

    ctx->on_sip_call_dtmf_digit_arrived(sip_call_id, digit);
}

static int sr_find_account_from_pjacc(char * acc_name)
{
    int acc_count = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    // QString acc_uri;

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

        // acc_uri = QString::fromAscii(acc_info.acc_uri.ptr, acc_info.acc_uri.slen);
        // qDebug()<<"acc_uri for "<<acc_id<<" " << acc_uri;
        // if (acc_uri.split(" ").at(0) == acc_name) {
        //     if (!pjsua_acc_is_valid(acc_info.id)) {
        //         qDebug()<<"Warning: acc id is not valid:"<<acc_info.id<<" "<<acc_uri;
        //     }
        //     return acc_info.id;
        // }
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
    // QString acc_uri;
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

int init_sip_app_contex()
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
    ua_cfg.cb.on_dtmf_digit = &on1_dtmf_digit;
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

    media_cfg.clock_rate = 24000; // should or should not be equal pjsua_var.mconf_cfg.samples_per_frame????
    // Customize other settings (or initialize them from application specific
    // configuration file):

    // Initialize pjsua
    status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error initializing pjsua", status);
        return status;
    }

    // 必须放在pjsua_init调用后面才管用。
    // pjsua_var.mconf_cfg.samples_per_frame = 8000;
    // pjsua_var.mconf_cfg.samples_per_frame = 16000;
    pjsua_var.mconf_cfg.samples_per_frame = 24000;
    // pjsua_var.mconf_cfg.samples_per_frame = 32000;
    // pjsua_var.mconf_cfg.samples_per_frame = 48000;
    pjsua_var.media_cfg.clock_rate = 24000; // 也许这样行。果然可以。

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

    pjsua_dump(PJ_TRUE);
    // dump pjsua contex
    // max call count
    // 
    int max_call_count = pjsua_call_get_max_count();
    int samples_per_frame = pjsua_var.mconf_cfg.samples_per_frame;
    int channel_count = pjsua_var.mconf_cfg.channel_count;
    int bits_per_sample = pjsua_var.mconf_cfg.bits_per_sample;
    int bytes_per_frame = bits_per_sample * samples_per_frame * channel_count/8;
    pjsua_transport_id  tran_ids[100];
    unsigned int tran_count = 100;

    status = pjsua_enum_transports(tran_ids, &tran_count);

    fprintf(stdout, "INFO/SIP: max_call_count:%d, transport count: %d, samples_per_frame:%d, \n    channel count:%d, bits_per_sample:%d, bytes_per_frame:%d\n",
            max_call_count, tran_count, samples_per_frame, channel_count, 
            bits_per_sample, bytes_per_frame);

    
    fprintf(stdout, "PJSUA_MAX_CONF_PORTS: %d\n", PJSUA_MAX_CONF_PORTS);
    fprintf(stdout, "PJSUA_DEFAULT_CLOCK_RATE: %d\n", PJSUA_DEFAULT_CLOCK_RATE);
    fprintf(stdout, "PJSUA_DEFAULT_AUDIO_FRAME_PTIME: %d\n", PJSUA_DEFAULT_AUDIO_FRAME_PTIME);
    fprintf(stdout, "PJSUA_DEFAULT_CODEC_QUALITY: %d\n", PJSUA_DEFAULT_CODEC_QUALITY);
    fprintf(stdout, "PJSUA_DEFAULT_ILBC_MODE: %d\n", PJSUA_DEFAULT_ILBC_MODE);
    fprintf(stdout, "PJSUA_DEFAULT_EC_TAIL_LEN: %d\n", PJSUA_DEFAULT_EC_TAIL_LEN);
    fprintf(stdout, "PJSUA_MAX_PLAYERS: %d\n", PJSUA_MAX_PLAYERS);
    fprintf(stdout, "PJSUA_MAX_RECORDERS: %d\n", PJSUA_MAX_RECORDERS);

    char tmp_str[100];
    pjsua_codec_info codec_infos[50];
    pjsua_codec_info *codec_info;
    unsigned int codec_count = 50;
    status = pjsua_enum_codecs(codec_infos, &codec_count);
    fprintf(stdout, "total codec count: %d \n", codec_count);
    for (int i = 0; i < codec_count ; i ++) {
        codec_info = &codec_infos[i];
        memcpy(tmp_str, codec_info->codec_id.ptr, codec_info->codec_id.slen);
        tmp_str[codec_info->codec_id.slen] = '\0';
        fprintf(stdout, "    Audio codec # %d, pt: %u, name: %s\n",
                i, codec_info->priority, tmp_str);
    }
    // set codec priority if you want use special codec, big is first select, max 256


    pjmedia_aud_dev_info audio_dev_infos[256];
    pjmedia_aud_dev_info *audio_dev_info;
    unsigned audio_dev_count = 256;
    status = pjsua_enum_aud_devs(audio_dev_infos, &audio_dev_count);
    for (int i = 0 ; i < audio_dev_count; i++) {
        audio_dev_info = &audio_dev_infos[i];
        fprintf(stdout, "audio dev info # %d, ic: %d, oc: %d, name: [%s]\n",
                i, audio_dev_info->input_count, audio_dev_info->output_count,
                audio_dev_info->name);
    }

    pjmedia_snd_dev_info snd_dev_infos[256];
    pjmedia_snd_dev_info *snd_dev_info;
    unsigned snd_dev_count = 256;
    status = pjsua_enum_snd_devs(snd_dev_infos, &snd_dev_count);
    for (int i = 0 ; i < snd_dev_count; i++) {
        snd_dev_info = &snd_dev_infos[i];
        fprintf(stdout, "snd dev info # %d, ic: %d, oc: %d, name: [%s]\n",
                i, snd_dev_info->input_count, snd_dev_info->output_count,
                snd_dev_info->name);
    }

    return 0;
}

int set_sip_call_sample_rate(int rate)
{
    return pjsua_var.mconf_cfg.samples_per_frame;
}

// caller_name, phone_number, serv_addr, skype_call_id
// ctx sip call user data
int sip_call_phone(void *ctx, char *arg_str)
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

    static char *SIP_FROM_DOMAIN = "sips.qtchina.net:5060";

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
            snprintf(ubuf, sizeof(ubuf) - 1, "%s <sip:%s@%s>",
                     args.at(0).toAscii().data(), args.at(0).toAscii().data(), SIP_FROM_DOMAIN);
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
    status = pjsua_call_make_call(acc_id, &uri, 0, ctx, NULL, &call_id);
    if (status != PJ_SUCCESS) {
        pjsua_perror(__FILE__, "Error making call", status);
        qDebug()<<__FILE__<<__LINE__<<"Error making call"<<status;
        //error_exit("Error making call", status);
        return -1;
    }

    ((ServContex*)ctx)->m_sip_call_id = call_id;

    // int cmdno = 11;
    // int cmdlen = 0;
    // char cmdbuf[200] = {0};
    // char *wbuf = (char*)calloc(1, 200);

    // sprintf(cmdbuf, "%d,%d", args.at(3).toInt(), call_id);
    // cmdlen = strlen(cmdbuf);

    // memcpy(wbuf, &cmdlen, sizeof(int));
    // memcpy(wbuf + sizeof(int), &cmdno, sizeof(int));
    // memcpy(wbuf + 2*sizeof(int), cmdbuf, strlen(cmdbuf));

    // struct epoll_event ev;
    // ev.events = EPOLLIN | EPOLLOUT;
    // ev.data.fd = g_param->peer_fd;
    // cmdlen = epoll_ctl(g_param->epfd, EPOLL_CTL_MOD, g_param->peer_fd, &ev);

    // qDebug()<<"oncall slot returned"<<cmdlen<<strerror(errno);

    // sip_call_prop_t *scp = (sip_call_prop_t*)calloc(1, sizeof(sip_call_prop_t));
    // memset(scp, 0, sizeof(sip_call_prop_t));
    // scp->sip_call_id = call_id;
    // scp->skype_call_id = args.at(3).toInt();
    // scp->rec_id = g_rec_id;

    // sip_calls.insert((int)call_id, scp);

    status = PJ_SUCCESS;
    return status;     
}

int check_pjsip_config_validation()
{
    int invalid_count = 0;

    if (PJSUA_MAX_CALLS < 128 || PJSUA_MAX_CALLS > 256) {
        fprintf(stdout, "Warning: we want 128 <= PJSUA_MAX_CALLS <= 256, but now is: %d\n",
                PJSUA_MAX_CALLS);
        invalid_count ++;
    }

    if (PJSUA_MAX_CONF_PORTS < 1024 || PJSUA_MAX_CONF_PORTS > 2048) {
        fprintf(stdout, "Warning: we want 1023 <= PJSUA_MAX_CONF_PORTS <= 2048, but now is: %d\n",
                PJSUA_MAX_CONF_PORTS);
        invalid_count ++;
    }

    if (PJSUA_MAX_PLAYERS < 128 || PJSUA_MAX_PLAYERS > 256) {
        fprintf(stdout, "Warning: we want 128 <= PJSUA_MAX_PLAYERS <= 256, but now is: %d\n",
                PJSUA_MAX_RECORDERS);
        invalid_count ++;
    }


    if (PJSUA_MAX_RECORDERS < 128 || PJSUA_MAX_RECORDERS > 256) {
        fprintf(stdout, "Warning: we want 128 <= PJSUA_MAX_RECORDERS <= 256, but now is: %d\n",
                PJSUA_MAX_RECORDERS);
        invalid_count ++;
    }

    if (invalid_count > 0) {
        fprintf(stdout, "Warning: pjsip config invalid=%d, maybe you should recompile pjsip\n",
                invalid_count);
    }

    return 0;
}
