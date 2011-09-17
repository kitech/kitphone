// websocketserver3.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-16 06:42:56 +0000
// Version: $Id$
// 

#include "simplelog.h"
#include "intermessage.h"

#include "websocketserver3.h"

/* list of supported protocols and callbacks */
static struct libwebsocket_protocols vopp_protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only",		/* name */
        // callback_http,		/* callback */
        &WebSocketServer3::websocket_service_http,
        0			/* per_session_data_size */
    },
    {
        "vopp",
        // callback_wso,
        &WebSocketServer3::websocket_service_vopp,
        // sizeof(struct per_session_data__wso)
        sizeof(struct WebSocketServer3::wssess_data_vopp)
    },
    {
        NULL, NULL, 0		/* End of list */
    }
};

static struct libwebsocket_extension libwebsocket_vopp_extensions[] = {
	{ /* terminator */
		NULL, NULL, 0
	}
};

////// protocol 
#define LOCAL_RESOURCE_PATH "."
// #define MAX_MESSAGE_QUEUE 128
// struct a_message {
// 	void *payload;
// 	size_t len;
// };

// static struct a_message ringbuffer[MAX_MESSAGE_QUEUE];
// static int ringbuffer_head;
// static struct a_message ringbuffer_wso[MAX_MESSAGE_QUEUE];
// static int ringbuffer_head_wso = 0;
// static int close_testing_wso = 0;

// static
int WebSocketServer3::websocket_service_http(struct libwebsocket_context *context,
                                             struct libwebsocket *wsi,
                                             enum libwebsocket_callback_reasons reason, void *user,
                                             void *in, size_t len)
{
	char client_name[128];
	char client_ip[128];

	switch (reason) {
	case LWS_CALLBACK_HTTP:
		fprintf(stderr, "serving HTTP URI %s\n", (char *)in);

		if (in && strcmp((char*)in, "/favicon.ico") == 0) {
			if (libwebsockets_serve_http_file(wsi,
			        LOCAL_RESOURCE_PATH"/favicon.ico", "image/x-icon"))
				fprintf(stderr, "Failed to send favicon\n");
			break;
		}

		/* send the script... when it runs it'll start websockets */

		if (libwebsockets_serve_http_file(wsi,
				  LOCAL_RESOURCE_PATH"/test.html", "text/html"))
			fprintf(stderr, "Failed to send HTTP file\n");
		break;

	/*
	 * callback for confirming to continue with client IP appear in
	 * protocol 0 callback since no websocket protocol has been agreed
	 * yet.  You can just ignore this if you won't filter on client IP
	 * since the default uhandled callback return is 0 meaning let the
	 * connection continue.
	 */

	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:

		libwebsockets_get_peer_addresses((int)(long)user, client_name,
			     sizeof(client_name), client_ip, sizeof(client_ip));

		fprintf(stderr, "Received network connect from %s (%s)\n",
							client_name, client_ip);

		/* if we returned non-zero from here, we kill the connection */
		break;

	default:
		break;
	}

    return 0;
}

