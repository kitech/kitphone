// sipengine.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-03 17:19:37 +0000
// Version: $Id: sipengine.cpp 998 2011-09-17 11:03:58Z drswinghead $
// 

#include <QString>

#include <pjsip.h>
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include <pjsua.h>
#include <pjmedia/wave.h>
#include <pjmedia/wav_port.h>

#include "pjtypes.h"

#include "simplelog.h"
#include "intermessage.h"
#include "preferences.h"

#include "websocketserver3.h"
#include "sipengine.h"


//////////////
SipEngine *SipEngine::m_inst = NULL;
SipEngine *SipEngine::instance()
{
    if (SipEngine::m_inst == NULL) {
        SipEngine::m_inst = new SipEngine();
    }
    return SipEngine::m_inst;
}
SipEngine::SipEngine()
{
    this->mpool = NULL;
    this->mws = NULL;

    // this->create_memory_pool();
}

SipEngine::~SipEngine()
{
    // this->release_memory_pool();
}

bool SipEngine::start()
{
    bool bret = false;

    this->mws = WebSocketServer3::instance();
    this->mws->error.connect(boost::bind(&SipEngine::on_websocket_error, this, _1));
    this->mws->started.connect(boost::bind(&SipEngine::on_websocket_started, this));
    this->mws->new_connection.connect(boost::bind(&SipEngine::on_new_websocket_connection, this));
    this->mws->new_message.connect(boost::bind(&SipEngine::on_websocket_message, this, _1, _2));
    this->mws->connection_closed.connect(boost::bind(&SipEngine::on_websocket_connection_closed, this, _1));
    // TODO start here maybe has block 
    if (!this->mws->isRunning()) {
        bret = this->mws->start();
    }

    this->mset = boost::thread(boost::bind(&SipEngine::run, this));
    return true;
}

bool SipEngine::stop()
{
    return true;
}

bool SipEngine::restart()
{
    return true;
}

inline std::string threadid2str(boost::thread::id tid)
{
    std::stringstream ostr;
    ostr << tid;
    return ostr.str();
}

void SipEngine::run()
{
    this->mset_id = boost::this_thread::get_id();

    pj_thread_desc initdec;
    pj_thread_t* thread = 0;
    pj_status_t status;
    int evt_cnt = 0;

    status = pjsua_create();
    assert(status == PJ_SUCCESS);

    this->create_memory_pool();

    qLogx()<<""; // 这个调用都会出错？？？
    this->set_pre_custom_sip_config();
    qLogx()<<"";

    status = pjsua_init(&m_ua_cfg, &m_log_cfg, &m_media_cfg);
    assert(status == PJ_SUCCESS);

    pjsua_var.mconf_cfg.samples_per_frame = 8000; // pjsua_var from 

    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;
        pjsua_transport_config rtp_cfg;
        pjsua_transport_id udp_tp_id;
        pjsua_transport_id tcp_tp_id;
       
        // 创建指定端口的RTP/RTCP层media后端
        pjsua_transport_config_default(&rtp_cfg);
        rtp_cfg.port = 8050;
        status = pjsua_media_transports_create(&rtp_cfg);

        // SIP 层初始化，可指定端口
        pjsua_transport_config_default(&cfg);
        cfg.port = 35678; // if not set , use random big port 
        // cfg.public_addr = pj_str("123.1.2.3"); // 与上面的port一起可用于穿透，指定特定的公共端口!!!
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, &m_udp_tp_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error creating udp transport", status);
            // error_exit("Error creating transport", status);
        }
        status = pjsua_transport_set_enable(m_udp_tp_id, PJ_TRUE);

        // TCP transport
        pjsua_transport_config_default(&cfg);
        cfg.port = 45678;
        status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, &m_tcp_tp_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error creating tcp transport", status);
            // error_exit("Error creating transport", status);
        }
        status = pjsua_transport_set_enable(m_tcp_tp_id, PJ_TRUE);
        status = pjsua_transport_set_enable(m_udp_tp_id, PJ_TRUE);
    }
    
    status = pjsua_start();
    assert(status == PJ_SUCCESS);

    qLogx()<<"";
    
    this->sip_engine_started();

    this->set_post_custom_sip_config();
    status = pjsua_detect_nat_type();

    //////////
    boost::function<void()> func;
    while (1) {
        qLogx()<<""<<this;
        // sleep(2);
        boost::mutex::scoped_lock lock(this->mut_sfc);
        while( this->sync_funcs.empty() ) {// while - to guard agains spurious wakeups
            this->cond_sfc.wait( lock );
        }
        func = this->sync_funcs.front();            
        this->sync_funcs.erase(this->sync_funcs.begin());
        func();
    }
}

