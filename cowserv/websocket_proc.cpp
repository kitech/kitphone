// websocket_proc.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-09 15:11:39 +0800
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

#include "../skyserv/database.h"
#include "kitserv.h"

#include "ewebsocket.h"
#include "websocket_proc.h"

int set_sock_nonblock (int sock)
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

    return 0;
}

int set_sock_block(int sock)
{
    unsigned long flags = 0;
#ifdef WIN32

#else
    flags = fcntl(sock, F_GETFL, NULL);
    // assert(flags >= 0);
    flags |= (~O_NONBLOCK);
    return (fcntl(sock, F_SETFL, flags) >=0);
#endif
    return 1;
}

int create_tcp_server(unsigned short port)
{
    // socket, bind, listen, accept
    int ret = 0;
    int sock = 0;
    struct sockaddr_in serv_addr;

    sock = ::socket(AF_INET, SOCK_STREAM, 0);

    ret = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int*)&ret, sizeof(ret)); 

    inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr.s_addr);
    // inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
    // serv_addr.sin_port = 0;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;
    ret = ::bind(sock, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr));
    ret = ::listen(sock, 10);
    assert(ret == 0);
    printf("set sock nonblock erro: %d, %s\n", errno, strerror(errno));

    // where to accept
    socklen_t addr_size = sizeof(serv_addr);
    ::getsockname(sock, (struct sockaddr*)&serv_addr, &addr_size);
 
    ret = set_sock_nonblock(sock);
    if (ret != 0) {
        printf("set sock nonblock erro: %d, %s\n", errno, strerror(errno));
    }
    // assert(ret == 0);

    return sock;
}

unsigned short get_tcp_server_port(int sock)
{
    unsigned short port = 0;
    struct sockaddr_in serv_addr;

    // where to accept
    socklen_t addr_size = sizeof(serv_addr);
    ::getsockname(sock, (struct sockaddr*)&serv_addr, &addr_size);

    port = ntohs(serv_addr.sin_port);

    return port;
}

int read_ws_message(ServContex *ctx, int fd)
{
    unsigned char rbuf[1000] = {0};
    int rlen = 0, irv;
    ws_ctx_t *ws_ctx = NULL;
    QString ws_msg;

    rlen = ::read(fd, rbuf, sizeof(rbuf));

    if (rlen < 0) {
        // why 
    } else if (rlen == 0) {
        irv = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_DEL, fd, 0);
        if (irv != 0) {
            assert(irv == 0);
        }
        // socket close
        qDebug()<<"ws client close:"<<ctx->m_websocket_client_fds.findLeft(fd).value();
        ctx->m_websocket_client_fds.removeLeft(fd);
        if (ctx->m_websocket_client_ws_ctxes.leftContains(fd)) {
            ws_ctx = (ws_ctx_t*)ctx->m_websocket_client_ws_ctxes.findLeft(fd).value();
            ctx->m_websocket_client_ws_ctxes.removeLeft(fd);
            if (ws_ctx) {
                ws_socket_free(ws_ctx);
            }
        }
    } else {
        //always 00datahereff format ?????????
        if (rbuf[0] == 0x00 && rbuf[rlen-1] == 0xff) {
            ws_msg = QByteArray((const char*)(rbuf + 1), rlen - 2);
	    qDebug()<<"ws msg:"<<ws_msg;
            // websocket close standard command 0xff0x00
        } else if(rlen == 2 && rbuf[0] == 0xff && rbuf[1] == 0x00) {
            qDebug()<<"websocket recieve close request.";
            irv = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_DEL, fd, 0);
            if (irv != 0) {
                assert(irv == 0);
            }
            // socket close
            qDebug()<<"ws client close:"<<ctx->m_websocket_client_fds.findLeft(fd).value();
            ctx->m_websocket_client_fds.removeLeft(fd);
            if (ctx->m_websocket_client_ws_ctxes.leftContains(fd)) {
                ws_ctx = (ws_ctx_t*)ctx->m_websocket_client_ws_ctxes.findLeft(fd).value();
                ctx->m_websocket_client_ws_ctxes.removeLeft(fd);
                if (ws_ctx) {
                    ws_socket_free(ws_ctx);
                }
            }
        } else {
            qDebug()<<"Invalid/Uknown ws data frame, omited";
        }
    }

    return rlen;
}


