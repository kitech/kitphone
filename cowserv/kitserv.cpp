// kitserv.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-25 08:32:48 +0800
// Version: $Id$
// 

#include <syscall.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef HAVA_CONFIG_H
#include "config.h"
#endif

#include <QHostAddress>
#include <QNetworkInterface>

#include "sidg_pcmif_server.hpp"
#include "SidPCMInterface.hpp"
#include "SidProtocolEnums.hpp"
#include "sidg_pcmif_cb_client.hpp"
#include "SidDebugging.hpp"

#include "PCMSxsHost.h"

#include "cskype.h"

#include "../skyserv/configs.h"
#include "../skyserv/database.h"

#include "websocket_proc.h"

#include "rpc_command.h"
#include "sip_entry.h"
#include "sxs_switcher.h"
#include "main.h"
#include "kitserv.h"


/*
  该版本的kitserv只使用Qt的工具类，像QString, QHash等
  该版本的kitserv绝不使用Qt的QApplication事件循环和Qt的signal/slot机制。
 */

/*
  由于所有的实例管理都在一进程中，所以出问题的时候程序应该有非常强的自恢复功能。
  这里的线程比较复杂，有以下线程，各有用处，各有限制
  * 主线程(MIT main init thread), 初始化各种全局环境，包括，skype runtime, skype media host, pjsua 的初始化。
  * contex 事件循环 hub 线程(CHT context hub thread)，每个contex都有自己的命令处理事件循环。
  * skype 客户端线程(SCT skype client thread)
  * skype host 端线程(SHT skype host processer thread)。
  * skype host 媒体流线程(SHT skype host media thread)。
  * pjsua 执行线程(PJRT pjsip runtime thread)，现在与主线程是重合的，只能在该线程（主线程）执行pjsua/pjsip函数调用。
  * websocket 服务器线程
  * 所有，如果在skype客户端线程，或者skype host 端线程中有需要执行pjsua/pjsip函数的需要，
  则需要把它们传递到pjsua/pjsip执行线程（主线程）来执行。
 */

/////////////////////
////////////////////
/////////////////////
static int contex_hub_thread_process_passby_command(ThreadPassbyCommand *cmd)
{
    int cmdno, iv;
    char *ptr;
    char cmd_buf[2000];
    QString str;
    QStringList args;
    pj_status_t status;
    pj_str_t pjstr;
    ServContex *ctx;
    bool bv;

    cmdno = cmd->cmdno;
    ctx = cmd->ctx;

    switch (cmdno) {
    case 10:
        str = cmd->args.join(",");
        memset(cmd_buf, 0, sizeof(cmd_buf));
        strncpy(cmd_buf, str.toAscii().data(), sizeof(cmd_buf) - 1);
        iv = sip_call_phone(cmd->ctx, cmd_buf);
        // sip_call_phone(ctx, arg_str); // Assertion `!"Calling pjlib from unknown/external thread.
        break;
    case 12:
        // 
        if (ctx->m_sip_call_id < 0) {
            fprintf(stdout, "Are you want to hangup a invalid sip call:%d?\n", ctx->m_sip_call_id);
        } else {
            status = pjsua_call_hangup(ctx->m_sip_call_id, 0, NULL, NULL);
            assert(status == PJ_SUCCESS);
        }
        break;
    case 13:
        if (!ctx->m_skype_call_conv) {
            fprintf(stdout, "Are you want to hangup a invalid skype call:?\n");
        } else {
            CConversation::Ref conv = (static_cast<CConversation*>
                                       (ctx->m_skype_call_conv.operator->()))->ref();
            // conv->LeaveLiveSession(true);
            iv = ctx->hangup_skype_call(ctx->m_skype_call_conv);
        }
        break;
    case 15:
    case 17:
        // media ready
        bv = ((PCMSxsHost*)(cmd->ctx->pcmif))->switch_media_mode_sxs();
        break;
    case RPCMD::SIP_CALL_DTMF_DIGIT:
        if (!ctx->m_skype_call_conv) {
            fprintf(stdout, "Are you want to hangup a invalid skype call:?\n");
        } else {
            CConversation::Ref conv = (static_cast<CConversation*>
                                       (ctx->m_skype_call_conv.operator->()))->ref();
            // conv->LeaveLiveSession(true);
            // ctx->hangup_skype_call(ctx->m_skype_call_conv);
            str = args.at(1);
            Participant::DTMF dtmf = (Participant::DTMF)str.toInt();
            bv = conv->SendDTMF(dtmf, 200);
        }
        break;
    case 22:
    case 23:
        // media none
        bv = ((PCMSxsHost*)(cmd->ctx->pcmif))->switch_media_mode_player();
        break;

    case RPCMD::SEND_WS_NOTE_MSG:
        iv = cmd->args.at(1).toInt();
        str = cmd->args.at(0);
        iv = websocket_send_data(iv, str.toAscii().data(), str.length());
        break;
    case RPCMD::SKYPE_SEND_DTMF:
        // dtmf from skype
        if (ctx->m_sip_call_id < 0) {
            fprintf(stdout, "Are you want to hangup a invalid sip call:%d?\n", ctx->m_sip_call_id);
        } else {
            str = cmd->args.at(0);
            pjstr = pj_str(str.toAscii().data());
            status = pjsua_call_dial_dtmf(ctx->m_sip_call_id, &pjstr);
            assert(status == PJ_SUCCESS);
        }
        break;
    case RPCMD::PCMIF_SAMPLE_RATE_CHANGED:
        fprintf(stdout, "hub Info: sample rate changed, %d, %u\n",
                cmd->args.at(0).toInt(), cmd->args.at(1).toUInt());
        pjsua_var.mconf_cfg.samples_per_frame = cmd->args.at(1).toUInt();
        pjsua_var.media_cfg.clock_rate = pjsua_var.mconf_cfg.samples_per_frame;
        break;

    case RPCMD::SKYPE_NEED_RECONNECT:
        iv = cmd->ctx->reconnect_skype_runtime();
        break;
    case RPCMD::SKYPE_NEED_RELOGIN:
        iv = cmd->ctx->relogin_skype_runtime();
        break;
    default:
        fprintf(stdout, "Unsupported cmd: %d, %s \n", cmdno,
                cmd->args.join(",").toAscii().data());
        break;
    };

    return 0;
}