bool SipEngine::set_create_ws_server()
{
    return true;
}

bool SipEngine::set_register_thread()
{
    return true;
}

void SipEngine::set_pre_custom_sip_config()
{
    char tmp[80] = {0};
    pj_status_t status;

    // Initialize configs with default settings.
    pjsua_config_default(&m_ua_cfg);
    pjsua_logging_config_default(&m_log_cfg);
    pjsua_media_config_default(&m_media_cfg);

    snprintf(tmp, sizeof(tmp)-1, "KitPhone v%s; PJSUA v%s %s", KP_VERSION_STR,
             pj_get_version(), PJSTR2A(pj_get_sys_info()->info));
    m_ua_cfg.user_agent = pj_strdup3(this->mpool, tmp);
    m_ua_cfg.thread_cnt = 1;
    // At the very least, application would want to override
    // the call callbacks in pjsua_config:
    // ua_cfg.cb.on_incoming_call = ...
    // ua_cfg.cb.on_call_state = ..
    // ua_cfg.cb.on_incoming_call = &on_incoming_call;
    // ua_cfg.cb.on_call_state = &on_call_state;
    // ua_cfg.cb.on_call_media_state = &on_call_media_state;

    m_ua_cfg.cb.on_incoming_call = &SipEngine::on_incoming_call_wrapper;
    m_ua_cfg.cb.on_call_state = &SipEngine::on_call_state_wrapper;
    m_ua_cfg.cb.on_call_media_state = &SipEngine::on_call_media_state_wrapper;
    m_ua_cfg.cb.on_reg_state = &SipEngine::on_reg_state_wrapper;
    m_ua_cfg.cb.on_nat_detect = &SipEngine::on_nat_detect_wrapper;

    m_ua_cfg.nat_type_in_sdp = 1;

    /////// media config 
    m_media_cfg.snd_auto_close_time = 1;
    // Customize other settings (or initialize them from application specific
    // configuration file):
}

// 完全不起作用！！！
void SipEngine::set_post_custom_sip_config()
{
    // m_ua_cfg.cb.on_nat_detect = &SipEngine::on_nat_detect_wrapper;

    // m_ua_cfg.stun_srv_cnt = 0;
    // m_ua_cfg.stun_host = pj_str(TURN_SERVER ":" TURN_PORT); //dep
    m_ua_cfg.stun_srv[m_ua_cfg.stun_srv_cnt++] = pj_str(TURN_SERVER ":" TURN_PORT);

    /////// media config 
    // m_media_cfg.snd_auto_close_time = 1;
    // Customize other settings (or initialize them from application specific
    // configuration file):

    m_media_cfg.enable_turn = PJ_FALSE;
    m_media_cfg.turn_server = pj_str("turn.qtchina.net:34780");
    m_media_cfg.turn_conn_type = PJ_TURN_TP_UDP;

    m_media_cfg.turn_auth_cred.type = PJ_STUN_AUTH_CRED_STATIC;
    m_media_cfg.turn_auth_cred.data.static_cred.realm = pj_str("pjsip.org");
    m_media_cfg.turn_auth_cred.data.static_cred.username = pj_str("100");

    m_media_cfg.turn_auth_cred.data.static_cred.data_type = PJ_STUN_PASSWD_PLAIN;
    m_media_cfg.turn_auth_cred.data.static_cred.data = pj_str("100");

    m_media_cfg.enable_ice = PJ_FALSE;

    // why this detect no callback executed???
    if (0) {
        pj_ioqueue_t * sua_queue = pjsip_endpt_get_ioqueue(pjsua_var.endpt);
        pj_sockaddr_in stun_ain;
        pj_status_t status;
        pj_stun_config stun_cfg;

        pj_str_t stun_host = pj_str(TURN_SERVER);
        pj_uint16_t stun_port = atoi(TURN_PORT);
        status = pj_sockaddr_in_init(&stun_ain, &stun_host, stun_port);
        qLogx()<<"addrin:"<<status;

        pj_timer_heap_t *my_th = NULL;
        status = pj_timer_heap_create(this->mpool, 5, &my_th);
        qLogx()<<"timer heap:"<<status<<my_th;

        pj_stun_config_init(&stun_cfg, &this->mcp.factory, 0, sua_queue, my_th);
                        
        status = pj_stun_detect_nat_type(&stun_ain, &stun_cfg, 0, &SipEngine::on_raw_nat_detect_wrapper);
        qLogx()<<"nat ... "<<status;
    }
}