int websocket_send_data(int fd, char *data, int len)
{
    ssize_t wlen = 0, wlen1 = 0;
    bool ok;
    char fixchars[8];

    Q_ASSERT(data != NULL);

    fixchars[0] = 0x00;
    wlen1 = ::write(fd, fixchars, 1);
    Q_ASSERT(wlen1 == 1);
    wlen = ::write(fd, data, len);
    fixchars[0] = 0xff;
    wlen1 = ::write(fd, fixchars, 1);
    Q_ASSERT(wlen1 == 1);
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<data;
    return wlen;
}

void *websocket_server_proc(void *args)
{
#define WS_EPOLL_MAX_EVENTS 60

    ServContex *ctx = (ServContex*)args;

    ctx->m_websocket_thread_id = syscall(__NR_gettid);

    fprintf(stdout, "Entry websocket thread loop, ctx id: %d, thread id: %d\n",
            ctx->m_ctx_id, ctx->m_websocket_thread_id);
    
    ctx->m_websocket_client_fds.insert(1, QString("abcd"));

    int ret;
    unsigned short serv_port;
    QString serv_ipaddr;
    socklen_t slen;
    int nfds, n;
    int cfd;
    struct epoll_event ev, events[60];

    ctx->m_websocket_epfd = epoll_create(20);
    if (ctx->m_websocket_epfd == -1) {
        perror("epoll_create");
        assert(ctx->m_websocket_epfd != -1);
    }
    
    struct sockaddr_in sock_addr;
    slen = sizeof(sock_addr);

    ctx->m_websocket_serv_fd = create_tcp_server();

    // ws port;
    serv_port = get_tcp_server_port(ctx->m_websocket_serv_fd);
    serv_ipaddr = ctx->serverIpAddr(0);
    // ctx->m_db->setForwardPort(QString(ctx->m_account_name), serv_port, serv_ipaddr);

    struct epoll_event ctl_evt;
    ctl_evt.events = EPOLLIN;
    ctl_evt.data.fd = ctx->m_websocket_serv_fd;
    ret = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_ADD, ctx->m_websocket_serv_fd, &ctl_evt);
    qDebug()<<"add swich fd to epoll:"<<ctx->m_ctx_id<<ctx->m_websocket_epfd
            <<" fd:"<<ctx->m_websocket_serv_fd<<" ret:"<<ret<<strerror(errno);

    if (ctx->m_ws_fd > 0) {
        ctl_evt.events = EPOLLIN;
        ctl_evt.data.fd = ctx->m_ws_fd;
        ret = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_ADD, ctx->m_ws_fd, &ctl_evt);
    }

    ws_ctx_t *ws_ctx = NULL;

    for (;ctx->m_websocket_thread_quit_loop == 0;) {
        nfds = epoll_wait(ctx->m_websocket_epfd, events, WS_EPOLL_MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_pwait");
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].events & EPOLLIN) {
                if (events[n].data.fd == ctx->m_websocket_serv_fd) {
                    cfd = ::accept(ctx->m_websocket_serv_fd, (struct sockaddr*)&sock_addr, &slen);
                    qDebug()<<"accepted out client connection: "<<cfd
                            <<ntohs(sock_addr.sin_port);
                    ws_ctx = do_handshake(cfd);
                    if (ws_ctx == NULL) {
                        ::close(cfd);
                        qDebug()<<"ws handshake faild, invalid client.";
                    } else {
                        // path=/skype_id/
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
                        skype_name = QString::fromAscii(p1+1, p2 - p1 -1);
                        ctx->m_websocket_client_fds.insert(cfd, skype_name);
                        ctx->m_websocket_client_ws_ctxes.insert(cfd, ws_ctx);

                        ev.data.fd = cfd;
                        ev.events = EPOLLIN;
                        if ((ret = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_ADD, cfd, &ev)) != 0) {
                            perror("epoll add out cli read event");
                            ctx->m_websocket_client_fds.removeLeft(cfd);
                            ctx->m_websocket_client_ws_ctxes.removeLeft(cfd);
                            ws_socket_free(ws_ctx);
                        }
                    }
                } else if (ctx->m_websocket_client_fds.leftContains(events[n].data.fd)) {
                    // client data ready
                    read_ws_message(ctx, events[n].data.fd);
                } else if (events[n].data.fd == ctx->m_ws_fd) {
                    assert(ctx->m_websocket_client_fds.leftContains(events[n].data.fd));
                    // already handshake 
		    read_ws_message(ctx, events[n].data.fd);
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

    return NULL;
}

