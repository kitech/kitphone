// websocketclient2.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-31 17:31:18 +0000
// Version: $Id$
// 


#include "simplelog.h"
#include "websocketclient2.h"

///////////////////////////////////
/////////////////////////////////
////////////////////////////////////
////// first wrapper for payloadable context
static struct libwebsocket_context * libwebsocket_create_context_ex (int port, const char * interf, struct libwebsocket_protocols * protocols, struct libwebsocket_extension * extensions, const char * ssl_cert_filepath, const char * ssl_private_key_filepath, int gid, int uid, unsigned int options, void *user_data)
{
    struct libwebsocket_context *ctx = NULL;

    ctx = libwebsocket_create_context(port, interf, protocols, extensions, ssl_cert_filepath, ssl_private_key_filepath, gid, uid, options);
    if (ctx != NULL) {
        ctx->user_data = user_data;
    }

    return ctx;
}

static void *libwebsocket_context_user_data(libwebsocket_context *ctx)
{
    if (ctx != NULL) 
        return ctx->user_data;

    return NULL;
}

struct libwebsocket_extension libwebsocket_my_extensions[] = {
	{ /* terminator */
		NULL, NULL, 0
	}
};


/* lws-client_wso_protocol */
static int callback_lws_client_wso(struct libwebsocket_context * ctx,
                                   struct libwebsocket *wsi,
                                   enum libwebsocket_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 +
                      LWS_SEND_BUFFER_POST_PADDING];
	int l;

    WebSocketClient2 *wsc = (WebSocketClient2*)(libwebsocket_context_user_data(ctx));

	switch (reason) {
	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "mirror: LWS_CALLBACK_CLOSED\n");
        wsc->lws_connection_closed(wsi);
		break;

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
        wsc->lws_new_connection_established(wsi);
		/*
		 * start the ball rolling,
		 * LWS_CALLBACK_CLIENT_WRITEABLE will come next service
		 */

		// libwebsocket_callback_on_writable(ctx, wsi);
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
        /*		fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in); */
        wsc->lws_ws_message_ready(wsi, (char*)in, len);
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:

		// l = sprintf((char *)&buf[LWS_SEND_BUFFER_PRE_PADDING],
		// 			"c #%06X %d %d %d;",
		// 			(int)random() & 0xffffff,
		// 			(int)random() % 500,
		// 			(int)random() % 250,
		// 			(int)random() % 24);

		// libwebsocket_write(wsi,
        //                    &buf[LWS_SEND_BUFFER_PRE_PADDING], l, LWS_WRITE_TEXT);

		// /* get notified as soon as we can write again */

		// libwebsocket_callback_on_writable(ctx, wsi);

		// /*
		//  * without at least this delay, we choke the browser
		//  * and the connection stalls, despite we now take care about
		//  * flow control
		//  */

		// usleep(200);
		break;

	default:
		break;
	}

	return 0;
}

struct client_session_user_data {
    struct libwebsocket *wsi;
    int seq;
};
/* list of supported protocols and callbacks */
static struct libwebsocket_protocols client_protocols[] = {
	{
		"wso",
		callback_lws_client_wso,
		sizeof(struct client_session_user_data),
	},
	{  /* end of list */
		NULL,
		NULL,
		0
	}
};


WebSocketClient2::WebSocketClient2(QString uri)
    : QThread(0)
{
    this->m_rpath = uri;
}

WebSocketClient2::~WebSocketClient2()
{
    qLogx()<<"";
}

