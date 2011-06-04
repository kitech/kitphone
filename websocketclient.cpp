// websocketclient.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-22 12:03:08 +0800
// Version: $Id$
// 

#include <QtCore>

#include <stdio.h>
#include <stdlib.h>

#include "simplelog.h"

#include "libwss/md5.h"

#include "websocketclient.h"

///////////////////////////////////////
///////
// TODO 更多的错误状态返回功能。
///////////////////////////////////////
WebSocketClient::WebSocketClient(QString uri)
    :QObject(0)
{
    this->m_uri = uri;
    this->initNoiseChars();

    this->m_sock = boost::shared_ptr<QTcpSocket>(new QTcpSocket());
    QObject::connect(this->m_sock.get(), SIGNAL(connected()), this, SLOT(on_connected_ws_server()));
    QObject::connect(this->m_sock.get(), SIGNAL(disconnected()), this, SLOT(on_disconnected_ws_server()));
    QObject::connect(this->m_sock.get(), SIGNAL(error(QAbstractSocket::SocketError )),
                     this, SLOT(on_ws_sock_error(QAbstractSocket::SocketError )));
    QObject::connect(this->m_sock.get(), SIGNAL(readyRead()), this, SLOT(on_backend_handshake_ready_read()));

    this->m_ws_ping_timer = new QTimer();
    this->m_ws_ping_timer->setInterval(3*1000);
    QObject::connect(this->m_ws_ping_timer, SIGNAL(timeout()), this, SLOT(on_ping_timeout()));
    this->m_ping_seq = 1;
}

WebSocketClient::~WebSocketClient()
{
    // qlog("freeing......\n");
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"freeing ...";
    this->disconnectFromServer();
}

/*
  rpath为null,则执行独立客户端模式
  rpath不为null,则执行非透明代理模式。
 */
bool WebSocketClient::connectToServer(QString rpath)
{
    QUrl mu(this->m_uri);
    qDebug()<<mu;

    unsigned short port = mu.port(80);
    QString host = mu.host();
    QString path = mu.path();

    this->m_rpath = rpath;

    this->m_sock->connectToHost(host, port);

    return true;
}

bool WebSocketClient::disconnectFromServer()
{
    if (this->m_sock) {
        this->m_sock->close();
        // delete this->m_sock;
    }
    this->m_ws_ping_timer->stop();
    
    return true;
}

void WebSocketClient::on_ws_sock_error(QAbstractSocket::SocketError socketError)
{
    qLogx()<<"";
    int error = socketError;
    QString errmsg = this->m_sock->errorString();
    emit this->onError(error, errmsg);
}

void WebSocketClient::on_ping_timeout()
{
    qLogx()<<"";
    int wlen = 0;
    unsigned char buf[32] = {0};

    // 00 00 00 00 00 00 00 00 11 FF
    // 00 00 00 00 00 00 00 00 02 00 FF
    // 89 88 4A AF 18 CA 4A AF 18 CA 4A AF 18 C3
    // buf[0] = 0x00;
    // strcpy((char*)buf+1, "ping");
    // *(buf+5) = 0xff;
    this->m_ping_seq ++;
    memcpy(&buf[8], &this->m_ping_seq, sizeof(int));
    buf[9+sizeof(int)-1] = 0xff;

    wlen = 9+sizeof(int);
    QByteArray ba = QByteArray((const char*)buf, wlen);
    // this->sendMessage(ba);

    bool ok = this->m_sock->write((const char*)buf, wlen);
    // 都不管用，服务器端还是会关闭连接
}

bool WebSocketClient::sendMessage(QByteArray msg)
{
 
    int wlen = 0;
    bool ok;
    unsigned char buf[512] = {0};
    QByteArray nba;

    Q_ASSERT(this->m_sock != NULL);

    buf[0] = 0x00;
    memcpy(buf+1, msg.data(), msg.length());
    buf[msg.length()+1] = 0xff;

    // ok = this->m_sock->putChar(0x00);
    // Q_ASSERT(ok);
    // wlen = this->m_sock->write((const char *)buf, len);
    // wlen = this->m_sock->write(msg);
    // ok = this->m_sock->putChar(0xff);
    // nba = QByteArray((const char*)buf, msg.length()+2);
    wlen = msg.length()+2;
    ok = this->m_sock->write((const char*)buf, wlen);
    Q_ASSERT(ok);
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<msg;
    // return wlen;

    return true;
}