// replace now serv_main's cmd queue process, 
// but this is context level,
void *server_contex_hub_proc(void *args)
{
    time_t curr_time = 0;
    ServContex *ctx = (ServContex*)args;
    ctx->m_hub_thread_id = syscall(__NR_gettid);

    fprintf(stdout, "Entry hub thread loop, ctx id: %d, thread id: %d\n",
            ctx->m_ctx_id, ctx->m_hub_thread_id);

    // 注册成pjsip处理线程
    pj_status_t status;
    pj_thread_desc initdec;
    pj_thread_t *pjsip_reged_thread = 0;
    char pjsip_thread_name[100] = {0};

    snprintf(pjsip_thread_name, sizeof(pjsip_thread_name) - 1, 
             "ctx_reg_pjsip_thread_%d", ctx->m_ctx_id);
    if (!pj_thread_is_registered()) {
        // status = pj_thread_register("simpleaddpjthread", initdec, &thread);
        status = pj_thread_register(pjsip_thread_name, initdec, &pjsip_reged_thread);
        if (status != PJ_SUCCESS) {
            // qDebug()<<"register ctx pjsip thread error:"<<status;
            fprintf(stdout, "Register ctx(%d) pjsip thread(%d) error:\n", 
                    ctx->m_ctx_id, ctx->m_hub_thread_id);
            assert(status == PJ_SUCCESS);
            return 0;
        }
    }
    PJ_CHECK_STACK();

    QList<ThreadPassbyCommand*> cmder_swap_queue;
    ThreadPassbyCommand *curr_cmd = NULL;
    struct timespec to_time;
    int wait_rc = 0;

    for (;ctx->m_hub_thread_quit_loop == 0;) {
        pthread_mutex_lock(&ctx->m_cmder_mutex);
        clock_gettime(CLOCK_REALTIME, &to_time);
        // to_time.tv_nsec += 50 * 1000; // 1000000=1s, 50 * 1000 = 50ms
        to_time.tv_sec += 2;
 
        wait_rc = pthread_cond_timedwait(&ctx->m_cmder_cond, &ctx->m_cmder_mutex, &to_time);
        // wait_rc = pthread_cond_wait(&ctx->m_cmder_cond, &ctx->m_cmder_mutex);
        if (wait_rc == 0) {
            fprintf(stdout, "someone wake up me, has something to do now.\n");
            while (ctx->m_hub_thread_quit_loop == 0 
                   && !ctx->m_cmder_queue.isEmpty()) {
                curr_cmd = ctx->m_cmder_queue.dequeue();
                if (curr_cmd != NULL) {
                    wait_rc = contex_hub_thread_process_passby_command(curr_cmd);
                    delete curr_cmd;
                    curr_cmd = NULL;
                }
            }
        } else if (wait_rc == ETIMEDOUT) {
            // fprintf(stdout, "Timeout wake up(%d), take a look only. %ld\n", 
            //         ctx->m_ctx_id,  to_time.tv_nsec);
            // // dump thread list
            // fprintf(stdout, "Thread LIST: mit:%d, cht:%d, sct:%d, shct:%d, srt:%d, smt:%d\n",
            //         ctx->m_main_init_thread_id, ctx->m_hub_thread_id, 
            //         ctx->m_skype_client_thread_id, ctx->m_skype_audio_host_cmd_thread_id,
            //         ctx->m_sip_runtime_thread_id, ctx->m_sip_media_thread_id);
            curr_time = time(NULL);
            cmder_swap_queue.clear();
            for (int i = ctx->m_cmder_timer_queue.count() - 1; 
                 !ctx->m_hub_thread_quit_loop && i >= 0; --i) {
                curr_cmd = ctx->m_cmder_timer_queue.at(i);
                fprintf(stdout, "%d bus curr_cmd: %p\n", ctx->m_ctx_id, curr_cmd);
                if (curr_cmd != NULL 
                    && curr_time >= curr_cmd->delay_time + curr_cmd->entry_time) {
                    // fprintf(stdout, "curr_cmd: %p, %d\n", curr_cmd, ctx->m_cmder_timer_queue.count());
                    ctx->m_cmder_timer_queue.takeAt(i);
                    cmder_swap_queue.append(curr_cmd);
                    // fprintf(stdout, "curr_cmd: %p, %d\n", curr_cmd, ctx->m_cmder_timer_queue.count());
                    // wait_rc = contex_hub_thread_process_passby_command(curr_cmd);
                }
            }
            for (int i = cmder_swap_queue.count() - 1; i >= 0; --i) {
                curr_cmd = cmder_swap_queue.at(i);
                assert(curr_cmd != NULL);
                wait_rc = contex_hub_thread_process_passby_command(curr_cmd);
                delete curr_cmd; curr_cmd = NULL;
            }
            cmder_swap_queue.clear();
        } else if (wait_rc == EINVAL) {
            fprintf(stdout, "cond wait error EINVAL in ctx %d: %d, %s\n", 
                    ctx->m_ctx_id, wait_rc, strerror(errno));
        } else if (wait_rc == EPERM) {
            fprintf(stdout, "cond wait error EPERM in ctx %d: %d, %s\n", 
                    ctx->m_ctx_id, wait_rc, strerror(errno));
        } else {
            fprintf(stdout, "cond wait error in ctx %d: %d, %s\n", 
                    ctx->m_ctx_id, wait_rc, strerror(errno));
        }

        pthread_mutex_unlock(&ctx->m_cmder_mutex);
    }
    
    return NULL;
}

// run in fork
int serv_instance_main(int argc, char **argv, int way_id)
{
    (void)(argv);
    (void)(argv);

    int quit_main_loop = 0;
    ServContex *ctxes[100] = {0};
    ServContex *ctx;
    char storage_dir[256] = {0};// = getenv("HOME"); //"/tmp/";
    char *app_token = "xDbixo3eTW";
    const int way_count = 1;
    int iret;
    Database *db = NULL;
    QVector<QPair<QString, QString> > switchers;
    QString switcher_skype_name;
    QString switcher_password;

    switchers = Configs().getSwitchers();

    if (switchers.count() < way_count) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"switcher account not enough, check .serv.ini config file.";
        exit(-2);
    }

    db = new Database();
    if (!db->connectdb()) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"connect database error";
        exit(-1);
    }

    srandom(time(NULL));
    // init sip contex
    iret = init_sip_app_contex();
    iret = check_pjsip_config_validation();

    // init global contex 
    memset(storage_dir, 0, sizeof(storage_dir));
    snprintf(storage_dir, sizeof(storage_dir),
             "%s/%s/ipc_keys/ipc_key/", getenv("HOME"), Configs().get_storage_path().toAscii().data());
    memset(storage_dir, 0, sizeof(storage_dir));
    snprintf(storage_dir, sizeof(storage_dir),
             "%s/%s", getenv("HOME"), Configs().get_storage_path().toAscii().data());
    // fprintf(stderr, "storage_dir: %s\n", storage_dir);

    for (int i = 0 ; i < way_count ; ++i) {
        switcher_skype_name = switchers.at(way_id).first;
        switcher_password = switchers.at(way_id).second;

        ctxes[way_id] = ctx = new ServContex(way_id, 0, storage_dir, switcher_skype_name.toAscii().data());
        ctx->m_db = db;

        // ordered function call
        ctx->init_sxs_switcher();
        ctx->init_pcmhost_thread();
        ctx->init_websocket_thread();
        ctx->connect_skype_runtime(app_token, 8950 + way_id,
                                   switcher_skype_name, switcher_password);
        fprintf(stderr, "Next ctx... %d\n", way_id);
    }

    fprintf(stdout, "Enter main blocked loop pid: %ld ...\n", syscall(__NR_gettid));

    pthread_mutex_t cmder_mutex;
    pthread_mutexattr_t cmder_mutex_attr;
    pthread_cond_t cmder_cond;
    pthread_condattr_t cmder_cond_attr;

    pthread_mutexattr_init(&cmder_mutex_attr);
    pthread_mutex_init(&cmder_mutex, &cmder_mutex_attr);
    pthread_condattr_init(&cmder_cond_attr);
    pthread_cond_init(&cmder_cond, &cmder_cond_attr);

    pthread_mutexattr_destroy(&cmder_mutex_attr);
    pthread_condattr_destroy(&cmder_cond_attr);

    struct timespec to_time;
    int wait_rc = 0;
    for (;quit_main_loop == 0;) {
        pthread_mutex_lock(&cmder_mutex);
        clock_gettime(CLOCK_REALTIME, &to_time);
        // to_time.tv_nsec += 50 * 1000; // 1000000=1s, 50 * 1000 = 50ms
        to_time.tv_sec += 8;
 
        wait_rc = pthread_cond_timedwait(&cmder_cond, &cmder_mutex, &to_time);
        if (wait_rc == 0) {
            fprintf(stdout, "someone wake up me in ctx %d, has something to do now.\n",
                    10000+way_count);
        } else if (wait_rc == ETIMEDOUT) {
            // fprintf(stdout, "Timeout wake up(%d), take a look only. %ld\n", 
            //         10000+way_count,  to_time.tv_nsec);
        } else if (wait_rc == EINVAL) {
            fprintf(stdout, "cond wait error EINVAL in ctx %d: %d, %s\n", 
                    10000+way_count, wait_rc, strerror(errno));
        } else if (wait_rc == EPERM) {
            fprintf(stdout, "cond wait error EPERM in ctx %d: %d, %s\n", 
                    10000+way_count, wait_rc, strerror(errno));
        } else {
            fprintf(stdout, "cond wait in ctx %d error: %d, %s\n", 
                    10000+way_count, wait_rc, strerror(errno));
        }

        pthread_mutex_unlock(&cmder_mutex);
    }

    // for (;quit_main_loop == 0;) {
    //     sleep(1);
    // }

    // clean up here
    for (int i = 0 ; i < way_count ; ++i) {
        ctx = ctxes[i];
        fprintf(stderr, "Next exit ctx... %d\n", i);
    }

    fprintf(stdout, "Exit main blocked loop pid: %ld ...\n", syscall(__NR_gettid));

    return 0;
}