// static
int WebSocketServer3::websocket_service_vopp(struct libwebsocket_context *context,
                                             struct libwebsocket *wsi,
                                             enum libwebsocket_callback_reasons reason,
                                             void *user, void *in, size_t len)
{
    int n;
    int k;
    std::string str1,str2;
    struct WebSocketServer3::wssess_data_vopp *pwso = (struct WebSocketServer3::wssess_data_vopp*)user;
    WebSocketServer3 *user_hook = WebSocketServer3::instance();

    switch (reason) {
    case LWS_CALLBACK_ESTABLISHED:
        pwso->seq = 0;
        pwso->wsi = wsi;
        pwso->pc = new PackageCombiner;

        user_hook->lws_new_connection_established(wsi);

		// pwso->ringbuffer_tail = ringbuffer_head_wso;
        break;

    case LWS_CALLBACK_CLOSED:
        delete pwso->pc; pwso->pc = NULL;
        user_hook->lws_connection_closed(wsi);
        break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
        qLogx()<<"";
		// if (pwso->ringbuffer_tail != ringbuffer_head_wso) {

		// 	n = libwebsocket_write(wsi, (unsigned char *)
		// 		   ringbuffer_wso[pwso->ringbuffer_tail].payload +
		// 		   LWS_SEND_BUFFER_PRE_PADDING,
		// 		   ringbuffer_wso[pwso->ringbuffer_tail].len,
		// 						LWS_WRITE_TEXT);
		// 	if (n < 0) {
		// 		fprintf(stderr, "ERROR writing to socket");
		// 		exit(1);
		// 	}

		// 	if (pwso->ringbuffer_tail == (MAX_MESSAGE_QUEUE - 1))
		// 		pwso->ringbuffer_tail = 0;
		// 	else
		// 		pwso->ringbuffer_tail++;

		// 	if (((ringbuffer_head - pwso->ringbuffer_tail) %
		// 		  MAX_MESSAGE_QUEUE) < (MAX_MESSAGE_QUEUE - 15))
		// 		libwebsocket_rx_flow_control(wsi, 1);

		// 	libwebsocket_callback_on_writable(context, wsi);

		// }
        // libwebsocket_callback_on_writable(context, wsi);
		break;

    case LWS_CALLBACK_RECEIVE:
        n = libwebsockets_remaining_packet_payload(wsi);
        k = libwebsocket_is_final_fragment(wsi);
        qLogx()<<"remaining: "<<n<<len<<k;
        // n = user_hook->lws_ws_message_ready(wsi, (char*)in, len);
        str1 = std::string((char*)in, len);
        pwso->pc->save_fragment(str1);
        if (pwso->pc->is_package_finish(str1)) {
            str2 = pwso->pc->get_full_package(str1);
            strncpy(pwso->full_message, str2.c_str(), sizeof(pwso->full_message)-1);
            n = user_hook->lws_ws_message_ready(wsi, pwso->full_message, str2.length());
        }

		// if (ringbuffer_wso[ringbuffer_head_wso].payload)
		// 	free(ringbuffer_wso[ringbuffer_head_wso].payload);

		// ringbuffer_wso[ringbuffer_head_wso].payload =
		// 		malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
		// 				  LWS_SEND_BUFFER_POST_PADDING);
		// ringbuffer_wso[ringbuffer_head_wso].len = len;
		// memcpy((char *)ringbuffer_wso[ringbuffer_head_wso].payload +
		// 			  LWS_SEND_BUFFER_PRE_PADDING, in, len);
		// if (ringbuffer_head_wso == (MAX_MESSAGE_QUEUE - 1))
		// 	ringbuffer_head_wso = 0;
		// else
		// 	ringbuffer_head_wso++;

		// if (((ringbuffer_head_wso - pwso->ringbuffer_tail) %
		// 		  MAX_MESSAGE_QUEUE) > (MAX_MESSAGE_QUEUE - 10))
		// 	libwebsocket_rx_flow_control(wsi, 0);

        // 这一语句导致接收到的数据被马上又发送回去，像echo服务器一样了。
        // 不需要这样的功能，纯属无意导致的bug。
		// libwebsocket_callback_on_writable_all_protocol(
		// 			       libwebsockets_get_protocol(wsi));

        break;
    case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
        break;
    default:
        qLogx()<<"Unhandled lws callback event:"<<reason;
        break;
    }

    return 0;
}

WebSocketServer3 *WebSocketServer3::m_inst = NULL;
WebSocketServer3::WebSocketServer3()
{
    this->outer_conn_seq = 0;
    this->m_is_running = false;
    this->m_run_state = RS_STOPPED;
}
WebSocketServer3::~WebSocketServer3()
{
}