bool WebSocketClient2::connectToServer(QString rpath)
{
    unsigned short ctx_port = CONTEXT_PORT_NO_LISTEN;

    // memset(this->client_protocols, 0, sizeof(struct libwebsocket_protocols)*2);
    // VC10不支持
    // this->client_protocols[0] = 
    //     {
    //         "wso",
    //         callback_lws_client_wso,
    //         sizeof(struct client_session_user_data),
    //     };

    this->m_lws_ctx = libwebsocket_create_context_ex(ctx_port, NULL, client_protocols, libwebsocket_my_extensions, NULL, NULL, -1, -1, 0, this);
    Q_ASSERT(this->m_lws_ctx != NULL);


    QUrl mu(this->m_uri);
    qDebug()<<mu;

    unsigned short port = mu.port(80);
    QString host = mu.host();
    QString path = mu.path();

    // QString myname = WebSocketServer2::serverIpAddr(0);
    QString myname;

    // 需要关注测试一下，这个调用是否是阻塞式的，如果是非阻塞式，则可以这么用
    // 否则，还需要考虑其他解决方式。
    qLogx()<<"Before libwss clint connect..."<<QDateTime::currentDateTime();
	this->m_wsi = libwebsocket_client_connect(this->m_lws_ctx, host.toAscii().data(), port, 0,
                                              path.toAscii().data(), 
                                              myname.toAscii().data(), myname.toAscii().data(),
                                              client_protocols[0].name, -1);
    qLogx()<<"After libwss clint connect..."<<QDateTime::currentDateTime();

	if (this->m_wsi == NULL) {
		qLogx()<<"libwebsocket dumb connect failed.";
		return false;
	}

    this->start();
    
    return true;
}

bool WebSocketClient2::disconnectFromServer()
{

    libwebsocket_close_and_free_session(this->m_lws_ctx,
                                        this->m_wsi, LWS_CLOSE_STATUS_GOINGAWAY);
    this->quit_cli_loop = true;
    this->m_wsi = NULL;

    libwebsocket_context_destroy(this->m_lws_ctx);
    this->m_lws_ctx = NULL;
    
    return true;
}

bool WebSocketClient2::sendMessage(QByteArray msg)
{
    int wlen = 0;

    wlen = libwebsocket_write(this->m_wsi, (unsigned char*)msg.data(), msg.length(), LWS_WRITE_TEXT);
    // wlen = libwebsocket_write(wsi, (unsigned char*)buf, len, LWS_WRITE_TEXT);
    qLogx()<<wlen<<msg;
    return true;
}


bool WebSocketClient2::isClosed()
{
    return true;
}


int WebSocketClient2::lws_new_connection_established(libwebsocket *wsi)
{
    //fprintf(stderr, "c_path: %s\n", wsi->c_path);
    fprintf(stderr, "\n\n\n");
    struct lws_tokens *t;
    for (int i = 0 ; i < WSI_TOKEN_COUNT; i++) {
        t = &wsi->utf8_token[i];
        if (t->token_len == 0) {
            break;
        }
        fprintf(stderr, "dumping hdr: %s, %d\n", t->token, t->token_len);
        if (i == WSI_TOKEN_GET_URI) {
            // should be c_path token
            // this->pending_payload_paths.insert(wsi, QByteArray(t->token, t->token_len));
        }
    }
    // this->pending_conns.insert(wsi, QByteArray());
    // emit newConnection();
    return 0;
}

int WebSocketClient2::lws_connection_closed(libwebsocket *wsi)
{
    // qDebug()<<"backend ws close event:"<<wsi;
    // qlog("backend ws close event: %p", wsi);
    qLogx()<<"backed ws close event:"<<wsi;
    qint64 cseq = 0;
    // if (this->outer_conns.leftContains(wsi)) {
    //     emit this->clientSessionClosed(cseq);
    // }

    return 0;
}

int WebSocketClient2::lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len)
{
    int cseq = 0;
    QByteArray nmsg = QByteArray(msg, len);

    // cseq = this->outer_conns.findLeft(wsi).value();

    qDebug()<<"ws serv got and broard msg:"<<nmsg<<cseq;

    emit this->onWSMessage(nmsg);
    // emit this->newWSMessage(nmsg, cseq);
    // emit this->newWSMessage(nmsg, (QTcpSocket*)wsi);
    
    return 0;
}

// test bind method
int WebSocketClient2::callback_wso(struct libwebsocket_context * ctx,
                                   struct libwebsocket *wsi,
                                   enum libwebsocket_callback_reasons reason,
                                   void *user, void *in, size_t len)
{

    return 0;
}

void WebSocketClient2::run()
{
    int iret = 0;
    while (!quit_cli_loop) {
        iret = libwebsocket_service(this->m_lws_ctx, 2000);
        // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
        if (iret == 0) {
            // ok, success
        } else {
            // listen socket deak
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Listen socket maybe dead."
                    <<"Need restart this server.";
        }
    }
    
    this->exec();
}