bool SipEngine::set_sip_start()
{
    return true;
}

bool SipEngine::set_create_transport()
{
    return true;
}

void SipEngine::on_websocket_error(int eno)
{
    qLogx()<<eno;
    this->sip_engine_error(eno);
}

void SipEngine::on_websocket_started()
{
    
}

// TODO support multi client connect, from desktop app or web app
void SipEngine::on_new_websocket_connection()
{
    qLogx()<<"";
    std::string mvstr;
    int cseq = this->mws->nextPendingConnection();
    assert(cseq > 0);

    if (this->su1.cseq != -1) {
        qLogx()<<"only allow 1 client:"<<this->su1.cseq<<", drop this."<<cseq;
    }


    mvstr = this->mws->conn_payload_path(cseq);
    this->wsconns.left.insert(boost::bimap<int,int>::left_value_type(cseq, cseq));

    // TODO check connect validation
    this->su1.cseq = cseq;

    ////////
    this->move_runin(boost::bind(&SipEngine::on_send_codec_list, this));
    // this->on_send_codec_list();
}

void SipEngine::on_websocket_message(const std::string &msg, int cseq)
{
    qLogx()<<""<<msg.c_str()<<cseq;
    char tbuf[512] = {0};
    snprintf(tbuf, sizeof(tbuf), "Returned by server via %d ,%s", cseq, msg.c_str());  
    std::string ret_msg = std::string(tbuf, strlen(tbuf));

    this->move_runin(boost::bind(&SipEngine::process_ctrl_message, this, msg, cseq));

    // for test only
    // this->mws->wssend(cseq, ret_msg);
}

void SipEngine::on_websocket_connection_closed(int cseq)
{
    assert(this->su1.cseq == cseq);
    this->su1.cseq = -1;
    qLogx()<<""<<cseq;
}

bool SipEngine::process_ctrl_message(const std::string &msg, int cseq)
{
    qLogx()<<"processing: "<<cseq<<msg.length()<<msg.c_str();

    bool bret = false;
    int cmd_id = PJSUA_INVALID_ID;
    int cmd_len;
    int call_id = PJSUA_INVALID_ID;

    CmdMakeCall mc;
    CmdRegister reger;

    cmd_id = InterMessage().jpack_cmdid(msg);

    switch (cmd_id) {
    case IPC_NO_MAKE_CALL:
        InterMessage().junpack_message(mc, msg);
        this->su1.mc_str = msg;
        this->su1.cmd_mc = mc;
        bret = this->make_call(mc.acc_id, mc.caller_name, mc.callee_phone, mc.sip_server,
                               call_id, mc);
        if (call_id != PJSUA_INVALID_ID) {
            this->su1.call_id = call_id;
        }
        break;
    case IPC_NO_REGISTER:
        InterMessage().junpack_message(reger, msg);
        this->su1.reg_str = msg;
        this->su1.cmd_reg = reger;
        bret = this->register_account(reger.display_name, reger.user_name, reger.sip_server,
                                      !reger.unregister, reger.password, msg);
        break;
    default:
        qLogx()<<"Unsupported cmd:"<<cmd_id;
        break;
    }

    return true;
}

//////////////
void SipEngine::on_nat_detect(const pj_stun_nat_detect_result *res)
{
    // this will run in thread which is created by pjsip
    qLogx()<< threadid2str(boost::this_thread::get_id()).c_str();
}
void SipEngine::on_raw_nat_detect(void *user_data, const pj_stun_nat_detect_result *res)
{
    // this will run in thread which is created by pjsip
    qLogx()<< threadid2str(boost::this_thread::get_id()).c_str();
    qLogx()<<user_data<<res;
}