WebSocketServer3 *WebSocketServer3::instance()
{
    WebSocketServer3 *inst = NULL;
    if (WebSocketServer3::m_inst == NULL) {
        inst = WebSocketServer3::m_inst = new WebSocketServer3();
    } else {
        inst = WebSocketServer3::m_inst;
    }

    return inst;
}

bool WebSocketServer3::start()
{
    this->m_run_state = RS_STARTING;

    // why not move this block mode code to it's thread ???
    // port = 18080;
    const char *hostif = "127.0.0.1"; // "eth0"; "lo";  // "127.0.0.1"; means interface name, not ipaddress
    unsigned short port = 18080;
    // TODO dynamic cert_path resolv
    const char *cert_path =
        // LOCAL_RESOURCE_PATH"/libwebsockets-test-server.pem";
        "./seckey/cacert.pem";
    const char *key_path =
        // LOCAL_RESOURCE_PATH"/libwebsockets-test-server.key.pem";
        "./seckey/privkey.pem";

    qLogx()<<"want to listen:"<<port;
    // this->serv_ctx = libwebsocket_create_context(port, NULL, protocols, NULL, NULL, NULL, -1, -1, 0);
    // current: Compiled with SSL support, not using it
    // this->serv_ctx = libwebsocket_create_context_ex(port, hostif, vopp_protocols,
    //                                              libwebsocket_vopp_extensions, NULL, NULL, -1, -1, 0, this);
    this->serv_ctx = libwebsocket_create_context_ex(port, hostif, vopp_protocols,
                                                 libwebsocket_vopp_extensions, cert_path, key_path, -1, -1, 0, this);
    if (this->serv_ctx == NULL) {
        this->error(ET_CTX);
        assert(this->serv_ctx != NULL);
    }

    // qlog("server listen port: %d\n", this->serverPort()); 
    qLogx()<<"server listen port:"<<this->serverPort();

    // this->wst = boost::thread(WebSocketServer3::run);
    this->wst = boost::thread(boost::bind(&WebSocketServer3::run, this));
    this->m_run_state = RS_STARTED;
    this->started();

    return true;
}

void WebSocketServer3::run()
{
    WebSocketServer3 *serv = this; // WebSocketServer3::instance();
    
    int iret = 0;

    qLogx()<<"boost thread:";
    
    while (!serv->quit_srv_loop) {
        iret = libwebsocket_service(serv->serv_ctx, 2000);
        // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
        if (iret == 0) {
            // ok, success
        } else {
            // listen socket deak
            qLogx()<<"Listen socket maybe dead."
                   <<"Need restart this server.";
        }
    }

    this->m_run_state = RS_STOPPED;
    this->finished();
}

bool WebSocketServer3::isRunning()
{
    return (this->m_run_state == RS_STARTED || this->m_run_state == RS_STARTING);
}

int WebSocketServer3::nextPendingConnection()
{
    libwebsocket *backend = NULL;
    int cseq = 0;
    std::string mvstr;
    char *ptr = NULL;

    if (this->pending_conns.size() > 0) {
        backend = this->pending_conns.left.begin()->first;
        mvstr = this->pending_conns.left.begin()->second;
        this->pending_conns.left.erase(backend);

        char tbuf[60+1] = {0};
        strncpy(tbuf, mvstr.c_str(), sizeof(tbuf)-1);

        ptr = strchr(tbuf, '\t');
        assert(ptr != NULL);
        *ptr = '\0';
        
        cseq = strtol(tbuf, &ptr, 10);
        // cseq = ++this->outer_conn_seq;
        this->outer_conns.left.insert(boost::bimap<libwebsocket*, int>::left_value_type(backend, cseq));
    }

    return cseq;
    
    return 0;
}
unsigned short WebSocketServer3::serverPort()
{
    unsigned short port = 0;

    port = libwebsocket_context_server_port(this->serv_ctx);

	// int n;
	// int m;
	// struct libwebsocket *wsi;
    // struct sockaddr_in serv_addr;
    // socklen_t addr_size;

    // qLogx()<<FD_HASHTABLE_MODULUS;
	// for (n = 0; n < FD_HASHTABLE_MODULUS; n++) {
	// 	for (m = 0; m < this->serv_ctx->fd_hashtable[n].length; m++) {
	// 		wsi = this->serv_ctx->fd_hashtable[n].wsi[m];


    //         addr_size = sizeof(serv_addr);
    //         getsockname(wsi->sock, (struct sockaddr*)&serv_addr, &addr_size);
    //         port = ntohs(serv_addr.sin_port);

    //         qLogx()<<n<<m<<wsi<<wsi->sock<<wsi->mode<<port;

    //         if (wsi->mode == LWS_CONNMODE_SERVER_LISTENER) {
    //            break;
    //         } else {
    //             port = 0;
    //         }
    //         port = 0;
    //     }
    //     if (port != 0) {
    //         break;
    //     }
    // }
    // assert(port != 0);

    // 减少对基础库的改动
    // port = libwebsocket_server_get_port(this->serv_ctx);

    return port;
    return 0;
}

