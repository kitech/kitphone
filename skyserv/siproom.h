// siproom.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-07 11:06:55 +0800
// Version: $Id: siproom.h 908 2011-06-02 09:37:45Z drswinghead $
// 


#ifndef _SIPROOM_H_
#define _SIPROOM_H_

#include <stdio.h>
#include <stdlib.h>

extern "C" {
	#include <pjlib.h>
	#include <pjlib-util.h>
	#include <pjmedia.h>
	#include <pjmedia-codec.h>
	#include <pjsip.h>
	#include <pjsip_simple.h>
	#include <pjsip_ua.h>
	#include <pjsua-lib/pjsua.h>
    #include <pjsua-lib/pjsua_internal.h>
}

/*
  socket msg cmd: binary
  10(caller_name,callee_phone,sip_serv,skype_call_id,user_ipaddr) call sip phone sk -> sip
  11(skype_call_id,sip_call_id)  sip call created   sip -> sk
  12(skype_call_id,sip_call_id)  skype user init hangup  sk -> sip
  13(skype_call_id,sip_call_id,last_sip_code)  sip user init hangup    sip -> sk
  15(skype_call_id,sip_call_id,port)  sip switcher input media port ready       sip -> sk
  16(skype_call_id,sip_call_id,dtmf)  send dtmf                                 sk -> sip
  17(skype_call_id,sip_call_id,port)  sip switcher output media port ready      sip -> sk
  18(skype_call_id,sip_call_id,hold=1/unhold=0)   hold/unhold sip call          sk -> sip
  19(skype_call_id,sip_call_id,int-call_state)    sip call state changed        sip -> sk
  21(skype_call_id,sip_call_id,int-media_state)  sip call media state changed  sip -> sk

  websocket msg cmd: C,R,F       ascii-UTF-8
  100(caller_name,forward_gateway,port) return_forward_gateway_ws_port R -> C
  101(caller_name,phone) notepair_name_phone_ajax_replacement C -> R
  102(caller_name,response) return_notepair_name_phone_ajax_replacement C -> R
  103(caller_name,forward_gateway) get_foward_gateway_ws_port C -> R
  104(caller_name,router,skype_call_id,msg)  notice_call_msg, line busy  R -> C
  105(caller_name,forward,dtmf_string)    send_dtmf           C -> F
  106(caller_name,forward,skype_call_id,msg)  notice_call_msg  F -> C
  107(caller_name,forward,skype_call_id,msg)   hangup request   C -> F
  108(caller_name,forward,skype_call_id,hangupcause,desc) hangup note    F -> C
  110(caller_name,forward,skype_call_id,desc)   hold/unhold note       F -> C
  112(caller_name,forward,skype_call_id,msg)    pstn endpoint answered  F -> C
  114(caller_name,forward,skype_call_id,msg)    media active, 3-5 seconds mute, wait ring sound  F -> C
  115(caller_name,forward,skype_call_id,msg)    check user info,before forworder getcallpair    F -> C
  116(caller_name,forward,skype_call_id,msg)    分配线路    F -> C
  117(caller_name,forward,skype_call_id,msg)    连接对方话机    R -> C
  118(caller_name,forward,skype_call_id,msg)    对方挂机+原因    R -> C
  119(caller_name,forward,skype_call_id,real_caller_name)  用户名输入错误     R -> C
 */

#define SKXSIP_CMD_BUFFER_LEN  512

#define INIT_HANGUP_FROM_UNKNOWN 0
#define INIT_HANGUP_FROM_SKYPE 1
#define INIT_HANGUP_FROM_SIP 2

typedef struct sip_call_prop
{
    int sip_call_id;
    int skype_call_id;
    int rec_id;
    int hangup_point; // skype 1  or sip point 2, unknown 0
    int skxsip_sock;
    pjmedia_port *mport;
} sip_call_prop_t;


typedef struct sip_proc_param {
    int peer_fd;     // communicate endpoint
    unsigned short peer_port;
    char sock_path[256];
    int epfd;
    int mpo_serv_sock;
    int mpi_serv_sock;
    int mpo_cli_sock;
    int mpi_cli_sock;
    pjmedia_port *mport;
    pj_pool_t *pool;
} sip_proc_param_t;

void sip_hangup_call(int skype_call_id, int sip_call_id, int hangup_skype, int last_status);

/// 
int create_tcp_server();

#endif /* _SIPROOM_H_ */
