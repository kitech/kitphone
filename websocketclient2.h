// websocketclient2.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-31 17:31:25 +0000
// Version: $Id: websocketclient2.h 977 2011-09-06 15:08:42Z drswinghead $
// 

#ifndef _WEBSOCKETCLIENT2_H_
#define _WEBSOCKETCLIENT2_H_

#include <QtCore>

// #include "boost/smart_ptr/enable_shared_from_this2.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/signals2.hpp"

// 当这两个文件与<QtCore>同时包含时，需要把windows SDK中的winsock.h改与一个空文件，否则编译不过
// 因为libwebsockets使用的winsock2.h，而<QtCore>会把winsock.h包含进来，这两个头文件有冲突。
// #include "private-libwebsockets.h"
#include "libwebsockets.h"
#include "libwebsockets_extra.h"


///////////////////////////////
////// websock client using libwebsockets
///////////////////////////////
class WebSocketClient2 : public QThread, public boost::enable_shared_from_this<WebSocketClient2>
{
    Q_OBJECT;
public:
    explicit WebSocketClient2(QString uri);
    virtual ~WebSocketClient2();
    boost::shared_ptr<WebSocketClient2> refit() {
        return this->shared_from_this();
    }

    bool connectToServer(QString uri = QString());
    bool disconnectFromServer();
    bool sendMessage(QByteArray msg);
    bool sendMessage(std::string msg);

    bool isClosed();

protected:
    virtual void run();
    virtual bool connectToServerImpl(QString uri);

public slots:
    // cb by lws
    int lws_new_connection_established(libwebsocket *wsi);
    int lws_connection_closed(libwebsocket *wsi);
    int lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len);
    
    static int lws_callback_vopp(struct libwebsocket_context * ctx,
                                 struct libwebsocket *wsi,
                                 enum libwebsocket_callback_reasons reason,
                                 void *user, void *in, size_t len);

private slots:
    bool on_wsctx_inited();
    bool on_wsc_service_loop();
    void on_myself_started();

private:
    QString m_uri;

    libwebsocket_context *m_lws_ctx;
    libwebsocket *m_wsi;
    bool quit_cli_loop;
    bool real_connect_now;

signals:
    void onConnected(QString rpath);
    void onError();
    void onDisconnected();
    void onWSMessage(QByteArray msg);
};


#endif /* _WEBSOCKETCLIENT2_H_ */