// ssize_t WebSocketServer3::recv(int cseq, void *buf, size_t len)
// {

//     return 0;
// }

// ssize_t WebSocketServer3::send(int cseq, const void *buf, size_t len)
// {
//     // ssize_t wlen = 0;
//     // libwebsocket *wsi = this->outer_conns.findRight(cseq).value();
//     // // Q_ASSERT(sock != NULL);
//     // Q_ASSERT(cseq != 0);
//     // // wlen = sock->write((const char *)buf, len);

//     // if (!this->outer_conns.rightContains(cseq)) {
//     //     qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"maybe sess ended already.";        
//     // } else {
//     //     wlen = libwebsocket_write(wsi, (unsigned char *)buf, len, LWS_WRITE_TEXT);
//     //     qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<(const char*)buf;
//     // }

//     // return wlen;

//     return 0;
// }

ssize_t WebSocketServer3::wssend(int cseq, const void *buf, size_t len) // with frame code
{
    std::string msg;

    msg = std::string((char*)buf, len);
    return this->wssend(cseq, msg);

    // old way 
    ssize_t wlen = 0;
    bool ok;
    unsigned char wbuf[512] = {0};
    libwebsocket *wsi = NULL;

    if (this->outer_conns.right.find(cseq) != this->outer_conns.right.end()) {
        // this->outer_conns.findRight(cseq).value();
        wsi = this->outer_conns.right.find(cseq)->second;
    }

    assert(cseq != 0);

    /*
      buf 
      The data to send. For data being sent on a websocket connection (ie, not default http), this buffer MUST have LWS_SEND_BUFFER_PRE_PADDING bytes valid BEFORE the pointer and an additional LWS_SEND_BUFFER_POST_PADDING bytes valid in the buffer after (buf + len). This is so the protocol header and trailer data can be added in-situ.

      这应该就是问题多在了。
     */
    assert(strlen((const char*)buf) == len); // 因为传递过来的是可打印字符串。
    if (wsi == NULL) {
        qLogx()<<"maybe sess ended already."<<cseq;
    } else {
        assert(sizeof(wbuf) > (len + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING));
        memcpy(wbuf + LWS_SEND_BUFFER_PRE_PADDING, buf, len);
        wlen = libwebsocket_write(wsi, (unsigned char*)&wbuf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
        if (wlen == 0) {
            // write success
        } else {
            // write error
        }
        // QString ba = QByteArray((const char*)buf, len);
        qLogx()<<"wlen:"<<wlen<<(const char*)buf<<(const char*)(wbuf + LWS_SEND_BUFFER_PRE_PADDING);
        libwebsocket_callback_on_writable(this->serv_ctx, wsi);
    }
    return wlen;

    return 0;
}


ssize_t WebSocketServer3::wssend(int cseq, const std::string &msg)
{
    ssize_t ret = 0;
    ssize_t tret = 0;
    char tbuf[512] = {0};
    std::string frag;

    // memcpy(wbuf, msg.c_str(), msg.length());
    // assert(strlen(wbuf) == msg.length());
    // ret = this->wssend(cseq, wbuf, msg.length());

    libwebsocket *wsi = NULL;

    if (this->outer_conns.right.find(cseq) != this->outer_conns.right.end()) {
        // this->outer_conns.findRight(cseq).value();
        wsi = this->outer_conns.right.find(cseq)->second;
    }

    assert(cseq != 0);

    if (wsi == NULL) {
        qLogx()<<"maybe sess ended already."<<cseq;
    } else {
        PackageSplitter ps(msg);

        while (ps.hasNextFragment()) {
            frag = ps.nextFragment();

            memset(tbuf, 0, sizeof(tbuf));
            strcpy(&tbuf[LWS_SEND_BUFFER_PRE_PADDING], frag.c_str());
        
            ret = libwebsocket_write(wsi, (unsigned char*)&tbuf[LWS_SEND_BUFFER_PRE_PADDING],
                                     frag.length(), LWS_WRITE_TEXT);
            if (ret  == 0) {
                // write success
            } else {
                // write error
            }
        }
    }
    return ret;
    return 0;
}

// cb by lws
int WebSocketServer3::lws_new_connection_established(libwebsocket *wsi)
{
    std::string path_str, mvstr;
    // struct lws_tokens *t;
    // for (int i = 0 ; i < WSI_TOKEN_COUNT; i++) {
    //     t = &wsi->utf8_token[i];
    //     if (t->token_len == 0) {
    //         break;
    //     }
    //     fprintf(stderr, "dumping hdr: %s, %d\n", t->token, t->token_len);
    //     if (i == WSI_TOKEN_GET_URI) {
    //         // should be c_path token
    //         path_str = std::string(t->token, t->token_len);
    //         // this->pending_payload_paths.insert(wsi, QByteArray(t->token, t->token_len));
    //     }
    // }
    // this->pending_conns.insert(wsi, QByteArray());
    // emit newConnection();

    int tseq = ++this->outer_conn_seq;
    qLogx()<<wsi<<this->outer_conns.size()<<this->pending_conns.size()<<this->pending_payload_paths.size();
    path_str = libwebsocket_get_request_path(wsi);
    char tbuf[60 + 16];
    memset(tbuf, 0, sizeof(tbuf));
    snprintf(tbuf, sizeof(tbuf), "%d\t%s", tseq, path_str.c_str());
    mvstr = std::string(tbuf);
    this->pending_payload_paths.left.insert(boost::bimap<libwebsocket*, std::string>::left_value_type(wsi, mvstr));
    this->pending_conns.left.insert(boost::bimap<libwebsocket*, std::string>::left_value_type(wsi, mvstr));
    qLogx()<<wsi<<this->outer_conns.size()<<this->pending_conns.size()<<this->pending_payload_paths.size();

    this->new_connection();

    qLogx()<<path_str.c_str();

    return 0;
}

int WebSocketServer3::lws_connection_closed(libwebsocket *wsi)
{
    qLogx()<<"backened ws close event:"<<wsi;
    int cseq = 0;
    // if (this->outer_conns.leftContains(wsi)) {
    //     cseq = this->outer_conns.findLeft(wsi).value();
    //     this->outer_conns.removeLeft(wsi);
    //     emit this->clientSessionClosed(cseq);
    // }
    qLogx()<<this->outer_conns.size()<<this->pending_conns.size()<<this->pending_payload_paths.size();
    if (this->outer_conns.left.find(wsi) != this->outer_conns.left.end()) {
        cseq = this->outer_conns.left.find(wsi)->second;
        this->outer_conns.left.erase(wsi);
        this->connection_closed(cseq);
    }
    qLogx()<<this->outer_conns.size()<<this->pending_conns.size()<<this->pending_payload_paths.size();
    // 如果还没有没有处理的连接，需要在这清理。
    if (this->pending_conns.left.find(wsi) != this->pending_conns.left.end()) {
        this->pending_conns.left.erase(wsi);
    }
    qLogx()<<this->outer_conns.size()<<this->pending_conns.size()<<this->pending_payload_paths.size();
    if (this->pending_payload_paths.left.find(wsi) != this->pending_payload_paths.left.end()) {
        this->pending_payload_paths.left.erase(wsi);        
    }
    qLogx()<<this->outer_conns.size()<<this->pending_conns.size()<<this->pending_payload_paths.size();
    return 0;
}

int WebSocketServer3::lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len)
{
    int cseq = 0;
    // QByteArray nmsg = QByteArray(msg, len);
    std::string nmsg = std::string(msg, len);

    // cseq = this->outer_conns.findLeft(wsi).value();
    if (this->outer_conns.left.find(wsi) != this->outer_conns.left.end()) {
        cseq = this->outer_conns.left.find(wsi)->second;
    }

    // only for websocket-v00
    char ping_frm[32] = {0};
    unsigned char c;

    if (0) {
        for (int i = 0; i < len; ++i) {
            c = msg[i];
            fprintf(stderr, "%02X ", c);
        }
        fprintf(stderr, "\n");
    }

    if (memcmp(msg, ping_frm, 7) == 0
        && len == 11) {
        // here head 00 and tail FF already been dropped
        // 00 00 00 00 00 00 00 00 68 00 00 00 FF
        qLogx()<<"Recive clint ping frame. B"<<len;
    } else {
        qLogx()<<"lws serv got and broard msg:"<<cseq<<len<<nmsg.c_str();
        this->new_message(nmsg, cseq);
        // emit this->newWSMessage(nmsg, cseq);
    }

    return 0;
}