void SipEngine::on_reg_state(pjsua_acc_id acc_id)
{
    // this will run in thread which is created by pjsip
    qLogx()<<acc_id;
    pj_status_t status;
    pjsua_acc_info ai;

    status = pjsua_acc_get_info(acc_id, &ai);
    if (status != PJ_SUCCESS) {
    }

    // std::string *edata = (std::string*)(pjsua_acc_get_user_data(acc_id));
    // assert(edata == NULL);

    this->su1.cmd_reg.acc_id = acc_id;
    CmdRegState rs;
    rs.cmd_seq = this->su1.cmd_reg.cmd_seq;
    rs.acc_id = acc_id;
    rs.status = ai.status;
    rs.status_text = PJSTR2S(ai.status_text);
    rs.acc_uri = PJSTR2S(ai.acc_uri);

    std::string cmdstr = InterMessage().jpack_message(rs);

    // send to which wsi???
    int iret = this->mws->wssend(this->su1.cseq, cmdstr);
}

static bool fill_call_info(pjsua_call_id call_id, CmdCallState &cmd_cs) 
{
    pjsua_call_info pci;
    pj_status_t status;

    status = pjsua_call_get_info(call_id, &pci);

    cmd_cs.cmd_seq = 0;
    cmd_cs.acc_id = pci.acc_id;
    cmd_cs.call_id = call_id;

    cmd_cs.local_info = PJSTR2S(pci.local_info);
    cmd_cs.local_contact = PJSTR2S(pci.local_contact);
    cmd_cs.remote_info = PJSTR2S(pci.remote_info);
    cmd_cs.remote_contact = PJSTR2S(pci.remote_contact);

    cmd_cs.state = pci.state;
    cmd_cs.state_text = PJSTR2S(pci.state_text);
    cmd_cs.last_status = pci.last_status;
    cmd_cs.last_status_text = PJSTR2S(pci.last_status_text);
    cmd_cs.media_status = pci.media_status;
    cmd_cs.media_dir = pci.media_dir;

    return true;
}

void SipEngine::on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    // this will run in thread which is created by pjsip
    //     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    PJ_UNUSED_ARG(e);
    pjsua_call_info pci;
    pj_status_t status;

    status = pjsua_call_get_info(call_id, &pci);
    PJ_LOG(3, (__FILE__, "Call %d state=%.*s", call_id,
               (int)pci.state_text.slen,
               PJSTR2A(pci.state_text)));

    if (pci.state == PJSIP_INV_STATE_CONFIRMED) {
        // log_output(LT_USER, QString("呼叫已经建立：%1").arg(this->m_curr_call_id));
    }

    if (pci.state == PJSIP_INV_STATE_DISCONNECTED) {
        // log_output(LT_USER, QString("呼叫结束：%1").arg(this->m_curr_call_id));
        // this->m_curr_call_id = PJSUA_INVALID_ID;

        // QPushButton *btn = this->uiw->pushButton_5;
        // btn->setText(tr("Call ..."));
        // btn->setEnabled(true);
    }

    ///////////////
    CmdCallState cmd_cs;
    bool bret = ::fill_call_info(call_id, cmd_cs);

    std::string cmdstr = InterMessage().jpack_message(cmd_cs);

    // send to which wsi???
    int iret = this->mws->wssend(this->su1.cseq, cmdstr);
}

void SipEngine::on_call_media_state(pjsua_call_id call_id)
{
    // this will run in thread which is created by pjsip
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pjsua_call_info pci;
    pj_status_t status;
    
    status = pjsua_call_get_info(call_id, &pci);
    qLogx()<<status<<pci.media_status<<pci.conf_slot;

    ///////////////
    CmdCallMediaState cmd_cs;
    bool bret = ::fill_call_info(call_id, cmd_cs);
    
    std::string cmdstr = InterMessage().jpack_message(cmd_cs);

    // send to which wsi???
    int iret = this->mws->wssend(this->su1.cseq, cmdstr);
 
    if (pci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        pjsua_conf_connect(pci.conf_slot, 0);
        pjsua_conf_connect(0, pci.conf_slot);

//         // test port_info
//         pjsua_conf_port_info cpi;
//         pjsua_conf_get_port_info(pci->conf_slot, &cpi);
//         qLogx()<<"conf port info: port number="<<cpi.slot_id<<",name='"<<PJSTR2Q(cpi.name)
//                 <<"', chan cnt="<<cpi.channel_count
//                 <<", clock rate="<<cpi.clock_rate;

//         // pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(g_rec_id));

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
//         qLogx()<<"got wav server port :"<<n_port;
//         // wc->setPort(n_port);
//         // wc->arun();
        
//         if (ms == NULL) {
//             qLogx()<<"pjmedia_session is null.\n";
//         } else {
//             status = pjmedia_session_enum_streams(ms, &stream_count, msi);
//             qLogx()<<"enum stream count:"<<stream_count;
            
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
    }

//     // pjsua_dump(PJ_TRUE);
//     // pjsua_call_dump(call_id, ...);

//     free(pci);
}

