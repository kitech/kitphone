// websocketclient2.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-31 17:31:18 +0000
// Version: $Id: websocketclient2.cpp 995 2011-09-16 09:51:17Z drswinghead $
// 


#include "simplelog.h"
#include "websocketclient2.h"
#include "intermessage.h"

///////////////////////////////////
/////////////////////////////////
////////////////////////////////////
struct libwebsocket_extension libwebsocket_vopp_client_extensions[] = {
	{ /* terminator */
		NULL, NULL, 0
	}
};

struct client_session_user_data {
    struct libwebsocket *wsi;
    int seq;

    char full_message[5120];
    PackageCombiner *pc;
};

/* list of supported protocols and callbacks */
static struct libwebsocket_protocols vopp_client_protocols[] = {
	{
		"vopp",
		&WebSocketClient2::lws_callback_vopp,
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
    this->m_uri = uri;
    this->quit_cli_loop = false;
    this->real_connect_now = false;
    this->m_lws_ctx = NULL;
    this->m_wsi = NULL;

    QObject::connect(this, SIGNAL(started()), this, SLOT(on_myself_started()));
}

WebSocketClient2::~WebSocketClient2()
{
    qLogx()<<"";
}

bool WebSocketClient2::connectToServer(QString uri)
{
    if (this->m_uri == uri && uri.isEmpty()) {
        qLogx()<<"Empty connect uri.";
        return false;
    }

    if (!uri.isEmpty()) this->m_uri = uri;

    this->start();
    
    return true;

}

bool WebSocketClient2::connectToServerImpl(QString uri)
{
    unsigned short ctx_port = CONTEXT_PORT_NO_LISTEN;

    Q_ASSERT(this->m_lws_ctx == NULL);
    const char *cert_path =
        // LOCAL_RESOURCE_PATH"/libwebsockets-test-server.pem";
        "./seckey/cacert.pem";
    const char *key_path =
        // LOCAL_RESOURCE_PATH"/libwebsockets-test-server.key.pem";
        "./seckey/privkey.pem";

    // memset(this->client_protocols, 0, sizeof(struct libwebsocket_protocols)*2);
    // VC10不支持
    // this->client_protocols[0] = 
    //     {
    //         "wso",
    //         callback_lws_client_wso,
    //         sizeof(struct client_session_user_data),
    //     };

    this->m_lws_ctx = libwebsocket_create_context_ex(ctx_port, NULL, vopp_client_protocols,
                                                     libwebsocket_vopp_client_extensions, 
                                                     cert_path, key_path, -1, -1, 0, this);
    // this->m_lws_ctx = libwebsocket_create_context_ex(ctx_port, NULL, vopp_client_protocols,
    //                                                  libwebsocket_vopp_client_extensions, NULL, NULL, -1, -1, 0, this);
    if (this->m_lws_ctx == NULL) {
    }
    qLogx()<<"wsc ctx init done."<<this->m_lws_ctx;

    return true;
}

bool WebSocketClient2::on_wsctx_inited()
{
    QUrl mu(this->m_uri);
    qLogx()<<mu;

    unsigned short port = mu.port(18080);
    QString host = mu.host();
    QString path = mu.path();

    // 默认为1,最安全
    int ssl_mode = 1; // 0,ws://, 1,wss://encrypt, 2,wss://self signed

    if (mu.scheme().toLower() == "ws") {
        ssl_mode = 0;
    } else if (mu.scheme().toLower() == "wss") {
        ssl_mode = 2;
    } else {
        Q_ASSERT(1==2);
    }

    int conn_retry = 3;
    while (conn_retry -- > 0) {
        // 需要关注测试一下，这个调用是否是阻塞式的，如果是非阻塞式，则可以这么用
        // 否则，还需要考虑其他解决方式。
        qLogx()<<this->m_uri<<host<<path<<port;
        qLogx()<<"Before libwss clint connect..."<<QDateTime::currentDateTime();
        this->m_wsi = libwebsocket_client_connect(this->m_lws_ctx, host.toAscii().data(), port, ssl_mode,
                                                  path.toAscii().data(), 
                                                  host.toAscii().data(), host.toAscii().data(),
                                                  vopp_client_protocols[0].name, -1);
        // this->m_wsi = libwebsocket_client_connect(this->m_lws_ctx, host.toAscii().data(), port, 0,
        //                                           path.toAscii().data(), 
        //                                           host.toAscii().data(), host.toAscii().data(),
        //                                           vopp_client_protocols[0].name, -1);

        if (this->m_wsi == NULL) {
            qLogx()<<"libwebsocket dumb connect failed.";
            return false;
        }

        // here the conn state is 4, but when run to CALLBACK_ESTABLISH, the state will be 3 ok
        qLogx()<<"After libwss client connect..."<<QDateTime::currentDateTime()
               <<libwebsockets_remaining_packet_payload(this->m_wsi)
               <<libwebsocket_get_socket_fd(this->m_wsi)
               <<libwebsocket_get_connect_state(this->m_wsi);
        
        qLogx()<<"client wsi:"<<this->m_wsi;
        if (libwebsocket_client_is_connected(this->m_wsi)) {
            break;
        } else {
            break;
            this->m_wsi = NULL;
            qLogx()<<"Invalid client wsi state, retry.. "<<conn_retry;
            if (conn_retry == 0) {
                Q_ASSERT(1==2);
                return false;
            }
        }
    }
    return true;
}

bool WebSocketClient2::on_wsc_service_loop()
{
    int iret = 0;
    static int tcnt = 0;

    qLogx()<<"ws client serviceing...";
    iret = libwebsocket_service(this->m_lws_ctx, 2000);
    qLogx()<<iret;
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    if (iret == 0) {
        // ok, success
    } else {
        // listen socket deak
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Listen socket maybe dead."
                <<"Need restart this server.";
    }
        
    if (this->m_wsi != NULL) {
        // QString a = QString("[abcdddddd %1]").arg(++tcnt);
        // qLogx()<<"Sending :"<<a;
        // this->sendMessage(a.toAscii());
    }

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
    char tbuf[512] = {0};

    if (this->m_wsi == NULL) {
        // 
        qLogx()<<"client not connect.";
        return false;
    }

    // libwebsocket_write(wsi,
    //                    &buf[LWS_SEND_BUFFER_PRE_PADDING], l, LWS_WRITE_TEXT);
    memcpy(&tbuf[LWS_SEND_BUFFER_PRE_PADDING], msg.data(), msg.length());

    wlen = libwebsocket_write(this->m_wsi, (unsigned char*)&tbuf[LWS_SEND_BUFFER_PRE_PADDING],
                              msg.length(), LWS_WRITE_TEXT);
    // wlen = libwebsocket_write(this->m_wsi, (unsigned char*)msg.data(), msg.length(), LWS_WRITE_TEXT);
    // wlen = libwebsocket_write(wsi, (unsigned char*)buf, len, LWS_WRITE_TEXT);
    qLogx()<<msg.length()<<wlen<<msg;

    return true;
}

bool WebSocketClient2::sendMessage(std::string msg)
{
    int wlen = 0;
    char tbuf[512] = {0};
    char tbuf2[512] = {0};

    if (this->m_wsi == NULL) {
        // 
        qLogx()<<"client not connect.";
        return false;
    }

    std::string tstr;

    PackageSplitter ps(msg);
    while (ps.hasNextFragment()) {
        tstr = ps.nextFragment();

        memset(tbuf, 0, sizeof(tbuf));
        strcpy(&tbuf[LWS_SEND_BUFFER_PRE_PADDING], tstr.c_str());
        
        wlen = libwebsocket_write(this->m_wsi, (unsigned char*)&tbuf[LWS_SEND_BUFFER_PRE_PADDING],
                                  tstr.length(), LWS_WRITE_TEXT);
        if (wlen == 0) {
            // success
        } else {
            // error
        }
    }

    // libwebsocket_write(wsi,
    //                    &buf[LWS_SEND_BUFFER_PRE_PADDING], l, LWS_WRITE_TEXT);
    // strcpy(&tbuf[LWS_SEND_BUFFER_PRE_PADDING], msg.c_str());
    // strcpy(&tbuf[LWS_SEND_BUFFER_PRE_PADDING], 
    // "abcdefgdddddddd112132324324i3423423423432432423432kdsjfewiafjeafijaweofjaioewfjaweifjawofjeaioejfiajfadffffffffffffffffffffo12");
    // qLogx()<<msg.length()<<wlen<<msg.c_str()<<QString::fromAscii((char*)(&tbuf[LWS_SEND_BUFFER_PRE_PADDING]), msg.length());

    // wlen = libwebsocket_write(this->m_wsi, (unsigned char*)&tbuf[LWS_SEND_BUFFER_PRE_PADDING],
    //                           strlen(&tbuf[LWS_SEND_BUFFER_PRE_PADDING]), LWS_WRITE_TEXT);
    // strncpy(tbuf2, &tbuf[LWS_SEND_BUFFER_PRE_PADDING], msg.length());
    // wlen = libwebsocket_write(this->m_wsi, (unsigned char*)msg.data(), msg.length(), LWS_WRITE_TEXT);
    // wlen = libwebsocket_write(wsi, (unsigned char*)buf, len, LWS_WRITE_TEXT);
    // qLogx()<<msg.length()<<wlen<<msg.c_str()<<QString(&tbuf[LWS_SEND_BUFFER_PRE_PADDING])
    //        <<strlen(&tbuf[LWS_SEND_BUFFER_PRE_PADDING]);
    // <<QString::fromAscii((char*)(&tbuf[LWS_SEND_BUFFER_PRE_PADDING]), msg.length());

    return true;
    
}

bool WebSocketClient2::isConnected()
{
    
    return (this->m_wsi != NULL && libwebsocket_client_is_connected(this->m_wsi));
}

bool WebSocketClient2::isClosed()
{
    return true;
}

int WebSocketClient2::lws_new_connection_established(libwebsocket *wsi)
{
    //fprintf(stderr, "c_path: %s\n", wsi->c_path);
    qLogx()<<wsi<<libwebsocket_get_cpath(wsi).c_str();
    std::string req_path;

    req_path = libwebsocket_get_request_path(wsi);

    emit this->onConnected(QString(req_path.c_str()));

    return 0;
}

int WebSocketClient2::lws_connection_closed(libwebsocket *wsi)
{
    qLogx()<<"backed ws close event:"<<wsi;

    emit this->onDisconnected();

    return 0;
}

int WebSocketClient2::lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len)
{
    int cseq = 0;
    QByteArray nmsg = QByteArray(msg, len);

    qLogx()<<"ws serv got and broard msg:"<<nmsg<<cseq;

    emit this->onWSMessage(nmsg);
    
    return 0;
}

