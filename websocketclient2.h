// websocketclient2.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-31 17:31:25 +0000
// Version: $Id$
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
#include "private-libwebsockets.h"
#include "libwebsockets.h"


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

    bool connectToServer(QString rpath = QString());
    bool disconnectFromServer();
    bool sendMessage(QByteArray msg);

    bool isClosed();

protected:
    void run();

private slots:
    // void on_connected_ws_server();
    // void on_disconnected_ws_server();

    // void on_backend_handshake_ready_read();
    // void on_backend_ready_read();

public slots:
    // cb by lws
    int lws_new_connection_established(libwebsocket *wsi);
    int lws_connection_closed(libwebsocket *wsi);
    int lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len);
    
    // test bind method
    int callback_wso(struct libwebsocket_context * ctx,
                     struct libwebsocket *wsi,
                     enum libwebsocket_callback_reasons reason,
                     void *user, void *in, size_t len);

private:
    // void initNoiseChars();
    // QByteArray generateKey();
    // QByteArray generateKey3();
    // QByteArray getSecurityDigest(QByteArray key1, QByteArray key2, QByteArray key3);
    // QByteArray keyToBytes(QByteArray key);

private:
    QString m_uri;
    // QTcpSocket *m_sock;
    // boost::shared_ptr<QTcpSocket> m_sock;
    QString m_rpath;
    char noise_chars[128];
    int noise_slen;
    QByteArray expected_digest;

    // struct libwebsocket_protocols client_protocols[2];
    libwebsocket_context *m_lws_ctx;
    libwebsocket *m_wsi;
    int m_conn_cseq;
    bool quit_cli_loop;

public: // boost::signals2
    enum /* class */  MySignals {
        sOnConnected,
        sOnError,
        sOnDisconnected,
        sOnWSMessage
    };
    
    boost::signals2::connection connect(const char *sig, boost::signals2::signal<void(boost::shared_ptr<WebSocketClient2>)>::slot_type slot) {
        boost::signals2::connection conn;

        // xxxxxxx
        if (strcmp(SIGNAL(onError()), sig) == 0) {

        } else if (strcmp(SIGNAL(onDisconnected()), sig) == 0) {

        } else {

        }

        return conn;
    }

private:
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient2>, QString)> sig_onConnected;
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient2>)> sig_onError;
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient2>)> sig_onDisconnected;
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient2>, QByteArray)> sig_onWSMessage;



signals:
    void onConnected(QString rpath);
    void onError();
    void onDisconnected();
    void onWSMessage(QByteArray msg);
};


#endif /* _WEBSOCKETCLIENT2_H_ */