/**
   -n count   启动通话线路数, 1 -> 100, 默认10个。

   单进程多线程不支持多个Skype*实例？forum
   还真的不行，看来只能使用多进程结构的程序。
 */
int serv_main(int argc, char **argv)
{
    int pid = 0; 
    int idx = 0;
    pid_t pids[100] = {0};
    int line_cnt = 3;

    for (idx = 0; idx < line_cnt ; idx ++) {
    restart_process:
        pid = fork();
        if (pid  == -1) {
            printf("fork error:\n");
            exit(99);
        } else if (pid == 0) {
            // child process
            printf("child: get myself pid: %d, argc=%d, seqno=%d\n", 
                   getpid(), argc, idx);
            pid = serv_instance_main(argc, argv, idx);
            sleep(10);
            printf("child %d: before exit.\n", getpid());
            exit(1);
        } else {
            pids[idx] = pid;
            printf("got child pid: %d\n", pid);
        }
    }

    // parent process here
    int p_status = 0;
    for (;;) {
        pid = waitpid(-1, &p_status, 0);
        // 如果没有子进程，那么这个waitpid会立即返回，形成死循环了。
        printf("waitpid return: %d, %d, %s\n", pid, errno, strerror(errno));
        for (idx = 0; idx < line_cnt ; idx ++) {
            if (pids[idx] == pid) {
                // how, restart it???
                printf("it's(%d) idx is: %d\n", pid, idx);
                pids[idx] = 0;
                // goto restart_process;
                if (0) {
                    goto restart_process;
                }
                break;
            }
        }
    }
    return 0;
}

///////////////////////
/*
  使用到的目录结构。
  /storage/gateways/
          /logs/
          /ipc_keys/
                   /ipc_key_xx
          /pids/
 */
///////////////////////
ServContex::ServContex(int ctx_id, int ws_fd, char *storage_dir, char *skype_name) 
    : m_ctx_id(ctx_id), m_ws_fd(ws_fd)
{
    this->m_db = NULL;

    this->m_hub_thread = -1;
    // this->m_cmder_mutex;
    // this->m_cmder_cond;

    this->m_skype = NULL;
    this->m_skype_call_conv = Conversation::Ref(0);
    this->m_port = NULL;
    this->m_pool = NULL;
    this->m_sip_call_id = -1;
    this->m_hangup_init_point = INIT_HANGUP_FROM_NONE;

    this->m_app_token = NULL;
    strncpy(this->m_storage_dir, storage_dir, sizeof(this->m_storage_dir));
    memset(this->m_localname, 0, sizeof(this->m_localname));
    snprintf(this->m_localname, sizeof(this->m_localname)-1,
             "%s/ipc_keys/ipc_key_un_name_%d.sock", this->m_storage_dir, this->m_ctx_id);
    this->m_account_name = strdup(skype_name);
    this->m_account_password = NULL;

    // qDebug()<<"local sock:"<<this->m_localname;

    memset(this->m_log_file, 0 , sizeof(this->m_log_file));
    snprintf(this->m_log_file, sizeof(this->m_log_file)-1, 
             "%s/logs/sk_client_%d", this->m_storage_dir, this->m_ctx_id);

    this->m_hub_thread_quit_loop = 0;
    this->m_websocket_thread_quit_loop = 0;

    this->pcmif_server = NULL;
    this->pcmif_cb_client = NULL;
    this->pcmif = NULL;

    this->m_main_init_thread_id = syscall(__NR_gettid);
    this->m_skype_client_thread_id = 0;
    this->m_skype_audio_host_cmd_thread_id = 0;
    this->m_skype_audio_host_media_thread_id = 0;
    this->m_sip_runtime_thread_id = 0;
    this->m_sip_media_thread_id = 0; // put frame, get frame

    // "%s/skype_cluster/ipc_keys/ipc_key_xx/
    // ipc_prefix like /path/to/ipc/
    // and this will be reformated to /path/to/ipc_key_{ctx_id}/
    memset(this->from_skypekit_key, 0, sizeof(this->from_skypekit_key));
    snprintf(this->from_skypekit_key, sizeof(this->from_skypekit_key),
             "%s/ipc_keys/ipc_key_%d/pcm_from_skypekit_key", this->m_storage_dir, this->m_ctx_id);
    memset(this->to_skypekit_key, 0, sizeof(this->to_skypekit_key));
    snprintf(this->to_skypekit_key, sizeof(this->to_skypekit_key),
             "%s/ipc_keys/ipc_key_%d/pcm_to_skypekit_key", this->m_storage_dir, this->m_ctx_id);

    // fprintf(stderr, "from ipc key: %s\n", this->from_skypekit_key);
    // fprintf(stderr, "to ipc key: %s\n", this->to_skypekit_key);

    /*
    SEReference ra = Account::Ref();
    fprintf(stdout, "SEReference NULL ?= Ref(0) : %d, %d %d \n",
            this->m_skype_call_conv == ra ? 1 : 0, ra.present() ? 1 : 0,
            this->m_skype_call_conv ? 1 : 0 );
    */

    int iv;
#ifdef HAVE_SOCK_NONBLOCK
    iv = socketpair(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK, 0, this->m_sock_pair_sv);
#else
    iv = socketpair(AF_LOCAL, SOCK_STREAM, 0, this->m_sock_pair_sv);
#endif
    assert(iv == 0);

    pthread_attr_t thread_attr;
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_condattr_init(&cond_attr);

    pthread_mutex_init(&this->m_cmder_mutex, &mutex_attr);
    pthread_cond_init(&this->m_cmder_cond, &cond_attr);

    pthread_attr_init(&thread_attr);
    int iret = pthread_create(&this->m_hub_thread, &thread_attr,
                              &server_contex_hub_proc, this);
    
    pthread_mutexattr_destroy(&mutex_attr);
    pthread_condattr_destroy(&cond_attr);
    pthread_attr_destroy(&thread_attr);

    if (iret == 0) {
    } else {
        fprintf(stdout, "Create hub(%d) thread faild, %d, %s\n", this->m_ctx_id,
                iret, strerror(errno));
    }
    assert(iret == 0);
}
ServContex::~ServContex() {
}

