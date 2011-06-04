// websocket.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-15 22:50:52 +0800
// Version: $Id$
// 

#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include <QtCore>
#include <QtNetwork>

#include "../libng/qbihash.h"

#include "private-libwebsockets.h"
#include "libwebsockets.h"

// #include "boost/smart_ptr/enable_shared_from_this2.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/signals2.hpp"

typedef struct {
    int verbose;
    char listen_host[256];
    int listen_port;
    int handler_id;
    char *cert;
    char *key;
    int ssl_only;
    int daemon;
} settings_t;

typedef struct {
    char path[1024+1];
    char host[1024+1];
    char origin[1024+1];
    char key1[1024+1];
    char key2[1024+1];
    char key3[8+1];
} headers_t;

// class WebSocketServer : public QObject
// {
//     Q_OBJECT;
// public:
//     explicit WebSocketServer(QObject *parent = 0);
//     virtual ~WebSocketServer();

//     bool setAllowedHost(QStringList allows);
//     QTcpSocket *nextPendingConnection();

//     bool listen(unsigned short port); // all ip
//     unsigned short serverPort();
//     QString serverIpAddr(int type);

//     ssize_t recv(QTcpSocket *sock, void *buf, size_t len);
//     ssize_t send(QTcpSocket *sock, const void *buf, size_t len);
//     ssize_t wssend(QTcpSocket *sock, const void *buf, size_t len); // with frame code

// private:
//     bool handshake(QTcpSocket *sock);
//     int parse_handshake(char *handshake, headers_t *headers);
//     bool checkHostPermission(QString ws_uri);

// private slots:
//     void onNewRawConnection();
//     void onReadyRead();
//     void onDisconnected();

//     void onWSMessageReadyRead();

// private:
//     QTcpServer *ws_serv_sock;
//     QMap<QTcpSocket*, QByteArray> pending_conns; // before handshake done
//     QStringList allowed_hosts;

// signals:
//     // void newConnection(QString path, QTcpSocket *sock); // return handshaked socket connection
//     void newConnection(); // path in sock's property 'payload_path'
//     void newWSMessage(QByteArray msg, QTcpSocket *sock);
// };

///////////////////////////////
//////
////////////////////////////////
class WebSocketServer2 : public QThread
{
    Q_OBJECT;
public:
    explicit WebSocketServer2(QObject *parent = 0);
    virtual ~WebSocketServer2();

    virtual void run();

    bool setAllowedHost(QStringList allows);
    qint64 nextPendingConnection();

    bool listen(unsigned short port); // all ip
    unsigned short serverPort();
    static QString serverIpAddr(int type); // type 指定获取内网ip/外网ip

    ssize_t recv(qint64 cseq, void *buf, size_t len);
    ssize_t send(qint64 cseq, const void *buf, size_t len);
    ssize_t wssend(qint64 cseq, const void *buf, size_t len); // with frame code
    ssize_t wssend(qint64 cseq, const QString &msg);

    QString conn_payload_path(qint64 cseq);
    QString conn_get_peer_address(qint64 cseq);
    int conn_close(qint64 cseq);

    // cb by lws
    int lws_new_connection_established(libwebsocket *wsi);
    int lws_connection_closed(libwebsocket *wsi);
    int lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len);

private:
    // QTcpServer *ws_serv_sock;
    QMap<libwebsocket*, QByteArray> pending_conns; // before handshake done
    QMap<libwebsocket*, QByteArray> pending_payload_paths; // before handshake done
    KBiHash<libwebsocket*, qint64> outer_conns; // ws -> conn seq
    QStringList allowed_hosts;
    libwebsocket_context *serv_ctx;
    bool quit_srv_loop;
    qint64 outer_conn_seq;

signals:
    // void newConnection(QString path, QTcpSocket *sock); // return handshaked socket connection
    void newConnection(); // path in sock's property 'payload_path'
    // void newWSMessage(QByteArray msg, QTcpSocket *sock);
    void newWSMessage(QByteArray msg, qint64 conn_seq);
    void clientSessionClosed(qint64 conn_seq);
};

///////////////////////////////
//////
///////////////////////////////
class WebSocketClient : public QObject, public boost::enable_shared_from_this<WebSocketClient>
{
    Q_OBJECT;
public:
    WebSocketClient(QString uri);
    virtual ~WebSocketClient();
    boost::shared_ptr<WebSocketClient> refit() {
        return this->shared_from_this();
    }

    bool connectToServer(QString rpath = QString());
    bool disconnectFromServer();
    bool sendMessage(QByteArray msg);

    bool isClosed();

private slots:
    void on_connected_ws_server();
    void on_disconnected_ws_server();

    void on_backend_handshake_ready_read();
    void on_backend_ready_read();

private:
    void initNoiseChars();
    QByteArray generateKey();
    QByteArray generateKey3();
    QByteArray getSecurityDigest(QByteArray key1, QByteArray key2, QByteArray key3);
    QByteArray keyToBytes(QByteArray key);

private:
    QString m_uri;
    // QTcpSocket *m_sock;
    boost::shared_ptr<QTcpSocket> m_sock;
    QString m_rpath;
    char noise_chars[128];
    int noise_slen;
    QByteArray expected_digest;

public: // boost::signals2
    enum class MySignals {
        onConnected,
            onError,
            onDisconnected,
            onWSMessage
    };
    
    boost::signals2::connection connect(const char *sig, boost::signals2::signal<void(boost::shared_ptr<WebSocketClient>)>::slot_type slot) {
        boost::signals2::connection conn;

        // xxxxxxx
        if (strcmp(SIGNAL(onError()), sig) == 0) {

        } else if (strcmp(SIGNAL(onDisconnected()), sig) == 0) {

        } else {

        }

        return conn;
    }

private:
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient>, QString)> sig_onConnected;
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient>)> sig_onError;
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient>)> sig_onDisconnected;
    boost::signals2::signal<void(boost::shared_ptr<WebSocketClient>, QByteArray)> sig_onWSMessage;



signals:
    void onConnected(QString rpath);
    void onError();
    void onDisconnected();
    void onWSMessage(QByteArray msg);
};

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

    struct libwebsocket_protocols client_protocols[2];
    libwebsocket_context *m_lws_ctx;
    libwebsocket *m_wsi;
    int m_conn_cseq;
    bool quit_cli_loop;

public: // boost::signals2
    enum class MySignals {
        onConnected,
            onError,
            onDisconnected,
            onWSMessage
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

#endif /* _WEBSOCKET_H_ */
