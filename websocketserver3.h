// websocketserver3.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-16 06:41:48 +0000
// Version: $Id$
// 

#ifndef _WEBSOCKETSERVER3_H_
#define _WEBSOCKETSERVER3_H_

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

#ifdef _WIN32
// #define size_t unsigned int
#define ssize_t unsigned int
#endif


class WebSocketServer3
{
public:
    /* this protocol server (always the first one) just knows how to do HTTP */
    static
    int websocket_service_http(struct libwebsocket_context * context,
                               struct libwebsocket *wsi,
                               enum libwebsocket_callback_reasons reason, void *user,
                               void *in, size_t len);

    static 
    int websocket_service_vopp(struct libwebsocket_context * context,
                               struct libwebsocket *wsi,
                               enum libwebsocket_callback_reasons reason,
                               void *user, void *in, size_t len);
    // manually support max message than 127 bytes
    struct wssess_data_vopp {
        struct libwebsocket *wsi;
        int seq;
        int ringbuffer_tail;
        char full_message[5120];
        PackageCombiner *pc;
    };

private:
    explicit WebSocketServer3();
    static WebSocketServer3 *m_inst;
public:
    static WebSocketServer3 *instance();
    virtual ~WebSocketServer3();

    bool start();
    bool stop();
    bool restart();
    unsigned short serverPort();
    void run();
    bool isRunning();

    int nextPendingConnection();

    // ssize_t recv(int cseq, void *buf, size_t len);
    // ssize_t send(int cseq, const void *buf, size_t len);
    ssize_t wssend(int cseq, const void *buf, size_t len); // with frame code
    ssize_t wssend(int cseq, const std::string &msg);

    std::string conn_payload_path(int cseq);
    std::string conn_get_peer_address(int cseq);
    int conn_close(int cseq);

    // cb by lws
    int lws_new_connection_established(libwebsocket *wsi);
    int lws_connection_closed(libwebsocket *wsi);
    int lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len);

public:
    libwebsocket_context *serv_ctx;
    bool quit_srv_loop;
    int outer_conn_seq;

    boost::bimap<libwebsocket*, std::string> pending_conns; // before handshake done
    boost::bimap<libwebsocket*, std::string> pending_payload_paths; // before handshake done
    boost::bimap<libwebsocket*, int> outer_conns; // ws -> conn seq

    boost::thread wst;
    boost::thread::id wst_id;

    enum ErrorType {
        ET_MIN = 0,
        ET_CTX,
        ET_MAX,
    };
    boost::signals2::signal<void (int)> error;
    boost::signals2::signal<void(void)> started;
    boost::signals2::signal<void(void)> finished;
    boost::signals2::signal<void()> new_connection;
    boost::signals2::signal<void(const std::string&, int)> new_message;
    boost::signals2::signal<void(int)> connection_closed;

protected:
    bool m_is_running;
    enum WSTRunState {
        RS_MIN = 0,
        RS_STOPPED,
        RS_STOPPING,
        RS_STARTING,
        RS_STARTED,
        RS_MAX
    };
    int m_run_state;
};


#endif /* _WEBSOCKETSERVER3_H_ */