void SipEngine::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    // this will run in thread which is created by pjsip
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     pj_status_t status;
//     pjsua_call_info ci;
 
//     PJ_UNUSED_ARG(acc_id);
//     PJ_UNUSED_ARG(rdata);
 
//     pjsua_call_get_info(call_id, &ci);
 
//     PJ_LOG(3,(__FILE__, "Incoming call from %.*s!!",
//               (int)ci.remote_info.slen,
//               PJSTR2A(ci.remote_info)));
 
//     /* Automatically answer incoming calls with 200/OK */
//     status = pjsua_call_answer(call_id, 200, NULL, NULL);
}

void SipEngine::on_send_codec_list()
{
    if (this->su1.cseq == -1) {
        qLogx()<<"No ws client connected now.";
        return;
    }
    pj_status_t status;
    {
        CmdCodecList cmd_cl;
        
        // set codec
        pjsua_codec_info cids[100];
        unsigned int cid_cnt = 100;
        QString selected_codec;
        char tbuf[200];
        pjsua_transport_info tpi;

        status = pjsua_enum_codecs(cids, &cid_cnt);
        for (int i = 0; i < cid_cnt; i++) {
            cmd_cl.codecs.push_back(PJSTR2S(cids[i].codec_id));
        }

        std::string jstr = InterMessage().jpack_message(cmd_cl);
        int iret = this->mws->wssend(this->su1.cseq, jstr);
    }

    pjmedia_aud_dev_info auids[128];
    pjmedia_snd_dev_info sndids[128];
    unsigned int auid_count = 128;
    unsigned int sndid_count = 128;

    status = pjsua_enum_aud_devs(auids, &auid_count);
    qLogx()<<"found aud dev count:"<<auid_count;
    status = pjsua_enum_snd_devs(sndids, &sndid_count);
    qLogx()<<"found snd dev count:"<<sndid_count;

    for (int i = 0 ; i < sndid_count; i ++) {
        QString name;
        qLogx()<<"aud:"<<QString(auids[i].name)<<" snd:"<<QString(sndids[i].name);
    }
    
    int cap_dev = -1, pb_dev = -1;
    status = pjsua_get_snd_dev(&cap_dev, &pb_dev);
    qLogx()<<"curr snd dev:"<<"status="<<status<<" cap="<<cap_dev<<" pb="<<pb_dev;
    qLogx()<<"snd ok?"<<pjsua_snd_is_active();

    // status = pjsua_set_snd_dev(0, 0);
    qLogx()<<"snd ok?"<<pjsua_snd_is_active();

}

void SipEngine::move_runin(boost::function<void()> func)
{
    qLogx()<<"";

    {
        boost::mutex::scoped_lock lock(this->mut_sfc);
        this->sync_funcs.push_back(func);
        this->cond_sfc.notify_one();
    }
}

bool SipEngine::create_memory_pool()
{
    if (this->mpool != NULL) {
        qLogx()<<"Already created main app pool";
        assert(this->mpool == NULL);
    }

    pj_status_t status;
    status = pj_init();
    assert(status == PJ_SUCCESS);

    pj_caching_pool_init(&this->mcp, NULL, 512 * 1024);

    this->mpool = pj_pool_create(&this->mcp.factory, "main_sec_pool", 1000, 1000, NULL);
    if (this->mpool == NULL) {
        qLogx()<<"Create pool error.";
        assert(this->mpool != NULL);
    }

    // has a simple way, but use raw pjlib for test 
    // this->mpool = pjsua_pool_create("main_sec_pool", 1000, 1000);

    return true;
}

bool SipEngine::release_memory_pool()
{
    if (this->mpool == NULL) {
        qLogx()<<"Hav'nt created main app pool";
        assert(this->mpool != NULL);
    }

    pj_pool_release(this->mpool);
    pj_caching_pool_destroy(&this->mcp);

    return true;
}

