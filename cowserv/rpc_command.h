// rpc_command.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-08 15:04:47 +0800
// Version: $Id$
// 

#ifndef _RPC_COMMAND_H_
#define _RPC_COMMAND_H_

/*
  socket msg cmd: binary
  10(caller_name,callee_phone,sip_serv,skype_call_id) call sip phone sk -> sip
  11(skype_call_id,sip_call_id)  sip call created   sip -> sk
  12(skype_call_id,sip_call_id)  skype user init hangup  sk -> sip
  13(skype_call_id,sip_call_id)  sip user init hangup    sip -> sk
  15(skype_call_id,sip_call_id,port)  sip switcher input media port ready       sip -> sk
  16(skype_call_id,sip_call_id,dtmf)  send dtmf                                 sk -> sip
  17(skype_call_id,sip_call_id,port)  sip switcher output media port ready      sip -> sk
  18(skype_call_id,sip_call_id,hold=1/unhold=0)   hold/unhold sip call          sk -> sip
  19(skype_call_id,sip_call_id,int-call_state)    sip call state changed        sip -> sk
  21(skype_call_id,sip_call_id, int-media_state)  sip call media state changed  sip -> sk

  22(skype_call_id,sip_call_id)      sip switcher input media none
  23(skype_call_id,sip_call_id)      sip switcher output media none

  websocket msg cmd: C,R,F       ascii-UTF-8
  100(caller_name,forward_gateway,port) return_forward_gateway_ws_port R -> C
  101(caller_name,phone) notepair_name_phone_ajax_replacement C -> R
  102(caller_name,response) return_notepair_name_phone_ajax_replacement C -> R
  103(caller_name,forward_gateway) get_foward_gateway_ws_port C -> R
  104(caller_name,router,skype_call_id,msg)  notice_call_msg, line busy  R -> C
  105(caller_name,phone,dtmf_string)    send_dtmf           C -> F
  106(caller_name,forward,skype_call_id,msg)  notice_call_msg  F -> C
  108(caller_name,forward,skype_call_id,hangupcause,desc) hangup note    F -> C
  110(caller_name,forward,skype_call_id,desc)   hold/unhold note       F -> C
  112(caller_name,forward,skype_call_id,msg)    pstn endpoint answered  F -> C

  skype 
  
  200        reconnect skype
  201       relogin skype

  202        web skype cmd
  
*/

class RPCMD 
{
public:
    enum {
        CMDMIN=0,
        CALL_SIP_PHONE = 10,
        SIP_CALL_CREATED = 11,
        SKYPE_USER_INIT_HANGUP = 12,
        SIP_USER_INIT_HANGUP = 13,
        SIP_MEDIA_INPUT_PORT_READY = 15,
        SKYPE_SEND_DTMF = 16,
        SIP_MEDIA_OUTPUT_PORT_READY = 17,
        SKYPE_HOLD_UNHOLD_CALL = 18,
        SIP_CALL_STATE_CHANGED = 19,
        SIP_CALL_MEDIA_STATE_CHANGED = 20,
        SIP_CALL_DTMF_DIGIT = 21,
        SIP_MEDIA_INTPUT_PORT_DONE = 22,
        SIP_MEDIA_OUTPUT_PORT_DONE = 23,
        //// send to ws msg
        SEND_WS_NOTE_MSG = 24,
        //// pcm if sample rate change
        PCMIF_SAMPLE_RATE_CHANGED = 25,

        RESP_FORWARD_GATEWAY_WS_PORT = 100,
        NOTEPAIR_NAME_PHONE = 101,
        RESP_NOTPAIR_NAME_PHONE = 102,
        GET_FORWARD_GATEWAY_WS_PORT = 103,
        NOTICE_MSG_LINE_BUSY = 104,
        WSC_DTMF_REQUEST = 105,
        NOTICE_CALL_MSG = 106,
        NOTICE_CALL_HANGUP = 108,
        WSC_HOLD_UNHOLD_REQUEST = 110,
        NOTICE_SIP_ANSWERED = 112,
        // websocket point send hangup request, not from skype, sip 
        WSC_HANGUP_REQUEST = 113,

        SKYPE_NEED_RECONNECT = 200,
        SKYPE_NEED_RELOGIN = 201,
        WEB_SKYPE_REQUEST = 202,
        CMDMAX,
    };
};

#endif /* _RPC_COMMAND_H_ */