int ServContex::connect_skype_runtime(char *app_token, unsigned short app_port, QString skype_name, QString password)
{
    Q_UNUSED(skype_name);

    const char *host_ipaddr = "127.0.0.1";
    Sid::TransportInterface::Status status;
    
    this->m_app_token = app_token;
    this->m_app_port = app_port;
    // this->m_account_name = "abcd2113";
    // this->m_account_password = "l1.ll#23456";

    // this->m_account_name = strdup(skype_name.toAscii().data());
    this->m_account_password = strdup(password.toAscii().data());

    this->m_skype = new CSkype();
    this->m_skype->ctx = this;
    // status = this->m_skype->init(this->m_app_token, host_ipaddr, this->m_app_port, log_streams);
    qDebug()<<"connect skypekkkkk:"<<host_ipaddr<<this->m_app_port;
    if (1) {
        status = this->m_skype->init(this->m_app_token, host_ipaddr, this->m_app_port, this->m_log_file);
    } else {
        status = this->m_skype->init(this->m_app_token, this->m_localname, this->m_log_file);
    }
    if (status != Sid::TransportInterface::OK) {
        printf("\n::: Error connecting to skypekit, enter 'r' to reconnect...\n");

        // 应该使用延迟任务，像QTimer::SingleShot(5000, this, SLOT(timeout());
        this->post_timeout_cmd(RPCMD::SKYPE_NEED_RECONNECT, this, QStringList(), 3);

        return -1;
    }
        
    this->m_skype->start();
    
    Sid::String version;
    this->m_skype->GetVersionString(version);
    printf("sk runtime version: %s\n\n", (const char*) version);

    SEString MyAccountName = this->m_account_name;
    bool ok = this->m_skype->GetAccount(MyAccountName, this->m_skype->activeAccount);

    SEString MyAccountPwd = this->m_account_password;
    ok = this->m_skype->activeAccount->LoginWithPassword(MyAccountPwd, false, true);
    
    return 0;
}

int ServContex::connect_skype_runtime(char *app_token, unsigned short app_port)
{
    const char *host_ipaddr = "127.0.0.1";
    Sid::TransportInterface::Status status;
    
    this->m_app_token = app_token;
    this->m_app_port = app_port;
    // this->m_account_name = "abcd2113";
    // this->m_account_password = "l1.ll#23456";

    // this->m_account_name = strdup(skype_name.toAscii().data());
    // this->m_account_password = strdup(password.toAscii().data());

    this->m_skype = new CSkype();
    this->m_skype->ctx = this;
    // status = this->m_skype->init(this->m_app_token, host_ipaddr, this->m_app_port, log_streams);
    qDebug()<<"connect skypekkkkk:"<<host_ipaddr<<this->m_app_port;
    if (1) {
        status = this->m_skype->init(this->m_app_token, host_ipaddr, this->m_app_port, this->m_log_file);
    } else {
        status = this->m_skype->init(this->m_app_token, this->m_localname, this->m_log_file);
    }
    if (status != Sid::TransportInterface::OK) {
        printf("\n::: Error connecting to skypekit, enter 'r' to reconnect...\n");

        // 应该使用延迟任务，像QTimer::SingleShot(5000, this, SLOT(timeout());
        this->post_timeout_cmd(RPCMD::SKYPE_NEED_RECONNECT, this, QStringList(), 3);

        return -1;
    }
        
    this->m_skype->start();
    
    Sid::String version;
    this->m_skype->GetVersionString(version);
    printf("sk runtime version: %s\n\n", (const char*) version);

    return 0;
}

int ServContex::login_skype_runtime(QString skype_name, QString password)
{
    this->m_account_password = strdup(password.toAscii().data());

    SEString MyAccountName = this->m_account_name;
    bool ok = this->m_skype->GetAccount(MyAccountName, this->m_skype->activeAccount);

    SEString MyAccountPwd = this->m_account_password;
    ok = this->m_skype->activeAccount->LoginWithPassword(MyAccountPwd, false, true);

    return 0;
}

int ServContex::reconnect_skype_runtime()
{
    const char *host_ipaddr = "127.0.0.1";
    Sid::TransportInterface::Status status;

    Skype *old_skype = this->m_skype;
    this->m_skype = NULL;
    delete old_skype;

    this->m_skype = new CSkype();
    this->m_skype->ctx = this;
    // status = this->m_skype->init(this->m_app_token, host_ipaddr, this->m_app_port, this->m_log_file);
    if (1) {
        status = this->m_skype->init(this->m_app_token, host_ipaddr, this->m_app_port, this->m_log_file);
    } else {
        status = this->m_skype->init(this->m_app_token, this->m_localname, this->m_log_file);
    }

    if (status != Sid::TransportInterface::OK) {
        printf("\n::: Error connecting to skypekit, enter 'r' to reconnect...\n");
        return -1;
    }
        
    this->m_skype->start();
    
    Sid::String version;
    this->m_skype->GetVersionString(version);
    printf("sk runtime version: %s\n\n", (const char*) version);

    SEString MyAccountName = this->m_account_name;
    bool ok = this->m_skype->GetAccount(MyAccountName, this->m_skype->activeAccount);

    SEString MyAccountPwd = this->m_account_password;
    ok = this->m_skype->activeAccount->LoginWithPassword(MyAccountPwd, false, true);
    
    return 0;
}
int ServContex::relogin_skype_runtime() 
{
    Sid::String version;
    this->m_skype->GetVersionString(version);
    printf("sk runtime version: %s\n\n", (const char*) version);

    SEString MyAccountName = this->m_account_name;
    bool ok = this->m_skype->GetAccount(MyAccountName, this->m_skype->activeAccount);

    SEString MyAccountPwd = this->m_account_password;
    ok = this->m_skype->activeAccount->LoginWithPassword(MyAccountPwd, false, true);

    return 0;
}

