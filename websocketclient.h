// websocketclient.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-22 12:02:57 +0800
// Version: $Id$
// 

#ifndef _WEBSOCKETCLIENT_H_
#define _WEBSOCKETCLIENT_H_

#include <QtCore>
#include <QtNetwork>

// #include "boost/smart_ptr/enable_shared_from_this2.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/signals2.hpp"


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
    enum MySignals {
        SIG_onConnected,
        SIG_onError,
        SIG_onDisconnected,
        SIG_onWSMessage
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


#endif /* _WEBSOCKETCLIENT_H_ */
