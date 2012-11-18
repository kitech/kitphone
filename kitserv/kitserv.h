// kitserv.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-25 08:32:53 +0800
// Version: $Id$
// 

#ifndef _KITSERV_H_
#define _KITSERV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <QQueue>
#include <QStringList>

#include "../libng/qbihash.h"

#include "skype-object.h"

struct pj_pool_t;
struct pjmedia_port;

class Database;
// class ThreadPassbyCommand;

class CSkype;
class SkypePCMInterface;

/*
  TODO 命令行参数处理。
  -c file config file
  -w n   way count
 */

int serv_main(int argc, char **argv);
int init_sip_app_contex();

void *pcm_host_main_proc(void *args);

//////////////////
////
//////////////////
class ServContex;
class ThreadPassbyCommand
{
public:
    ThreadPassbyCommand() {
        this->cmdno = 0;
        this->ctx = NULL;

        this->delay_time = 0;
        this->entry_time = 0;
        this->entry_tv.tv_sec = 0;
        this->entry_tv.tv_usec = 0;
    }

    ~ThreadPassbyCommand() {}

    int cmdno;
    ServContex *ctx;
    QStringList args;

    int delay_time;   // 延迟时间。秒, 暂不支持秒以前的延迟
    time_t entry_time; // 开始计时时间。
    struct timeval entry_tv; // try支持tv_usec
};


/////////////////////////
//////
////////////////////////
class ServContex {
public:
    ServContex(int ctx_id, char *storage_dir, char *skype_name);
    ~ServContex();

    int connect_skype_runtime(char *app_token, unsigned short app_port, QString skype_name, QString password);
    int reconnect_skype_runtime();
    int relogin_skype_runtime();
    int init_sxs_switcher();
    int init_websocket_thread();
    int init_pcmhost_thread();

    void onNewForwardCallArrived(SEReference &conversation, QString callerName, QString calleeName, int skypeCallID);
    void onSkypeForwardCallAnswered(SEReference &conversation, int skypeCallID, QString callerName, QString calleeName);
    int hangup_skype_call(SEReference &conversation);

    int on_sip_call_state_changed(int sip_call_id);
    int on_sip_call_media_state_changed(int sip_call_id);
    int on_sip_call_dtmf_digit_arrived(int sip_call_id, int dtmf);

    int post_schedule_cmd(int cmdno, ServContex *ctx, QStringList args);
    int post_timeout_cmd(int cmdno, ServContex *ctx, QStringList args, int delay);

    bool init_ws_serv(QString handle_name);

    bool send_ws_command_100(QString caller_name, QString gateway, QString host, unsigned short port);
    bool send_ws_command_102(QString caller_name, QString data);
    bool send_ws_command_104(QString caller_name, QString gateway, int skype_call_id, QString msg);
    bool send_ws_command_106(QString caller_name, QString gateway, int skype_call_id, QString msg);
    bool send_ws_command_108(QString caller_name, QString gateway, int skype_call_id, int hangupcause, QString reason);
    bool send_ws_command_110(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_112(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_114(QString caller_name, QString gateway, int skype_call_id, QString reason);
    bool send_ws_command_common(QString caller_name, QString cmdline);

    // utils function
    QString serverIpAddr(int type);

    const int m_ctx_id;

    Database *m_db;

    pthread_t m_hub_thread;
    int m_hub_thread_quit_loop;
    pthread_mutex_t m_cmder_mutex;
    pthread_cond_t m_cmder_cond;
    QQueue<ThreadPassbyCommand*> m_cmder_queue;
    QList<ThreadPassbyCommand*> m_cmder_timer_queue;

    pthread_t m_websock_thread;
    int m_websocket_thread_quit_loop;
    int m_websocket_epfd;
    int m_websocket_serv_fd;
    KBiHash<int, QString> m_websocket_client_fds;
    KBiHash<int, void *> m_websocket_client_ws_ctxes;

    CSkype *m_skype;
    char *m_app_token;
    unsigned short m_app_port;
    char m_localname[256];  // local connection name for sk instance
    char *m_account_name;
    char *m_account_password;
    char m_storage_dir[256];
    char m_log_file[256];

    int m_rec_id; // really a pjsua_record_id
    // void *m_port; // pjsip pjmedia port*,
    // void *m_pool;   // pj_pool_t *
    pjmedia_port *m_port;
    pj_pool_t *m_pool;
    int m_sip_call_id;  // pjsua_call_id
    /// 哪方发起的挂断
    int m_hangup_init_point; // INIT_HANGUP_FROM_NONE, INIT_HANGUP_FROM_SKYPE, INIT_HANGUP_FROM_SIP, how decision
    enum {INIT_HANGUP_FROM_NONE = 0, INIT_HANGUP_FROM_SKYPE, INIT_HANGUP_FROM_SIP};
    SEReference m_skype_call_conv;

    void *pcmif_server; // really SkypePCMInterfaceServer*
    void *pcmif_cb_client; //really SkypePCMCallbackInterfaceClient*
    // void *pcmif; // really SkypePCMInterface*
    SkypePCMInterface *pcmif;

    char from_skypekit_key[256];
    char to_skypekit_key[256];

    /// for pcmif and sip media port transport voice
    int m_sock_pair_sv[2];

    pthread_t pcmhost_main_thread;
    pthread_t skypekit_main_thread;
    pid_t pcmhost_main_thread_pid;
    pid_t skypekit_main_thread_pid;

    pid_t m_hub_thread_id;
    pid_t m_main_init_thread_id;
    pid_t m_websocket_thread_id;
    pid_t m_skype_client_thread_id;
    pid_t m_skype_audio_host_cmd_thread_id;
    pid_t m_skype_audio_host_media_thread_id;
    pid_t m_sip_runtime_thread_id;
    pid_t m_sip_media_thread_id; // put frame, get frame
};

#endif /* _KITSERV_H_ */