void WebSocketClient::on_connected_ws_server()
{
    QUrl mu(this->m_uri);
    QByteArray request;
    QByteArray key1, key2, key3;
    QByteArray host, origin, path;

    request = QByteArray() +
        "GET {0} HTTP/1.1\r\n" +
        "Upgrade: WebSocket\r\n" +
        "Connection: Upgrade\r\n" +
        "Host: {1}\r\n" +
        "Origin: {2}\r\n" +
        "Cookie: {3}\r\n" +
        "Sec-WebSocket-Protocol: wso\r\n" +       // new ws version,20110409
        "Sec-WebSocket-Key1: {4}\r\n" +
        "Sec-WebSocket-Key2: {5}\r\n" +
        "{6}" +
        "\r\n";

    if (this->m_rpath.length() > 0) {
        path = this->m_rpath.toAscii();
    } else {
        path = mu.path().toAscii();
    }
    host = mu.host().toAscii();
    origin = "http://127.0.0.1/";
    
    key1 = this->generateKey();
    key2 = this->generateKey();
    key3 = this->generateKey3();

    this->expected_digest = this->getSecurityDigest(key1, key2, key3);

    request.replace("{0}", path);
    request.replace("{1}", host);
    request.replace("{2}", origin);
    request.replace("{3}", "");
    request.replace("{4}", key1);
    request.replace("{5}", key2);
    request.replace("{6}", "");

    this->m_sock->write(request);
    this->m_sock->write(key3);

    qDebug()<<"send client handshake request:"<<"";
    if (1) {
        QList<QByteArray> req_lines = request.split('\n');
        for (int i = 0; i < 6 && i < req_lines.count(); i ++) {
            qDebug()<<req_lines.at(i).trimmed();
        }
    }
    // qDebug()<<"send client handshake request:"<<request<<key3;    
}

bool WebSocketClient::isClosed()
{
    return this->m_sock.get() == 0 || this->m_sock->state() == QAbstractSocket::UnconnectedState;
}


void WebSocketClient::on_disconnected_ws_server()
{
    // 如果出现服务器端发起的关闭，则有问题。
    qLogx()<<"";
}

void WebSocketClient::on_backend_handshake_ready_read()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QByteArray ba;

    ba = this->m_sock->readAll();

    qDebug()<<"handshake return:"<<ba.length();
    QByteArray reply_digest;
    QList<QByteArray> header_lines = ba.split('\r');

    reply_digest = header_lines.at(header_lines.count()-1).trimmed();
    if (reply_digest == this->expected_digest) {
        qDebug()<<"client reiceve server handshake response, ok"<<this->expected_digest.length();

        qDebug()<<"disconnect object 2";
        QObject::disconnect(this->m_sock.get(), SIGNAL(readyRead()),
                            this, SLOT(on_backend_handshake_ready_read()));    
        QObject::connect(this->m_sock.get(), SIGNAL(readyRead()), this, SLOT(on_backend_ready_read()));

        emit onConnected(this->m_rpath);

        this->m_ws_ping_timer->start();
    } else {
        qDebug()<<"digest doesn't match:"
                <<"l"<<reply_digest.length()<<"=?"<<this->expected_digest.length()
                << "++"
                << reply_digest << "++!=++" << this->expected_digest << "++";
        // qDebug()<<"digest doesn't match:'"
        //         << reply_digest << "'!='" << this->expected_digest << "'";
        this->m_sock->close();
        emit this->onError(this->EWS_HANDSHAKE, QString(tr("Handshake with server faild.")));
    }
}

void WebSocketClient::on_backend_ready_read()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QByteArray ba;

    ba = this->m_sock->readAll();

    char pong_frm[32] = {0};
    char srcbuf[5120] = {0};
    // char msgbuf[5120] = {0};
    // int ret;
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<this->m_sock<<"read ws message"
            <<ba.length()<<ba;

    //always 00datahereff format ?????????
    memcpy(srcbuf, ba.data(), ba.length());

    if (memcmp(srcbuf, pong_frm, 8) == 0
        && (unsigned char)(srcbuf[ba.length() - 1]) == 0xff) {
        // 00 00 00 00 00 00 00 00 46 00 00 00 FF
        qLogx()<<"Recive server pong frame.B"<<ba.length();
    } else if ((unsigned char)(srcbuf[0]) == 0x00 
        && (unsigned char)(srcbuf[ba.length() - 1]) == 0xff) {
        ba = QByteArray(srcbuf + 1, ba.length() - 2);
        if (ba.length() > 0) {
            qDebug()<<"ws client/proxy got and broard msg:"<<ba;
            // emit this->newWSMessage(ba, sock);
            emit this->onWSMessage(ba);
        }
	// websocket close standard command 0xff0x00
    } else if(ba.length() == 2 && 
	      (unsigned char)(srcbuf[0]) == 0xff &&
	      (unsigned char)(srcbuf[1]) == 0x00) {
        qDebug()<<"websocket recieve close request.";
    } else {
        qDebug()<<"Invalid/Uknown ws data frame, omited";
    }

}

void WebSocketClient::initNoiseChars()
{
    this->noise_slen = 0;
    for (int i = 0x21; i <= 0x2f; ++i) {
        this->noise_chars[this->noise_slen] = i;
        this->noise_slen ++;
    }

    for (int j = 0x3a; j <= 0x7a; ++j) {
        this->noise_chars[this->noise_slen] = j;
        this->noise_slen ++;
    }
}