std::string WebSocketServer3::conn_payload_path(int cseq)
{
    std::string path;
    libwebsocket *wsi = NULL;

    if (this->outer_conns.right.find(cseq) != this->outer_conns.right.end()) {
        wsi = this->outer_conns.right.find(cseq)->second;
        if (this->pending_payload_paths.left.find(wsi) != this->pending_payload_paths.left.end()) {
            path = this->pending_payload_paths.left.find(wsi)->second;
            this->pending_payload_paths.left.erase(wsi);
        }
        // path = this->pending_payload_paths.value(wsi);
        // this->pending_payload_paths.remove(wsi);
    }

    return path;
}

std::string WebSocketServer3::conn_get_peer_address(int cseq)
{
    std::string addr;
    libwebsocket *wsi = NULL;
    
    if (this->outer_conns.right.find(cseq) != this->outer_conns.right.end()) {
        wsi = this->outer_conns.right.find(cseq)->second;
    }

    // char buf[100] = {0};
    // struct sockaddr_in serv_addr;
    // socklen_t addr_size;

    if (wsi) {
        // addr_size = sizeof(serv_addr);
        // ::getsockname(wsi->sock, (struct sockaddr*)&serv_addr, &addr_size);
        // const char *pret = ::inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, buf, sizeof(buf)-1);
        addr = libwebsocket_get_peer_address(wsi);
    }
    qLogx()<<"get addr from lws wsi conn: "<<addr.c_str();

    return addr;
}

int WebSocketServer3::conn_close(int cseq)
{
    int iret = 0;
    libwebsocket *wsi = NULL;

    if (this->outer_conns.right.find(cseq) != this->outer_conns.right.end()) {
        // this->outer_conns.findRight(cseq).value();
        wsi = this->outer_conns.right.find(cseq)->second;

        // iret = libwebsocket_write(wsi, NULL, 0, LWS_WRITE_CLOSE);
        qLogx()<<"closing wsi now...";
        libwebsockets_hangup_on_client(this->serv_ctx, 
                                       libwebsocket_get_socket_fd(wsi));
    }

    return 0;
}