int WebSocketClient2::lws_callback_vopp(struct libwebsocket_context * ctx,
                                        struct libwebsocket *wsi,
                                        enum libwebsocket_callback_reasons reason,
                                        void *user, void *in, size_t len)
{
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 +
                      LWS_SEND_BUFFER_POST_PADDING];
	int l;
    int n;
    std::string str1, str2;

    struct client_session_user_data *ssd = (struct client_session_user_data*)user;
    WebSocketClient2 *wsc = (WebSocketClient2*)(libwebsocket_context_user_data(ctx));

	switch (reason) {
	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "mirror: LWS_CALLBACK_CLOSED\n");
        if (ssd->pc != NULL) {
            delete ssd->pc; ssd->pc = NULL;
        }
        wsc->lws_connection_closed(wsi);
		break;

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
        // 因为在调用libwebsocket_client_connect的时候，事件循环还没有启动，
        // 所以事件不会传递出来。
        qLogx()<<"connection state:"
               <<libwebsockets_remaining_packet_payload(wsi)
               <<libwebsocket_get_socket_fd(wsi)
               <<libwebsocket_get_connect_state(wsi);

        ssd->pc = new PackageCombiner;

        wsc->lws_new_connection_established(wsi);
		/*
		 * start the ball rolling,
		 * LWS_CALLBACK_CLIENT_WRITEABLE will come next service
		 */

		// libwebsocket_callback_on_writable(ctx, wsi);
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
        /*		fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in); */
        // wsc->lws_ws_message_ready(wsi, (char*)in, len);

        str1 = std::string((char*)in, len);
        ssd->pc->save_fragment(str1);
        if (ssd->pc->is_package_finish(str1)) {
            str2 = ssd->pc->get_full_package(str1);
            strncpy(ssd->full_message, str2.c_str(), sizeof(ssd->full_message)-1);
            n = wsc->lws_ws_message_ready(wsi, ssd->full_message, str2.length());
        }

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
        libwebsocket_callback_on_writable(ctx, wsi);
		// /*
		//  * Without at least this delay, we choke the browser
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