void *websocket_bus_proc(void *args)
{
#define WS_EPOLL_MAX_EVENTS 60

    ServContex *ctx = (ServContex*)args;

    ctx->m_websocket_thread_id = syscall(__NR_gettid);

    fprintf(stdout, "Entry websocket thread loop, ctx id: %d, thread id: %d\n",
            ctx->m_ctx_id, ctx->m_websocket_thread_id);
    
    ctx->m_websocket_client_fds.insert(1, QString("abcd"));

    int ret;
    unsigned short serv_port;
    QString serv_ipaddr;
    socklen_t slen;
    int nfds, n;
    int cfd;
    struct epoll_event ev, events[60];

    ctx->m_websocket_epfd = epoll_create(20);
    if (ctx->m_websocket_epfd == -1) {
        perror("epoll_create");
        assert(ctx->m_websocket_epfd != -1);
    }
    
    struct sockaddr_in sock_addr;
    slen = sizeof(sock_addr);

    // ctx->m_websocket_serv_fd = create_tcp_server();

    // ws port;
    // serv_port = get_tcp_server_port(ctx->m_websocket_serv_fd);
    // serv_ipaddr = ctx->serverIpAddr(0);
    // ctx->m_db->setForwardPort(QString(ctx->m_account_name), serv_port, serv_ipaddr);

    struct epoll_event ctl_evt;
    ctl_evt.events = EPOLLIN;
    ctl_evt.data.fd = ctx->m_websocket_serv_fd;
    ret = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_ADD, ctx->m_websocket_serv_fd, &ctl_evt);
    qDebug()<<"add swich fd to epoll:"<<ctx->m_ctx_id<<ctx->m_websocket_epfd
            <<" fd:"<<ctx->m_websocket_serv_fd<<" ret:"<<ret<<strerror(errno);


    ws_ctx_t *ws_ctx = NULL;

    for (;ctx->m_websocket_thread_quit_loop == 0;) {
        nfds = epoll_wait(ctx->m_websocket_epfd, events, WS_EPOLL_MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_pwait");
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].events & EPOLLIN) {
                if (events[n].data.fd == ctx->m_websocket_serv_fd) {
                    cfd = ::accept(ctx->m_websocket_serv_fd, (struct sockaddr*)&sock_addr, &slen);
                    qDebug()<<"accepted out client connection: "<<cfd
                            <<ntohs(sock_addr.sin_port);
                    ws_ctx = do_handshake(cfd);
                    if (ws_ctx == NULL) {
                        ::close(cfd);
                        qDebug()<<"ws handshake faild, invalid client.";
                    } else {
                        // path=/skype_id/
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
                        skype_name = QString::fromAscii(p1+1, p2 - p1 -1);
                        ctx->m_websocket_client_fds.insert(cfd, skype_name);
                        ctx->m_websocket_client_ws_ctxes.insert(cfd, ws_ctx);

                        ev.data.fd = cfd;
                        ev.events = EPOLLIN;
                        if ((ret = epoll_ctl(ctx->m_websocket_epfd, EPOLL_CTL_ADD, cfd, &ev)) != 0) {
                            perror("epoll add out cli read event");
                            ctx->m_websocket_client_fds.removeLeft(cfd);
                            ctx->m_websocket_client_ws_ctxes.removeLeft(cfd);
                            ws_socket_free(ws_ctx);
                        }
                    }
                } else if (ctx->m_websocket_client_fds.leftContains(events[n].data.fd)) {
                    // client data ready
                    read_ws_message(ctx, events[n].data.fd);
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

    return NULL;
}