bool SipEngine::register_account(const std::string &display_name, const std::string &user_name, 
                                 const std::string &serv_addr, bool reg, const std::string &passwd, 
                                 const std::string &cmdstr)
{
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    unsigned int acc_cnt;
    pjsua_acc_info acc_info;
    pj_status_t status;

    bool bret = false;
    /* Register to SIP server by creating SIP account. */
    pjsua_acc_config cfg;
    // SipAccount sip_acc;
    QString acc_uri;

    if (reg) {
        acc_id = this->_find_account_from_pjacc(user_name, serv_addr);
        // Q_ASSERT(sip_acc.userName.isEmpty() == false);

        pjsua_acc_config_default(&cfg);
        // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
        // cfg.id = pj_str(strdup(QString(SIP_USER " <sip:" SIP_USER "@"  "sips.qtchina.net:15678>").toAscii().data()));
        // cfg.id = pj_str(strdup(QString("%1 <sip:%1@sips.qtchina.net:15678>").arg(user_name).toAscii().data()));
        cfg.id = pj_str(strdup(QString("%1 <sip:%1@%2>").arg(user_name.c_str())
                               .arg(serv_addr.c_str()).toAscii().data()));
        // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
        // cfg.reg_uri = pj_str(QString("SIP:%1").arg(sip_acc.domain).toAscii().data());
        cfg.reg_uri = pj_strdup3(this->mpool, QString("SIP:%1@%2").arg(user_name.c_str())
                                 .arg(serv_addr.c_str()).toAscii().data());
        // cfg.reg_timeout = 800000000;
        // cfg.publish_enabled = PJ_FALSE;
        // cfg.auth_pref.initial_auth = 0; // no use
        // cfg.reg_retry_interval = 0;
        cfg.cred_count = 1;
        // cfg.cred_info[0].realm = pj_str(sip_acc.domain.toAscii().data());
        cfg.cred_info[0].realm = pj_str("*");
        cfg.cred_info[0].scheme = pj_str("digest");
        // cfg.cred_info[0].username = pj_str(sip_acc.userName.toAscii().data());
        cfg.cred_info[0].username = pj_strdup3(this->mpool, user_name.c_str());
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_strdup3(this->mpool, passwd.c_str());
        // cfg.cred_info[0].data = pj_str(strdup(recs.at(0).value("account_password").toString().toAscii().data()));

        if (acc_id == -1) {
            status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
            if (status != PJ_SUCCESS) {
                pjsua_perror(__FILE__, "Error adding account", status);
                //   error_exit("Error adding account", status);
            }
        } else {
            status = pjsua_acc_get_info(acc_id, &acc_info);
            if (acc_info.status == 0) {
                // already reged
                status = pjsua_acc_set_default(acc_id);
            } else {
                status = pjsua_acc_modify(acc_id, &cfg);
                status = pjsua_acc_set_registration(acc_id, 1);
            }
        }
    } else {
        acc_id = this->_find_account_from_pjacc(user_name, serv_addr);
        if (acc_id != -1) {
            status = pjsua_acc_set_registration(acc_id, 0); // unregister
        } else {
            qLogx()<<"User not exist, or not valid";
        }
    }

    if (acc_id != -1) {
    }

    // std::string *edata = new std::string(cmdstr);
    // status = pjsua_acc_set_user_data(acc_id, edata);
    // assert(status == PJ_SUCCESS);

    return true;
}

int SipEngine::_find_account_from_pjacc(const std::string &user_name, const std::string & serv_addr)
{
    unsigned int acc_cnt = 0;
    pjsua_acc_id acc_id;
    pjsua_acc_id acc_ids[100];
    pjsua_acc_info acc_info;
    pj_status_t status;
    QString acc_uri;
    QString sip_addr;

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
        acc_uri = PJSTR2Q(acc_info.acc_uri);
        qLogx()<<"acc_uri for "<<user_name.c_str()<<serv_addr.c_str()<<acc_id<<" " << acc_uri;
        // SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">
        sip_addr = acc_uri.mid(acc_uri.indexOf(":") + 1,
                               acc_uri.lastIndexOf(">") - acc_uri.indexOf(":")-1);
        qLogx()<<sip_addr;
        // if (acc_uri.split(" ").at(0) == acc_name) {
        if (sip_addr == QString("%1@%2").arg(user_name.c_str()).arg(serv_addr.c_str())) {
            if (!pjsua_acc_is_valid(acc_info.id)) {
                qLogx()<<"Warning: acc id is not valid:"<<acc_info.id<<" "<<acc_uri;
            }
            return acc_info.id;
        }
    }

    return PJSUA_INVALID_ID;
    return -1;
}