void WebSocketClient2::run()
{
    int iret = 0;
    bool bret = false;
    int tcnt = 0;
    
    // init only now
    bret = this->connectToServerImpl(this->m_uri);
    if (bret == false) {
        qLogx()<<"returnted "<<bret;
        return;
    }

    // qLogx()<<"enter client service loop "<<bret;
    // while (!this->quit_cli_loop) {
    //     qLogx()<<"ws client serviceing...";
    //     iret = libwebsocket_service(this->m_lws_ctx, 2000);
    //     qLogx()<<iret;
    //     // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    //     if (iret == 0) {
    //         // ok, success
    //     } else {
    //         // listen socket deak
    //         qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Listen socket maybe dead."
    //                 <<"Need restart this server.";
    //     }
        
    //     if (this->m_wsi != NULL) {
    //         QString a = QString("abcdddddd %1").arg(++tcnt);
    //         qLogx()<<"Sending :"<<a;
    //         this->sendMessage(a.toAscii());
    //     }
    // }


    // 这样的话，QThread::exec没有执行，则该线程的信号/槽不能使用。
    // while (!this->quit_cli_loop) {
    //     bret = on_wsc_service_loop();
    //     qLogx()<<bret;
    // }

    this->exec();
}

void WebSocketClient2::on_myself_started()
{
    moveToThread(this); // 非常重要的操作，在这句之前，运行在主界面线程，在这句之后，运行在该线程之内。

    qLogx()<<"";
    // 使用先启动service,后启动连接的方法，这样还可收到连接建立的事件，    LWS_CALLBACK_ESTABLISHED
    // 启动service
    QTimer *loop_timer = new QTimer();
    QObject::connect(loop_timer, SIGNAL(timeout()), this, SLOT(on_wsc_service_loop()));
    loop_timer->start(1000);

    // 连接到服务器
    QTimer::singleShot(30, this, SLOT(on_wsctx_inited()));
}