// [min,max]
inline unsigned int random_int(unsigned int min, unsigned int max)
{
    unsigned int rint;

    if (min == max) {
        return min;
    }

    rint = qrand() % ( max - min + 1) + min;

    return rint;
}

QByteArray WebSocketClient::generateKey()
{
    QByteArray key;
    char kbuf[100], ch;
    unsigned int spaces = random_int(1, 12);// = qrand()%11 + 1;
    unsigned int max = UINT_MAX / spaces;
    // should be random_int(0, max), but if max == UINT_MAX, a SIGFPE will crash process
    unsigned int number = random_int(0, spaces == 1 ? max-1 : max);//qrand()%max;

    memset(kbuf, 0, sizeof(kbuf));
#if defined(Q_WS_WIN)
    _snprintf(kbuf, sizeof(kbuf)-1, "%u", number * spaces);
#else
    snprintf(kbuf, sizeof(kbuf)-1, "%u", number * spaces);
#endif

    int noises = random_int(1, 12);// qrand()%12 + 1;
    int pos;
    int klen;

    key = QByteArray::number(number * spaces);
    klen = key.length();
    for (int i = 0; i < noises ; ++i) {
        // ch = this->noise_chars[qrand()%this->noise_slen];
        ch = this->noise_chars[random_int(0, this->noise_slen - 1)];
        // pos = random_int(0, strlen(kbuf)); // qrand()%(strlen(kbuf)+1);
        pos = random_int(0, klen);
        assert(pos >= 0 && pos <= klen);
        // if (pos < klen) {
        //     // memmove(kbuf + pos + 1, kbuf + pos, strlen(kbuf) - pos);
        //     memmove(kbuf + pos + 1, kbuf + pos, klen - pos);
        // }
        // kbuf[pos] = ch;
        key.insert(pos, ch);
        klen ++;
    }

    for (int j = 0; j < spaces; ++j) {
        // pos = random_int(1, strlen(kbuf) - 1); // qrand()%(strlen(kbuf)) + 1;
        pos = random_int(1, klen - 1);
        // memmove(kbuf + pos + 1, kbuf + pos, strlen(kbuf) - pos);
        assert(pos >= 1 && pos <= klen -1);
        // memmove(kbuf + pos + 1, kbuf + pos, klen - pos);
        // kbuf[pos] = ' ';
        key.insert(pos, ' ');
        klen ++;
    }

    // key = QByteArray(kbuf, klen);
    assert(klen == key.length());

    return key;
}

QByteArray WebSocketClient::generateKey3()
{
    QByteArray key3;

    for (int i = 0; i < 8; ++i) {
        // key3.append(qrand()%256);
        key3.append(random_int(0, 255));
    }

    return key3;
}

// static void *md5_buffer (const char *src, size_t len, void *dest)
// {
// 	char md5sum[36] = {0};
// 	char md5str[36] = {0} ;
// 	md5_context  ctx ;
// 	md5_starts(&ctx) ;	
// 	md5_update(&ctx,(unsigned char*)src , len);
// 	md5_finish(&ctx,(unsigned char *)md5sum);	
	
//     memcpy(dest, md5sum, 16);

// 	return dest;
// }

QByteArray WebSocketClient::getSecurityDigest(QByteArray key1, QByteArray key2, QByteArray key3)
{
    QByteArray bytes1;
    QByteArray bytes2;

    QByteArray bytes;

    bytes1 = this->keyToBytes(key1);
    bytes2 = this->keyToBytes(key2);

    char raw_md5[16] = {0};
    QByteArray ba_src = (bytes1 + bytes2 + key3);
    char *src = ba_src.data();

    // md5_buffer(src, ba_src.length(), raw_md5);
    MD5((const unsigned char*)src, ba_src.length(), (unsigned char*)raw_md5);

    bytes = QByteArray(raw_md5, 16);

    return bytes;
}

QByteArray WebSocketClient::keyToBytes(QByteArray key)
{
    QByteArray bytes;
    char kbuf[100] = {0}, ch;
    unsigned int keyNum = 0;
    unsigned int spaces = 0;

    for (int i = 0; i < key.length(); i++) {
        if (key[i] >= '0' && key[i] <= '9') {
            kbuf[strlen(kbuf)] = key[i];
        }
        if (key[i] == ' ') {
            spaces ++;
        }
    }
    keyNum = QString(kbuf).toUInt();

    unsigned int resultNum = keyNum / spaces;

    for (int j = 3 ; j >= 0; --j) {
        ch = (resultNum >> (j * 8)) & 0xff;
        bytes.append(ch);
    }

    return bytes;
}

////////////////////////
//////
/////////////////////////////////