bool SipEngine::make_call(int acc_id, const std::string &caller_name, const std::string &callee_phone,
                          const std::string &sip_server, int &call_id, const CmdMakeCall &cmd)
{
    qLogx()<<acc_id<<caller_name.c_str()<<callee_phone.c_str()<<sip_server.c_str();

    // check call state
    // Q_ASSERT(this->m_curr_call_id == PJSUA_INVALID_ID);
    // if (this->m_curr_call_id != PJSUA_INVALID_ID) {
    //     this->log_output(LT_USER, QString(tr("正在呼叫中：%1，请不要重复呼叫。")).arg(this->m_curr_call_id));
    //     return;
    // }

    //////
    // set codec
    // pjsua_codec_info cids[100];
    // unsigned int cid_cnt = 100;
    // QString selected_codec;
    // char tbuf[200];
    pj_status_t status;
    pjsua_transport_info tpi;

    switch (cmd.tp_type) {
    case Preferences::SP_TCP:
        // TCP mode
        status = pjsua_transport_set_enable(this->m_tcp_tp_id, PJ_TRUE);
        status = pjsua_transport_set_enable(this->m_udp_tp_id, PJ_FALSE);
        status = pjsua_transport_get_info(this->m_tcp_tp_id, &tpi);
        qLogx()<<"Transport real host:port,"<<PJSTR2Q(tpi.local_name.host)<<tpi.local_name.port;
        break;
    default:
        // UDP mode
        status = pjsua_transport_set_enable(this->m_tcp_tp_id, PJ_FALSE);
        status = pjsua_transport_set_enable(this->m_udp_tp_id, PJ_TRUE);
        status = pjsua_transport_get_info(this->m_udp_tp_id, &tpi);
        qLogx()<<"Transport real host:port,"<<PJSTR2Q(tpi.local_name.host)<<tpi.local_name.port;
        break;
    }

    pjsua_call_id ncall_id;

    pjsua_codec_info cids[100];
    unsigned int cid_cnt = 100;
    QString selected_codec;
    char tbuf[200];
    // pjsua_transport_info tpi;
    bool codec_setted = false;

    status = pjsua_enum_codecs(cids, &cid_cnt);
    for (int i = 0; i < cid_cnt; i++) {
        status = pjsua_codec_set_priority(&cids[i].codec_id, 0);
        if (PJSTR2S(cids[i].codec_id) == cmd.use_codec) {
            status = pjsua_codec_set_priority(&cids[i].codec_id, 200);
            codec_setted = true;
        }
    }
    if (!codec_setted) {
        pj_str_t cs = pj_str("G729/8000");
        status = pjsua_codec_set_priority(&cs, 200);
    }

    // // char *sipu = "<SIP:99008668056013552776960@122.228.202.105:4060;transport=UDP>";
    char *sipu = strdup(QString("<SIP:%1@%2;transport=UDP>")
                        .arg(callee_phone.c_str()).arg(sip_server.c_str()) .toAscii().data());
    qLogx()<<"call peer: "<<sipu;
    // // char *sipu = "<SIP:99008668056013552776960@202.108.29.234:5060;transport=UDP>";
    pj_str_t uri = pj_str(sipu);

    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, &ncall_id);
    if (status != PJ_SUCCESS) {
        if (status == 469996) {
            // this->log_output(LT_USER, "无法打开声音设备。");
        } else {
            pjsua_perror(__FILE__, "Error making call", status);
            //error_exit("Error making call", status);
        }
    }

    free(sipu);

    qLogx()<<"oncall slot returned"<<call_id;
    return true;
}


void SipEngine::onon_test_q2b_rt()
{
    qLogx()<<""<<this;
    boost::thread th;
    int a = 5;
    // th.swap(this->mset);
    // th = boost::thread(boost::bind(&SipEngine::onon_test_q2b_rt2, this, a));
    // this->mset(boost::move(th));

    boost::function<void()> func = boost::bind(&SipEngine::onon_test_q2b_rt2, this, a);
    // func();
    this->move_runin(func);
}

void SipEngine::onon_test_q2b_rt2(int abcd)
{
    qLogx()<<""<<this<<abcd;
}


/////////////////////////////
/////////////////////////////

SipEngine::SessionUnit::SessionUnit()
{
    this->wsi = NULL;
    this->cseq = -1;
    this->acc_id = -1;
}

SipEngine::SessionUnit::~SessionUnit()
{

}