int ServContex::init_sxs_switcher()
{
    char pool_name[30] = {0};
    pj_status_t status;
    unsigned enc_type;
    void *enc_param;
    unsigned options;

    // pj_pool_t * 	pjsua_pool_create (const char *name, pj_size_t init_size, pj_size_t increment)
    snprintf(pool_name, sizeof(pool_name)-1, "ctx_pool_%d", this->m_ctx_id);
    this->m_pool = pjsua_pool_create(pool_name, 2000, 1000);

    enc_type = 0;
    enc_param = NULL;
    options = 0;
    status = pjsua_sxs_switcher_create((pj_pool_t*)this->m_pool, enc_type, 
                                       enc_param, options, &this->m_rec_id);
    assert(status == PJ_SUCCESS);

    status = pjsua_sxs_switcher_get_port(this->m_rec_id, (pjmedia_port**)&this->m_port);
    assert(status == PJ_SUCCESS);

    return 0;
}
int ServContex::init_websocket_thread()
{
    pthread_attr_t attr;
    int iret = 0;

    pthread_attr_init(&attr);
    iret = pthread_create(&this->m_websock_thread, &attr, &websocket_server_proc, this);
    pthread_attr_destroy(&attr);
    return 0;

    return 0;
}

int ServContex::init_pcmhost_thread()
{
    pthread_attr_t attr;
    int iret = 0;

    pthread_attr_init(&attr);
    iret = pthread_create(&this->pcmhost_main_thread, &attr, &pcm_host_main_proc, this);
    pthread_attr_destroy(&attr);
    return 0;
}

void *pcm_host_main_proc(void *args)
{
    ServContex *ctx = (ServContex *)args;

    ctx->pcmhost_main_thread_pid = syscall(__NR_gettid);
    fprintf(stderr, "pcm host (%d)'s pid is: %d\n", ctx->m_ctx_id, ctx->pcmhost_main_thread_pid);

    // main proc
	Sid::SkypePCMInterfaceServer *pcmif_server = new Sid::SkypePCMInterfaceServer();
	Sid::SkypePCMCallbackInterfaceClient *pcmif_cb_client = new Sid::SkypePCMCallbackInterfaceClient();

	SkypePCMInterface* pcmif = SkypePCMInterfaceGetEx(pcmif_cb_client, ctx);
	pcmif_server->set_if(pcmif);
    PCMSxsHost *sxshost = static_cast<PCMSxsHost*>(pcmif);
    pjmedia_sxs_set_pcm_host((pjmedia_port*)ctx->m_port, ctx, sxshost);
    ctx->pcmif_server = pcmif_server;
    ctx->pcmif_cb_client = pcmif_cb_client;
    ctx->pcmif = pcmif;

    ctx->m_skype_audio_host_cmd_thread_id = syscall(__NR_gettid);

	Sid::String fromskypekitkey;
	Sid::String toskypekitkey;

	// fromskypekitkey.Format( "%spcm_from_skypekit_key", parser.m_IpcPrefix);
	// toskypekitkey.Format( "%spcm_to_skypekit_key", parser.m_IpcPrefix);

    fromskypekitkey = ctx->from_skypekit_key;
    toskypekitkey = ctx->to_skypekit_key;

	pcmif_server->Connect(fromskypekitkey.data(), 0);
	pcmif_cb_client->Connect(toskypekitkey.data(), 500);

	Sid::Protocol::Status status;
	do {
		status = pcmif_server->ProcessCommands();
	} while (status == Sid::Protocol::OK);

	// SkypePCMInterfaceRelease(pcmif);
	pcmif_server->Disconnect();
	pcmif_cb_client->Disconnect();

	// delete pcmif_server;
	// delete pcmif_cb_client;

	printf("PCMServerTransport disconnected, exiting from pcmtesthost\n");

    return NULL;
}

void *websocket_connection_proc(void *args)
{
#define WS_EPOLL_MAX_EVENTS 60

    ServContex *ctx = (ServContex*)args;

    fprintf(stdout, "Entry websocket connected thread loop, ctx id: %d, thread id: \n",
            ctx->m_ctx_id);
    
    int ret;
    unsigned short serv_port;
    QString serv_ipaddr;
    socklen_t slen;
    int epfd;
    int nfds, n;
    int cfd;
    struct epoll_event ev, events[60];
    char buff[1000];

    epfd = epoll_create(20);
    if (epfd == -1) {
        perror("epoll_create");
        assert(epfd != -1);
    }
    
    struct sockaddr_in sock_addr;
    slen = sizeof(sock_addr);

    struct epoll_event ctl_evt;
    ctl_evt.events = EPOLLIN;
    ctl_evt.data.fd = ctx->m_ws_fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, ctx->m_ws_fd, &ctl_evt);
    qDebug()<<"add swich fd to epoll:"<<ctx->m_ctx_id<<epfd
            <<" fd:"<<ctx->m_ws_fd<<" ret:"<<ret<<strerror(errno);

    
    nfds = epoll_wait(epfd, events, WS_EPOLL_MAX_EVENTS, -1);
    if (nfds == -1) {
        perror("epoll_pwait");
    }

    for (n = 0; n < nfds; ++n) {
        if (events[n].events & EPOLLIN) {
            if (events[n].data.fd == ctx->m_ws_fd) {
                // do what
                ret = ::read(ctx->m_ws_fd, buff, sizeof(buff));
                if (ret < 0) {

                } else if (ret == 0) {
                    
                } else {
                    
                }
            }
        }
    }
    
    return NULL;
}

int ServContex::init_websocket_connection_thread()
{
    pthread_attr_t attr;
    int iret = 0;

    pthread_attr_init(&attr);
    iret = pthread_create(&this->m_websock_connection_thread, &attr, &websocket_connection_proc, this);
    pthread_attr_destroy(&attr);
    return 0;
}

void ServContex::onNewForwardCallArrived(SEReference &conversation, QString callerName, QString calleeName, int skypeCallID)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<callerName<<calleeName<<skypeCallID;
    QString callee_phone;

    assert(conversation);
    CConversation::Ref conv = (static_cast<CConversation*>(conversation.operator->()))->ref();

    // 查找 call pair
    callee_phone = this->m_db->getCallPeer(callerName);
    if (callee_phone.isEmpty() || callee_phone.length() == 0) {
        qDebug()<<"Error: call pair not found.";
        // this->mSkype->setCallHangup(QString::number(skypeCallID));
        // this->send_ws_command_106(callerName, calleeName, skypeCallID, 
        //                           QString("Error: your supplied account and call acount is not match.")
        //                           );
        // conv->LeaveLiveSession(); // hangup skype call right now
        this->hangup_skype_call(conversation);
        return;
    }

    // accept
    this->m_skype_call_conv = conv;
    conv->PickUpCall();

    // this->mSkype->setCallInputNull(QString("%1").arg(skypeCallID));
    // this->mSkype->setCallOutputNull(QString("%1").arg(skypeCallID));
    // this->mSkype->setCallInputFile(QString::number(skypeCallID),
    //                                QString(getenv("HOME")) + "/SKYPE1.wav");
    // QString("/home/gzleo/SKYPE1.wav"));
    // this->mSkype->setCallInputFile(QString("%1").arg(skypeCallID), QString("/dev/zero"));
    // this->mSkype->setCallOutputFile(QString::number(skypeCallID), QString("/dev/null"));
    
    // accept
    // this->mSkype->answerCall(QString::number(skypeCallID));
    // TODO, 如果answerCall 出错,下面不能继续执行. 所以,下面几行,应该放在slot onSkypeCallAnswered中
}

