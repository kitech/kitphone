// cowserv.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-18 15:00:03 +0800
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

#include "ewebsocket.h"
#include "websocket_proc.h"
#include "kitserv.h"

#include "cowserv.h"

int cowserv_instance_main(int argc, char **argv, int way_id, int cfd)
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
    QString sess_id;

    ws_ctx_t *ws_ctx = NULL;
    ws_ctx = do_handshake(cfd);
    if (ws_ctx == NULL) {
        ::close(cfd);
        qDebug()<<"ws handshake faild, invalid client.";
        assert(1 == 2);
    } else {
        // path=/skype_id/password/  // dep
        // path=/sess_id/skype_id/
        char *p1, *p2;
        QString skype_name;
        p1 = strchr(ws_ctx->headers.path, '/');
        if (!p1) {
            ws_socket_free(ws_ctx);
            qDebug()<<"ws handshake faild, invalid client.";
        }
        p2 = strchr(p1 + 1, '/');
        if (!p2) {
            ws_socket_free(ws_ctx);
            qDebug()<<"ws handshake faild, invalid client.";
        }
        QStringList path_elts = QString(ws_ctx->headers.path).split("/");
        qDebug()<<path_elts; // ("", "drswinghead", "203", "")
        // skype_name = QString::fromAscii(p1+1, p2 - p1 -1);
        // switcher_skype_name = path_elts.at(1);
        // switcher_password = path_elts.at(2);
        sess_id = path_elts.at(1);
        switcher_skype_name = skype_name = path_elts.at(2);
        // ctx->m_websocket_client_fds.insert(ctx->m_ws_fd, skype_name);
        // ctx->m_websocket_client_ws_ctxes.insert(ctx->m_ws_fd, ws_ctx);
    }

    db = new Database();
    if (!db->connectdb()) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"connect database error";
        exit(-1);
    }

    srandom(time(NULL));
    // init sip contex
    // iret = init_sip_app_contex();
    // iret = check_pjsip_config_validation();

    // init global contex 
    memset(storage_dir, 0, sizeof(storage_dir));
    snprintf(storage_dir, sizeof(storage_dir),
             "%s/%s/ipc_keys/ipc_key/", getenv("HOME"), Configs().get_storage_path().toAscii().data());
    memset(storage_dir, 0, sizeof(storage_dir));
    snprintf(storage_dir, sizeof(storage_dir),
             "%s/%s", getenv("HOME"), Configs().get_storage_path().toAscii().data());
    // fprintf(stderr, "storage_dir: %s\n", storage_dir);

    for (int i = 0 ; i < way_count ; ++i) {
        // switcher_skype_name = switchers.at(way_id).first;
        // switcher_password = switchers.at(way_id).second;

        ctxes[way_id] = ctx = new ServContex(way_id, cfd, storage_dir, switcher_skype_name.toAscii().data());
        ctx->m_db = db;
        ctx->m_sess_id = sess_id;
        ctx->m_websocket_client_fds.insert(cfd, switcher_skype_name);
        ctx->m_websocket_client_ws_ctxes.insert(cfd, ws_ctx);

        // ordered function call
        // ctx->init_sxs_switcher();
        // ctx->init_pcmhost_thread();
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

int cowserv_main(int argc, char **argv)
{
#define WS_EPOLL_MAX_EVENTS 60

    int ret;
    unsigned short serv_port;
    QString serv_ipaddr;
    socklen_t slen;
    int websocket_epfd;
    int websocket_serv_fd;
    int nfds, n;
    int cfd;
    struct epoll_event ev, events[60];
    int idx = 0;

    websocket_epfd = epoll_create(20);
    if (websocket_epfd == -1) {
        perror("epoll_create");
    }
    
    struct sockaddr_in sock_addr;
    slen = sizeof(sock_addr);

    websocket_serv_fd = create_tcp_server(8070);
    qDebug()<<"serv fd:"<<websocket_serv_fd;

    // ws port;
    serv_port = get_tcp_server_port(websocket_serv_fd);
    // serv_ipaddr = ctx->serverIpAddr(0);
    // ctx->m_db->setForwardPort(QString(ctx->m_account_name), serv_port, serv_ipaddr);

    struct epoll_event ctl_evt;
    ctl_evt.events = EPOLLIN;
    ctl_evt.data.fd = websocket_serv_fd;
    ret = epoll_ctl(websocket_epfd, EPOLL_CTL_ADD, websocket_serv_fd, &ctl_evt);
    // qDebug()<<"add swich fd to epoll:"<<ctx->m_ctx_id<<ctx->m_websocket_epfd
    // <<" fd:"<<ctx->m_websocket_serv_fd<<" ret:"<<ret<<strerror(errno);

    // ws_ctx_t *ws_ctx = NULL;
    int pid;
    for (;;) {
        nfds = epoll_wait(websocket_epfd, events, WS_EPOLL_MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_pwait");
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].events & EPOLLIN) {
                if (events[n].data.fd == websocket_serv_fd) {
                    cfd = ::accept(websocket_serv_fd, (struct sockaddr*)&sock_addr, &slen);
                    qDebug()<<"accepted out client connection: "<<cfd
                            <<ntohs(sock_addr.sin_port);
                    // spawn child process
                    {
                        pid = fork();
                        if (pid  == -1) {
                            printf("fork error:\n");
                            exit(99);
                        } else if (pid == 0) {
                            // child process
                            printf("child: get myself pid: %d, argc=%d, seqno=%d\n", 
                                   getpid(), argc, 1);
                            pid = cowserv_instance_main(argc, argv, idx, cfd);
                            sleep(10);
                            printf("child %d: before exit.\n", getpid());
                            exit(1);
                        } else {
                            // pids[idx] = pid;
			    idx ++;
                            printf("got child pid: %d\n", pid);
                        }
                    }
                } else {
                }
            }
            if (events[n].events & EPOLLOUT) {
                // if (events[n].data.fd == param->peer_fd) {
                //     // process_output_command(param->peer_fd);
                //     fprintf(stderr, "processed out data command done\n");
                // } else {
                //     // do_use_fd(events[n].data.fd);
                // }
            }
        }
    }
    
    

    return 0;
}

