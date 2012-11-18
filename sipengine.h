// sipengine.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-03 17:19:08 +0000
// Version: $Id: sipengine.h 996 2011-09-16 15:04:24Z drswinghead $
// 

#ifndef _SIPENGINE_H_
#define _SIPENGINE_H_

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

//#include "private-libwebsockets.h"
//#include "libwebsockets.h"
#include "libwebsockets_extra.h"

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

#define VOIP_SERVER "202.108.12.212"
#define SIP_SERVER "202.108.29.234"
// #define TURN_SERVER "211.100.41.6"
#define TURN_SERVER "stun.pjsip.org"

//#define VOIP_SERVER "voip.qtchina.net"
//#define SIP_SERVER "sip.qtchina.net"
// #define TURN_SERVER "turn.qtchina.net"
//#define TURN_SERVER "turn.qtchina.net"
#define TURN_PORT "3478"
// #define TURN_PORT "10000"

// public turn server
// stun.pjsip.org:3478,240ms,test ok
// stunserver.org:3478,300ms,test ok
//  stun.sipgate.net:10000,375, test ok
// stun.ekiga.net:3478,450ms, test ok
// stun.iptel.org:3478,400ms, test ok
// stun.voipstunt.com:3478,380ms, test ok
// stun.voipcheap.com:3478,450ms, test ok
// stun.voipdiscount.com:3478,380ms, test ok

// stun.call4-free.com:3478, test faild
// relay.l.google.com:3478, test faild
// stun.l.google.com:19302, test faild

#ifdef _WIN32
// #define size_t unsigned int
#define ssize_t unsigned int
#endif

#include "intermessage.h"

// class PackageCombiner;
// class PackageSplitter;
class WebSocketServer3;

//////////////////
class SipEngine
{
private:
    explicit SipEngine();
    static SipEngine *m_inst;
public:
    static SipEngine *instance();
    virtual ~SipEngine();

    bool start();
    bool stop();
    bool restart();
    void run();

    boost::thread mset;
    boost::thread::id mset_id;
    bool quit_srv_loop;

    WebSocketServer3 *mws;

    void move_runin(boost::function<void()> func);

    // test
    void onon_test_q2b_rt();
    void onon_test_q2b_rt2(int abcd);

    // boost slot
    void on_websocket_error(int eno);
    void on_websocket_started();
    void on_new_websocket_connection();
    void on_websocket_message(const std::string &msg, int cseq);
    void on_websocket_connection_closed(int cseq);
    boost::signals2::signal<void(int)> sip_engine_error;
    boost::signals2::signal<void()> sip_engine_started;

    bool process_ctrl_message(const std::string &msg, int cseq);

    // sip callback;
	void on_nat_detect(const pj_stun_nat_detect_result *res);
	static void on_nat_detect_wrapper(const pj_stun_nat_detect_result *res)
    {SipEngine::instance()->on_nat_detect(res);}
    void on_raw_nat_detect(void *user_data, const pj_stun_nat_detect_result *res);
    static void on_raw_nat_detect_wrapper(void *user_data, const pj_stun_nat_detect_result *res)
    {SipEngine::instance()->on_raw_nat_detect(user_data, res);}

    void on_reg_state(pjsua_acc_id acc_id);
    static void on_reg_state_wrapper(pjsua_acc_id acc_id)
    {SipEngine::instance()->on_reg_state(acc_id);}

    void on_call_state(pjsua_call_id call_id, pjsip_event *e);
    static void on_call_state_wrapper(pjsua_call_id call_id, pjsip_event *e)
    {SipEngine::instance()->on_call_state(call_id, e);}

    void on_call_media_state(pjsua_call_id call_id);
    static void on_call_media_state_wrapper(pjsua_call_id call_id)
    {SipEngine::instance()->on_call_media_state(call_id);}

    void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
    static void on_incoming_call_wrapper(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
    {SipEngine::instance()->on_incoming_call(acc_id, call_id, rdata);}

    // void on_user_added(int payload, pjsua_call_id call_id, int tid);
    // static void on_user_added_wrapper(int payload, pjsua_call_id call_id,int tid)
    // {SipEngine::instance()->on_user_added(payload, call_id, tid);}

    // void on_exceed_max_call_count(int payload);
    // static void on_exceed_max_call_count_wrapper(int payload)
    // {SipEngine::instance()->on_exceed_max_call_count(payload);}

    void on_send_codec_list();

private:
    bool set_create_ws_server();
    void set_pre_custom_sip_config();
    void set_post_custom_sip_config();
    bool set_register_thread();
    bool set_sip_start();
    bool set_create_transport();
    bool create_memory_pool(); // 
    bool release_memory_pool(); // 

    bool register_account(const std::string &display_name, const std::string &user_name, 
                          const std::string &serv_addr, bool reg, const std::string &passwd,
                          const std::string &cmdstr);
    int _find_account_from_pjacc(const std::string &user_name, const std::string & serv_addr);

    bool make_call(int acc_id, const std::string &caller_name, const std::string &callee_phone,
                   const std::string &sip_server, int &call_id, const CmdMakeCall &cmd);
private:
    // sip
    pjsua_config m_ua_cfg;
    pjsua_logging_config m_log_cfg;
    pjsua_media_config m_media_cfg;
    pjsua_transport_config m_tcp_tp_cfg;
    pjsua_transport_config m_udp_tp_cfg;
    pjsua_transport_config m_ipv6_tp_cfg;
    pjsua_transport_config m_tls_tp_cfg;
    pjsua_transport_config m_tcp6_tp_cfg;
    pjsua_transport_config m_udp6_tp_cfg;

    pjsua_transport_id m_udp_tp_id;
    pjsua_transport_id m_tcp_tp_id;
    pjsua_transport_id tls_tpid;
    pjsua_transport_id ipv6_tpid;
    pjsua_transport_id udp6_tpid;
    pjsua_transport_id tcp6_tpid;
    pjsua_transport_id user_tpid;
    pjsua_transport_id sctp_tpid;

    pj_caching_pool mcp;
    pj_pool_t *mpool;

    /////ws
    boost::bimap<int,int> wsconns;
    std::vector<boost::function<void()> > sync_funcs;
    
    boost::condition_variable cond_sfc;
    boost::mutex mut_sfc;

    class SessionUnit {
    public:
        SessionUnit();
        ~SessionUnit();

        libwebsocket *wsi;
        int cseq;

        int acc_id;
        int call_id;
        
        std::string mc_str;
        CmdMakeCall cmd_mc;

        std::string reg_str;
        CmdRegister cmd_reg;
    };

    // 只接受一个ws客户端。
    SessionUnit su1;
    
    std::vector<SessionUnit*> scs;
};



#endif /* _SIPENGINE_H_ */