void ServContex::onSkypeForwardCallAnswered(SEReference &conversation, int skypeCallID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<skypeCallID;
    // pj_status_t status;
    QString callee_phone;

    assert(conversation);
    CConversation::Ref conv = (static_cast<CConversation*>(conversation.operator->()))->ref();

    callee_phone = this->m_db->getCallPeer(callerName);
    if (callee_phone.isEmpty() || callee_phone.length() == 0) {
        qDebug()<<"Error: call pair not found.";
        // this->mSkype->setCallHangup(QString::number(skypeCallID));
        // conv->LeaveLiveSession(); // hangup skype call right now
        this->hangup_skype_call(conversation);
        return;
    }

    // // sip call, serv addr 要放在配置文件中
    QHash<QString, int> sip_servers = Configs().getSipServers();
    Q_ASSERT(sip_servers.count() > 0);
    QString serv_addr = sip_servers.begin().key();//"202.108.29.234:4060";// "172.24.172.21:4060";
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"before sip call phone call";

    // this->mSkypeSipCallMap.insert(skypeCallID, SSCM_WAIT_SIP_CALL_ID);

    QStringList args;
    args<<callerName<<callee_phone<<serv_addr<<QString::number(skypeCallID);
    // QString arg_list = args.join(",");

    // char *arg_str = "yat-sen,99008665108013552776960,202.108.29.234:4060,56789";
    // sip_call_phone(ctx, arg_str); // Assertion `!"Calling pjlib from unknown/external thread.
    // QStringList args = QString(arg_str).split(',');
    // args << QString::number(obj_id)
    //      << QString::number(conv->getOID());
    // << QString::number(video->getOID());
    qDebug()<<args;
    this->post_schedule_cmd(10, this, args);

    // int cmdlen = 0;
    // char *wbuf = new_rpc_command(10, &cmdlen, "s", arg_list.toAscii().data());
    // this->skype_sip_rpc_peer->write(wbuf, 2*sizeof(int) + cmdlen);
    // free(wbuf);

    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"after sip call phone call";
}

int ServContex::hangup_skype_call(SEReference &conversation)
{
    assert(conversation);
    CConversation::Ref conv = (static_cast<CConversation*>(conversation.operator->()))->ref();

    conv->LeaveLiveSession(); // hangup skype call right now

    int irv = 0;
    SEString caller_identity;
    QString caller_name;

    conv->GetPropIdentity(caller_identity);
    caller_name = QString(caller_identity.data());

    irv = this->m_db->releaseGateway(QString(), QString(this->m_account_name));

    this->send_ws_command_108(caller_name, QString(this->m_account_name), 0, 0, QString());

    return 0;
}

// this->send_ws_command_100(callerName, gateway, QString("202.108.12.212"), ws_port);
bool ServContex::send_ws_command_100(QString caller_name, QString gateway, QString host, unsigned short port)
{
    QString cmdline = QString("%1$%2$%3$%4$%5")
        .arg(100).arg(caller_name).arg(gateway).arg(host).arg(port);

    // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());
    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_102(QString caller_name, QString data)
{
    QString cmdline;

    cmdline = data;

    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_104(QString caller_name, QString gateway, int skype_call_id, QString msg)
{
    QString cmdline;

    cmdline = QString("104$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(msg);
    // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());

    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_106(QString caller_name, QString gateway, int skype_call_id, QString msg)
{
    QString cmdline;

    cmdline = QString("106$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(msg);
    // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());
  
    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_108(QString caller_name, QString gateway, int skype_call_id, int hangupcause, QString reason)
{

    QString cmdline;

    cmdline = QString("108$%1$%2$%3$%4$%5").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(hangupcause).arg(reason);
    // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());

    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_110(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;

    cmdline = QString("110$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);
        // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());

    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_112(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;

    cmdline = QString("112$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);
    // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());

    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_114(QString caller_name, QString gateway, int skype_call_id, QString reason)
{
    QString cmdline;

    cmdline = QString("114$%1$%2$%3$%4").arg(caller_name).arg(gateway)
        .arg(skype_call_id).arg(reason);

    return this->send_ws_command_common(caller_name, cmdline);
    return true;
}

bool ServContex::send_ws_command_common(QString caller_name, QString cmdline)
{
    int fd = -1;
    int ilen = 0;
    bool ok = false;

    if (!this->m_websocket_client_fds.rightContains(caller_name)) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"Why no ws map found???"<<caller_name;
    } else {
        fd = this->m_websocket_client_fds.findRight(caller_name).value();
        Q_ASSERT(fd != -1);
        // ilen = this->scn_ws_serv->wssend(sock, cmdline.toAscii().data(), cmdline.length());
        QStringList strlist;
        strlist << caller_name << QString::number(fd)
                << cmdline << QString::number(cmdline.length());
        ok = this->post_schedule_cmd(RPCMD::SEND_WS_NOTE_MSG, this, strlist);
    }

    return ok;
}

// callback from sip using ctx
int ServContex::on_sip_call_state_changed(int sip_call_id)
{
    pj_status_t status;
    pjsua_call_info ci;

    status = pjsua_call_get_info(sip_call_id, &ci);    

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

        ServContex *ctx = this;
        // sip call hangup
        if (ctx->m_hangup_init_point == INIT_HANGUP_FROM_NONE) {
            ctx->m_hangup_init_point = INIT_HANGUP_FROM_SIP;
            ctx->post_schedule_cmd(13, ctx, QStringList());
            // ThreadPassbyCommand *cmd = new ThreadPassbyCommand();
            // cmd->cmdno = 13;
            // cmd->ctx = ctx;

            // ctx->m_cmder_queue.enqueue(cmd);
            // int iret = pthread_cond_signal(&ctx->m_cmder_cond);
            // assert(iret == 0);
        } else {
            // do nothing
            ctx->m_hangup_init_point = INIT_HANGUP_FROM_NONE;
        }

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

    return 0;
}

int ServContex::on_sip_call_media_state_changed(int sip_call_id)
{
    int iret;
    pj_status_t status;
    pjsua_call_info ci;
    QStringList args;

    status = pjsua_call_get_info(sip_call_id, &ci);    

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        args.clear();
        args << QString::number(0) << QString::number(sip_call_id) 
             << QString::number(0);
        iret = this->post_schedule_cmd(17, this, args);

        args.clear();
        args << QString::number(0) << QString::number(sip_call_id) 
             << QString::number(0);
        iret = this->post_schedule_cmd(15, this, args);

        // ThreadPassbyCommand *cmd = new ThreadPassbyCommand();
        // cmd->cmdno = 17;
        // cmd->ctx = this;
        // cmd->args << QString::number(0) << QString::number(sip_call_id) 
        //           << QString::number(0);
        // this->m_cmder_queue.enqueue(cmd);

        // cmd = new ThreadPassbyCommand();
        // cmd->cmdno = 15;
        // cmd->ctx = this;
        // cmd->args << QString::number(0) << QString::number(sip_call_id) 
        //           << QString::number(0);
        // this->m_cmder_queue.enqueue(cmd);

        // iret = pthread_cond_signal(&this->m_cmder_cond);
        // assert(iret == 0);
    } else {
        args.clear();
        args << QString::number(0) << QString::number(sip_call_id) 
             << QString::number(0);
        iret = this->post_schedule_cmd(22, this, args);

        args << QString::number(0) << QString::number(sip_call_id) 
             << QString::number(0);
        iret = this->post_schedule_cmd(23, this, args);

        // ThreadPassbyCommand *cmd = new ThreadPassbyCommand();
        // cmd->cmdno = 22;
        // cmd->ctx = this;
        // cmd->args << QString::number(0) << QString::number(sip_call_id) 
        //           << QString::number(0);
        // this->m_cmder_queue.enqueue(cmd);

        // cmd = new ThreadPassbyCommand();
        // cmd->cmdno = 23;
        // cmd->ctx = this;
        // cmd->args << QString::number(0) << QString::number(sip_call_id) 
        //           << QString::number(0);
        // this->m_cmder_queue.enqueue(cmd);

        // iret = pthread_cond_signal(&this->m_cmder_cond);
        // assert(iret == 0);
    }

    return 0;
}

int ServContex::on_sip_call_dtmf_digit_arrived(int sip_call_id, int dtmf)
{
    int iret;
    pj_status_t status;
    pjsua_call_info ci;
    QStringList args;

    status = pjsua_call_get_info(sip_call_id, &ci);    

    args << QString::number(sip_call_id) << QString::number(dtmf);
    iret = this->post_schedule_cmd(RPCMD::SIP_CALL_DTMF_DIGIT, this, args);

    return 0;
}

int ServContex::post_schedule_cmd(int cmdno, ServContex *ctx, QStringList args)
{
    assert(ctx != NULL);

    ThreadPassbyCommand *tcmd = new ThreadPassbyCommand();
    tcmd->cmdno = cmdno;
    tcmd->ctx = ctx;
    tcmd->args = args;

    tcmd->entry_time = time(NULL);
    gettimeofday(&tcmd->entry_tv, NULL);

    ctx->m_cmder_queue.enqueue(tcmd);
    int iret = pthread_cond_signal(&ctx->m_cmder_cond);
    assert(iret == 0);

    return 0;
}

int post_schedule_cmd(int cmdno, ServContex *ctx, QByteArray data)
{

}

int ServContex::post_timeout_cmd(int cmdno, ServContex *ctx, QStringList args, int delay)
{
    assert(ctx != NULL);

    ThreadPassbyCommand *tcmd = new ThreadPassbyCommand();
    tcmd->cmdno = cmdno;
    tcmd->ctx = ctx;
    tcmd->args = args;

    tcmd->delay_time = delay;
    tcmd->entry_time = time(NULL);
    gettimeofday(&tcmd->entry_tv, NULL);

    ctx->m_cmder_timer_queue.append(tcmd);

    return 0;
}

QString ServContex::serverIpAddr(int type)
{
    QString ipaddr;
    QHostAddress addr;
    QList<QHostAddress> addr_list;
    QList<QString> addr_str_list;

    // addr = this->ws_serv_sock->serverAddress();
    // qDebug()<<"ws listen ip addr:"<<addr;
    // if (addr == QHostAddress::Null) {
        
    // } else {
    //     ipaddr = addr.toString();
    // }

    addr_list = QNetworkInterface::allAddresses();
    qDebug()<<addr_list;

    for (int i = 0 ; i < addr_list.count() ; i ++) {
      addr_str_list.append(addr_list.at(i).toString());
    }
    // qSort(list.begin(), list.end(), qGreater<int>());
    qSort(addr_str_list.begin(), addr_str_list.end(), qGreater<QString>());

    if (addr_list.count() == 0) {
    } else if (addr_list.count() == 1) {
        // must be 127.0.0.1
        ipaddr = addr_list.at(0).toString();
    } else {
        for (int i = 0 ; i < addr_list.count(); i ++) {
            addr = addr_list.at(i);
            ipaddr = addr.toString();
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
                    break;
                } else if (ipaddr.startsWith("172.24.")) {
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

    return ipaddr;
}

////// invoke helper
int skype_helper_invoke_skype_connected(SERootObject *root)
{
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    return 0;
}

int skype_helper_invoke_skype_disconnected(SERootObject *root)
{
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    // 应该使用延迟任务，像QTimer::SingleShot(5000, this, SLOT(timeout());
    ctx->post_timeout_cmd(RPCMD::SKYPE_NEED_RECONNECT, ctx, QStringList(), 3);

    return 0;
}

int skype_helper_invoke_skype_onconversation_list_change(SERootObject *root, const SEReference& conversation, const int type, const bool added)
{
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    assert(conversation);
    CConversation::Ref conv = (static_cast<CConversation*>(conversation.operator->()))->ref();

    ///// 
    SEString DisplayName;
    Conversation::LOCAL_LIVESTATUS LiveStatus;

    conv->GetPropDisplayname(DisplayName);
    conv->GetPropLocalLivestatus(LiveStatus);

    if (type == Conversation::LIVE_CONVERSATIONS) {
        fprintf(stdout, "want pickup 1\n");
        if (LiveStatus == Conversation::RINGING_FOR_ME) {
            fprintf(stdout, "want pickup 2\n");
            // ctx->m_skype_call_conv = conv;
            // DRef<CConversation, Conversation>(conv)->PickUpCall();
            SEString caller_identity;
            SEString callee_identity;
            
            QString caller_name;
            QString callee_name;
            
            conv->GetPropIdentity(caller_identity);
            ctx->m_skype->GetDefaultAccountName(callee_identity);

            caller_name = QString(caller_identity.data());
            callee_name = QString(callee_identity.data());
            
            ctx->onNewForwardCallArrived(conv, caller_name, callee_name, 0);
        }
        if (LiveStatus == Conversation::NONE) {
            
        }
    }

    return 0;
}

int skype_helper_invoke_skype_onmessage(SERootObject *root, const SEReference & message, const bool& changesInboxTimestamp, const SEReference& supersedesHistoryMessage, const SEReference& conversation)
{
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    if (!message) {
        return -1;
    }
    CMessage::Ref msg = (static_cast<CMessage*>(message.operator->()))->ref();


    CMessage::Ref supmsg;
    if (supersedesHistoryMessage) {
        supmsg = (static_cast<CMessage*>(supersedesHistoryMessage.operator->()))->ref();
    }

    assert(conversation);
    CConversation::Ref conv = (static_cast<CConversation*>(conversation.operator->()))->ref();

    return 0;
}


int skype_helper_invoke_account_onchange(SERootObject *root, SEReference &account, int prop)
{
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    assert(account);
    CAccount::Ref acc = (static_cast<CAccount*>(account.operator->()))->ref();

    ctx->m_skype_client_thread_id = syscall(__NR_gettid);

    if (prop == Account::P_STATUS) {
        unsigned int acc_status = acc->GetUintProp(Account::P_STATUS);
        if (acc_status == Account::LOGGED_IN) {
            
        } else if (acc_status == Account::LOGGED_OUT) {
            // 应该使用延迟任务，像QTimer::SingleShot(5000, this, SLOT(timeout());
            // ThreadPassbyCommand *cmd = new ThreadPassbyCommand();
            // cmd->cmdno = RPCMD::SKYPE_NEED_RELOGIN;
            // cmd->ctx = ctx;

            // cmd->delay_time = 3;
            // cmd->start_time = time(NULL);
            // ctx->m_cmder_timer_queue.append(cmd);
            ctx->post_timeout_cmd(RPCMD::SKYPE_NEED_RELOGIN, ctx, QStringList(), 3);
        } else {

        }
    }

    return 0;
}


int skype_helper_invoke_conversation_onchange(SERootObject *root, SEReference &conversation, int prop)
{
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    assert(conversation);
    CConversation::Ref conv = (static_cast<CConversation*>(conversation.operator->()))->ref();

    if (prop == Conversation::P_LOCAL_LIVESTATUS) {
        Conversation::LOCAL_LIVESTATUS LiveStatus;
        conv->GetPropLocalLivestatus(LiveStatus);

        if (LiveStatus == Conversation::RINGING_FOR_ME) {
            printf("RING RING..\n");
            SEString caller_identity;
            SEString callee_identity;
            
            QString caller_name;
            QString callee_name;
            
            conv->GetPropIdentity(caller_identity);
            ctx->m_skype->GetDefaultAccountName(callee_identity);

            caller_name = QString(caller_identity.data());
            callee_name = QString(callee_identity.data());
            
            ctx->onNewForwardCallArrived(conv, caller_name, callee_name, 0);
            // ctx->m_skype_call_conv = conv;
            // conv->PickUpCall();
        };

        if (LiveStatus == Conversation::IM_LIVE) {
            // Clearing previously live session reference, as we no longer care
            // if that object gets garbage collected or not.
            // conv->LiveSession = NULL;  // 感觉这么写不对
            conv->LiveSession = Conversation::Ref(0);
            printf("Live session is up.\n");
        };

        if ((LiveStatus == Conversation::RECENTLY_LIVE) || (LiveStatus == Conversation::NONE)) {
            // Here we will keep the last live session reference,
            // so that the object would remain in cache and events
            // for it will still fire.
            // conv->LiveSession = Conversation::Ref();  // 感觉这么写不对。改成自已经的一行，看有什么效果
            conv->LiveSession = conv;
            printf("Call has ended..\n");
            conv->IsLiveSessionUp = false;
        };
    };

    return 0;
}

int skype_helper_invoke_participant_onchange(SERootObject *root, SEReference &participant, int prop)
{
    // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    assert(participant);
    CParticipant::Ref part = (static_cast<CParticipant*>(participant.operator->()))->ref();

    SEString identity_name;
    SEString callee_identity;
    if (prop == Participant::P_VOICE_STATUS) {
        part->GetPropIdentity(identity_name); // caller_name
        Participant::VOICE_STATUS v_status;
        part->GetPropVoiceStatus(v_status);
        skype->GetDefaultAccountName(callee_identity);

        if (v_status == Participant::SPEAKING) {
            // answered ok
            int obj_id;
            obj_id = part->getOID();
            CConversation::Ref conv;
            part->GetPropConvoId(conv);
            Video::Ref video;
            part->GetVideo(video);

            QString caller_name = QString(identity_name.data());
            QString callee_name = QString(callee_identity.data());

            ctx->onSkypeForwardCallAnswered(conv, 0, caller_name, callee_name);

            // char *arg_str = "yat-sen,99008668056013552776960,202.108.29.234:4060,56789";
            // char *arg_str = "yat-sen,99008665108013552776960,202.108.29.234:4060,56789";
            // // sip_call_phone(ctx, arg_str); // Assertion `!"Calling pjlib from unknown/external thread.
            // QStringList args = QString(arg_str).split(',');
            // args << QString::number(obj_id)
            //      << QString::number(conv->getOID());
            // // << QString::number(video->getOID());
            // qDebug()<<args;
            // ctx->post_schedule_cmd(10, ctx, args);
            // ThreadPassbyCommand *tcmd = new ThreadPassbyCommand();
            // tcmd->cmdno = 10;
            // tcmd->ctx = ctx;
            // tcmd->args = args;

            // ctx->m_cmder_queue.enqueue(tcmd);
            // int iret = pthread_cond_signal(&ctx->m_cmder_cond);
            // assert(iret == 0);
        }

        if (v_status == Participant::VOICE_CONNECTING
            || v_status == Participant::RINGING) {
            SEString use_identity = "tel654321098";
            part->SetLiveIdentityToUse(use_identity);
        }

        // part->Hangup();
        if (v_status == Participant::VOICE_STOPPED) {
            // skype call hangup
            if (ctx->m_hangup_init_point == ServContex::INIT_HANGUP_FROM_NONE) {
                ctx->m_hangup_init_point = ServContex::INIT_HANGUP_FROM_SKYPE;
                ctx->post_schedule_cmd(12, ctx, QStringList());
                // ThreadPassbyCommand *cmd = new ThreadPassbyCommand();
                // cmd->cmdno = 12;
                // cmd->ctx = ctx;

                // ctx->m_cmder_queue.enqueue(cmd);
                // int iret = pthread_cond_signal(&ctx->m_cmder_cond);
                // assert(iret == 0);
                fprintf(stdout, "hangup from skype now.\n");
                ctx->hangup_skype_call(ctx->m_skype_call_conv);
            } else {
                // do nothing
                ctx->m_hangup_init_point = ServContex::INIT_HANGUP_FROM_NONE;
            }
            ctx->m_skype_call_conv = Conversation::Ref(0);
        }
    }

    return 0;
}

int skype_helper_invoke_participant_onincoming_dtmf(SERootObject *root, SEReference &participant, int dtmf){
    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    assert(root != NULL);
    CSkype *skype = static_cast<CSkype*>(root);
    ServContex *ctx = static_cast<ServContex*>(skype->ctx);
    assert(ctx != NULL);

    assert(participant);
    CParticipant::Ref conv = (static_cast<CParticipant*>(participant.operator->()))->ref();

    QString str_dtmf;

    switch (dtmf) {
    case Participant::DTMF_0: 
    case Participant::DTMF_1:
    case Participant::DTMF_2: 
    case Participant::DTMF_3: 
    case Participant::DTMF_4: 
    case Participant::DTMF_5:
    case Participant::DTMF_6: 
    case Participant::DTMF_7:
    case Participant::DTMF_8: 
    case Participant::DTMF_9:
        str_dtmf = QString::number(dtmf);
        break;
    case Participant::DTMF_STAR:
        str_dtmf = QString("*");
        break;
    case Participant::DTMF_POUND:
        str_dtmf = QString("#");
        break;
    default:
        Q_ASSERT(1== 2);
    };

    if (str_dtmf.length() > 0) {
        // do it
        QStringList strlist;
        strlist << str_dtmf;
        ctx->post_schedule_cmd(RPCMD::SKYPE_SEND_DTMF, ctx, strlist);
    }

    return 0;
}

