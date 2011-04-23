// websocket.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-15 22:51:07 +0800
// Version: $Id$
// 

#include <stdlib.h>
#include <resolv.h>
#include <errno.h>

#include "../utils.h"
#include "md5.h"

#include "websocket.h"


const char server_handshake[] = "HTTP/1.1 101 Web Socket Protocol Handshake\r\n\
Upgrade: WebSocket\r\n\
Connection: Upgrade\r\n\
%sWebSocket-Origin: %s\r\n\
%sWebSocket-Location: %s://%s%s\r\n\
%sWebSocket-Protocol: webso\r\n\
\r\n%s";

const char policy_response[] = "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>\n";

WebSocketServer::WebSocketServer(QObject *parent)
    : QObject(parent)
{
    this->ws_serv_sock = NULL;
    // this->allowed_hosts << "1.2.3.4";
}

WebSocketServer::~WebSocketServer()
{
}

bool WebSocketServer::setAllowedHost(QStringList allows)
{
  
    this->allowed_hosts = allows;
  
    return true;
}


QTcpSocket *WebSocketServer::nextPendingConnection()
{
    QTcpSocket *sock = NULL;

    if (this->pending_conns.size() > 0) {
        sock = this->pending_conns.begin().key();
        this->pending_conns.erase(this->pending_conns.begin());

        qDebug()<<"disconnect object 3";
        QObject::disconnect(sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    }

    return sock;
}

bool WebSocketServer::listen(unsigned short port)
{
    // port = 8080;
    this->ws_serv_sock = new QTcpServer();
    QObject::connect(this->ws_serv_sock, SIGNAL(newConnection()),
                     this, SLOT(onNewRawConnection()));
    bool ok = this->ws_serv_sock->listen(QHostAddress::Any, port);

    
    return ok;
}

unsigned short WebSocketServer::serverPort()
{
    return this->ws_serv_sock->serverPort();
}

QString WebSocketServer::serverIpAddr(int type)
{
    QString ipaddr;
    QHostAddress addr;
    QList<QHostAddress> addr_list;
    QList<QString> addr_str_list;
    
    addr = this->ws_serv_sock->serverAddress();
    qDebug()<<"ws listen ip addr:"<<addr;
    if (addr == QHostAddress::Null) {
        
    } else {
        ipaddr = addr.toString();
    }

    addr_list = QNetworkInterface::allAddresses();
    // qDebug()<<addr_list;
    for (int i = 0 ; i < addr_list.count() ; i ++) {
      addr_str_list.append(addr_list.at(i).toString());
    }
    // qSort(list.begin(), list.end(), qGreater<int>());
    qSort(addr_str_list.begin(), addr_str_list.end(), qGreater<QString>());

    if (addr_str_list.count() == 0) {
    } else if (addr_str_list.count() == 1) {
        // must be 127.0.0.1
        ipaddr = addr_str_list.at(0);
    } else {
        for (int i = 0 ; i < addr_str_list.count(); i ++) {
	  // addr = addr_list.at(i);
	  ipaddr = addr_str_list.at(i);
            if (ipaddr.indexOf(":") != -1) {
                // ipv6 addr
                ipaddr = QString();
                continue;
            } else {
                if (ipaddr.startsWith("127.0")) {
                    ipaddr = QString();
                    continue;
                } else if (!ipaddr.startsWith("172.24.")
                           &&!ipaddr.startsWith("192.168.")
                           &&!ipaddr.startsWith("10.10.")) {
                    // should a big ip addr
		  qDebug()<<"break big ip";
                    break;
                } else if (ipaddr.startsWith("172.24.")) {
		  qDebug()<<"break 172.2";
                    break;
                } else if (ipaddr.startsWith("10.10.")) {
                    break;
                } else if (ipaddr.startsWith("192.168.")) {
                    break;
                } else {
                    // do not want go here
                    Q_ASSERT(1 == 2);
                    break;
                }
            }
            ipaddr = QString();
        }
    }

    return ipaddr;
}

ssize_t WebSocketServer::recv(QTcpSocket *sock, void *buf, size_t len)
{
    return 0;
}

ssize_t WebSocketServer::send(QTcpSocket *sock, const void *buf, size_t len)
{
    ssize_t wlen = 0;

    Q_ASSERT(sock != NULL);
    wlen = sock->write((const char *)buf, len);
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<(const char*)buf;
    return wlen;
}

ssize_t WebSocketServer::wssend(QTcpSocket *sock, const void *buf, size_t len)
{
    ssize_t wlen = 0;
    bool ok;
    char wbuf[1000] = {0};

    Q_ASSERT(sock != NULL);

    // ok = sock->putChar(0x00);
    // Q_ASSERT(ok);
    // wlen = sock->write((const char *)buf, len);
    // ok = sock->putChar(0xff);
    // Q_ASSERT(ok);
    wbuf[0] = 0x00;
    wbuf[len+1] = 0xff;
    memcpy(wbuf+1, buf, len);
    
    wlen = sock->write(wbuf, len+2);
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<(const char*)buf;
    return wlen;
}

bool WebSocketServer::handshake(QTcpSocket *sock)
{
    char handshake[4096], response[4096], trailer[17];
    char *scheme, *pre;
    headers_t headers;
    int len, ret;


    return true;
}

/* ------------------------------------------------------- */
static int encode(u_char const *src, size_t srclength, char *target, size_t targsize) {
    int i, sz = 0, len = 0;
    unsigned char chr;
    target[sz++] = '\x00';
    len = b64_ntop(src, srclength, target+sz, targsize-sz);
    if (len < 0) {
        return len;
    }
    sz += len;
    target[sz++] = '\xff';
    return sz;
}

static int decode(char *src, size_t srclength, unsigned char *target, size_t targsize) {
    char *start, *end, cntstr[4];
    int i, len, framecount = 0, retlen = 0;
    unsigned char chr;
    if ((src[0] != '\x00') || (src[srclength-1] != '\xff')) {
        qDebug()<<("WebSocketServer framing error\n");
        return -1;
    }
    start = src+1; // Skip '\x00' start
    do {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");

        /* We may have more than one frame */
        end = (char*)memchr(start, '\xff', srclength);
        *end = '\x00';
        errno = 0;
        qDebug()<<"b64_pton erro:"<<codec->toUnicode(strerror(errno));
        len = b64_pton(start, target+retlen, targsize-retlen);
        if (len < 0) {
            qDebug()<<"b64_pton erro:"<<len<<codec->toUnicode(strerror(errno));
            return len;
        }
        retlen += len;
        start = end + 2; // Skip '\xff' end and '\x00' start 
        framecount++;
    } while (end < (src+srclength-1));
    if (framecount > 1) {
        memset(cntstr, 0, sizeof(cntstr));
        snprintf(cntstr, 3, "%d", framecount);
        // traffic(cntstr);
        qDebug()<<"frame count this time:"<<cntstr;
    }
    return retlen;
}

int WebSocketServer::parse_handshake(char *handshake, headers_t *headers)
{
    char *start, *end;

    if ((strlen(handshake) < 92) || (bcmp(handshake, "GET ", 4) != 0)) {
        return 0;
    }
    start = handshake+4;
    end = strstr(start, " HTTP/1.1");
    if (!end) { return 0; }
    strncpy(headers->path, start, end-start);
    headers->path[end-start] = '\0';

    start = strstr(handshake, "\r\nHost: ");
    if (!start) { return 0; }
    start += 8;
    end = strstr(start, "\r\n");
    strncpy(headers->host, start, end-start);
    headers->host[end-start] = '\0';

    start = strstr(handshake, "\r\nOrigin: ");
    if (!start) { return 0; }
    start += 10;
    end = strstr(start, "\r\n");
    strncpy(headers->origin, start, end-start);
    headers->origin[end-start] = '\0';
   
    start = strstr(handshake, "\r\n\r\n");
    if (!start) { return 0; }
    start += 4;
    if (strlen(start) == 8) {
        strncpy(headers->key3, start, 8);
        headers->key3[8] = '\0';

        start = strstr(handshake, "\r\nSec-WebSocket-Key1: ");
        if (!start) { return 0; }
        start += 22;
        end = strstr(start, "\r\n");
        strncpy(headers->key1, start, end-start);
        headers->key1[end-start] = '\0';
    
        start = strstr(handshake, "\r\nSec-WebSocket-Key2: ");
        if (!start) { return 0; }
        start += 22;
        end = strstr(start, "\r\n");
        strncpy(headers->key2, start, end-start);
        headers->key2[end-start] = '\0';
    } else {
        headers->key1[0] = '\0';
        headers->key2[0] = '\0';
        headers->key3[0] = '\0';
    }

    return 1;
}

bool WebSocketServer::checkHostPermission(QString ws_uri)
{
    QUrl u(ws_uri);

    if (this->allowed_hosts.count() == 0) {
        return true;
    }

    if (this->allowed_hosts.contains(u.host())) {
        return true;
    }
  
    return false;
}

static void *md5_buffer (const char *src, size_t len, void *dest)
{
	char md5sum[36] = {0};
	char md5str[36] = {0} ;
	md5_context  ctx ;
	md5_starts(&ctx) ;	
	md5_update(&ctx,(unsigned char*)src , len);
	md5_finish(&ctx,(unsigned char *)md5sum);	
	
    memcpy(dest, md5sum, 16);

	return dest;
}


static int gen_md5(headers_t *headers, char *target) {
    unsigned int i, spaces1 = 0, spaces2 = 0;
    // unsigned long num1 = 0, num2 = 0;
    quint32 num1 = 0, num2 = 0;
    unsigned char buf[17];
    for (i=0; i < strlen(headers->key1); i++) {
        if (headers->key1[i] == ' ') {
            spaces1 += 1;
        }
        if ((headers->key1[i] >= 48) && (headers->key1[i] <= 57)) {
            num1 = num1 * 10 + (headers->key1[i] - 48);
        }
    }
    qDebug()<<"num1 = num1 / spaces1;"<<(num1/spaces1)<<num1<<spaces1;
    num1 = num1 / spaces1;

    for (i=0; i < strlen(headers->key2); i++) {
        if (headers->key2[i] == ' ') {
            spaces2 += 1;
        }
        if ((headers->key2[i] >= 48) && (headers->key2[i] <= 57)) {
            num2 = num2 * 10 + (headers->key2[i] - 48);
        }
    }
    qDebug()<<"num2 = num2 / spaces2;"<<(num2/spaces2)<<num2<<spaces2;
    num2 = num2 / spaces2;

    /* Pack it big-endian */
    buf[0] = (num1 & 0xff000000) >> 24;
    buf[1] = (num1 & 0xff0000) >> 16;
    buf[2] = (num1 & 0xff00) >> 8;
    buf[3] =  num1 & 0xff;

    buf[4] = (num2 & 0xff000000) >> 24;
    buf[5] = (num2 & 0xff0000) >> 16;
    buf[6] = (num2 & 0xff00) >> 8;
    buf[7] =  num2 & 0xff;

    char *ptr = (char*)(buf + 8);
    // strncpy(buf+8, headers->key3, 8);
    strncpy(ptr, headers->key3, 8);
    buf[16] = '\0';

    ptr = (char*)buf;
    // md5_buffer(buf, 16, target);
    md5_buffer(ptr, 16, target);
    target[16] = '\0';

    return 1;
}

void WebSocketServer::onNewRawConnection()
{
    QTcpSocket *sock = this->ws_serv_sock->nextPendingConnection();
    this->pending_conns.insert(sock, QByteArray());
    QObject::connect(sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::connect(sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    
}

void WebSocketServer::onReadyRead()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    QByteArray ba = sock->readAll();

    char handshake[4096] = {0}, response[4096] = {0}, trailer[17] = {0};
    char *scheme = 0, *pre = 0;
    headers_t headers = {0};
    int len, ret;

    len = ba.length();
    strncpy(handshake, ba.data(), sizeof(handshake));    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sock<<ba.length()<<strlen(handshake)<<ba.length();
    if (1) {
        // log request and response without binary key data part
        QList<QByteArray> req_lines = ba.split('\n');
        for (int i = 0; i < 5 && i < req_lines.count(); i++) {
            qDebug()<<req_lines.at(i).trimmed();
        }
    }

    if (len == 0) {
        qDebug()<<("ignoring empty handshake\n");
        // close(sock);
        sock->close();
	this->pending_conns.remove(sock);
	delete sock;
        return;
    } else if (bcmp(handshake, "<policy-file-request/>", 22) == 0) {
        // len = recv(sock, handshake, 1024, 0);
        // handshake[len] = 0;
        qDebug()<<("sending flash policy response\n");
        // send(sock, policy_response, sizeof(policy_response), 0);
        // close(sock);
	this->send(sock, policy_response, sizeof(policy_response));
	sock->waitForBytesWritten(1000);
        sock->close();
	this->pending_conns.remove(sock);
	delete sock;
        return;
    } else if((bcmp(handshake, "\x16", 1) == 0) ||
              (bcmp(handshake, "\x80", 1) == 0)) {
        // SSL
        scheme = "wss";
        qDebug()<<("using SSL socket\n");
    } else {
        scheme = "ws";
        qDebug()<<("using plain (not SSL) socket\n");
    }
    
    if (!parse_handshake(handshake, &headers)) {
        qDebug()<<"Invalid WS request\n";
        // close(sock);
        sock->close();
	this->pending_conns.remove(sock);
	delete sock;
        return;
    }

    if (headers.key3[0] != '\0') {
        gen_md5(&headers, trailer);
        pre = "Sec-";
        qDebug()<<("using protocol version 76\n");
    } else {
        trailer[0] = '\0';
        pre = "";
        qDebug()<<("using protocol version 75\n");
    }

    qDebug()<<"orig:"<<headers.origin;
    qDebug()<<"scheme:"<<scheme;
    qDebug()<<"headers.host:"<<headers.host;
    qDebug()<<"headers.path:"<<headers.path;
    qDebug()<<"trailer:"<<trailer;
    
    sprintf(response, server_handshake, pre, headers.origin, pre, scheme,
            headers.host, headers.path, pre, trailer);
    //handler_msg("response: %s\n", response);
    // ws_send(ws_ctx, response, strlen(response));

    qDebug()<<"disconnect object 1";
    QObject::disconnect(sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    if (!this->checkHostPermission(QString(headers.origin))) {
      qDebug()<<"WebSocketServer connection from this origin host is not allowed:"<<QString(headers.origin);
      sock->close();
      this->pending_conns.remove(sock);
      delete sock;
      return;
    }

    QObject::connect(sock, SIGNAL(readyRead()), this, SLOT(onWSMessageReadyRead()));

    // handshake ok, broadcast new ws connection
    QString path = QString(headers.path);
    sock->setProperty("payload_path", path);
    // emit newConnection(path, sock);
    emit newConnection();
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sock<<"broard cast new ws connection.";

    this->send(sock, response, strlen(response));
}

void WebSocketServer::onDisconnected()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sock<<"closed pending websocket.";
    Q_ASSERT(this->pending_conns.contains(sock));
    this->pending_conns.remove(sock);
    sock->close();
    sock->deleteLater(); // free mem, in it's signal handle, cannot delete directly, use this instead
}

void WebSocketServer::onWSMessageReadyRead()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    QByteArray ba = sock->readAll();
    char srcbuf[5120] = {0};
    // char msgbuf[5120] = {0};
    // int ret;
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sock<<"read ws message"
            <<ba.length()<<ba;

    //always 00datahereff format ?????????
    memcpy(srcbuf, ba.data(), ba.length());

    if ((unsigned char)(srcbuf[0]) == 0x00 
        && (unsigned char)(srcbuf[ba.length() - 1]) == 0xff) {
        ba = QByteArray(srcbuf + 1, ba.length() - 2);
        if (ba.length() > 0) {
            qDebug()<<"ws serv got and broard msg:"<<ba;
            emit this->newWSMessage(ba, sock);
        }
	// websocket close standard command 0xff0x00
    } else if(ba.length() == 2 && 
	      (unsigned char)(srcbuf[0]) == 0xff &&
	      (unsigned char)(srcbuf[1]) == 0x00) {
      qDebug()<<"websocket recieve close request.";
    } else {
        qDebug()<<"Invalid/Uknown ws data frame, omited";
    }

    // ret = decode(srcbuf, ba.length(), (unsigned char*)msgbuf, sizeof(msgbuf));
    // memset(srcbuf, 0, sizeof(srcbuf));
    // memcpy(srcbuf, ba.data() + 1, ba.length() -2 );
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<ret<<srcbuf<<(msgbuf+2)<<msgbuf<<(msgbuf + 1);
    // Q_ASSERT(ret > 0);

//     QFile fp("a.msg");
//     fp.open(QIODevice::ReadWrite);
//     fp.resize(0);
//     fp.write(ba);
//     fp.close();

    // memset(msgbuf, 0, sizeof(msgbuf));
    // strcat(srcbuf, "只整合，说：");
    // snprintf(msgbuf, sizeof(msgbuf) - 3, "%c%s%c", 0x00, srcbuf, 0xff);
    // sock->write(msgbuf, strlen(srcbuf) + 2);
    // this->wssend(sock, srcbuf, strlen(srcbuf));
}


///////////////////////////////////////
///////
///////////////////////////////////////
WebSocketClient::WebSocketClient(QString uri)
    :QObject(0)
{
    this->m_uri = uri;

    this->initNoiseChars();

    this->m_sock = boost::shared_ptr<QTcpSocket>(new QTcpSocket());
    QObject::connect(this->m_sock.get(), SIGNAL(connected()), this, SLOT(on_connected_ws_server()));
    QObject::connect(this->m_sock.get(), SIGNAL(disconnected()), this, SLOT(on_disconnected_ws_server()));
    QObject::connect(this->m_sock.get(), SIGNAL(readyRead()), this, SLOT(on_backend_handshake_ready_read()));
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
    
    return true;
}

bool WebSocketClient::sendMessage(QByteArray msg)
{

    ssize_t wlen = 0;
    bool ok;

    Q_ASSERT(this->m_sock != NULL);

    ok = this->m_sock->putChar(0x00);
    Q_ASSERT(ok);
    // wlen = this->m_sock->write((const char *)buf, len);
    wlen = this->m_sock->write(msg);
    ok = this->m_sock->putChar(0xff);
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

void WebSocketClient::on_disconnected_ws_server()
{

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
    } else {
        qDebug()<<"digest doesn't match:"
                <<"l"<<reply_digest.length()<<"=?"<<this->expected_digest.length()
                << "++"
                << reply_digest << "++!=++" << this->expected_digest << "++";
        // qDebug()<<"digest doesn't match:'"
        //         << reply_digest << "'!='" << this->expected_digest << "'";
        this->m_sock->close();
    }
}

void WebSocketClient::on_backend_ready_read()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QByteArray ba;

    ba = this->m_sock->readAll();

    char srcbuf[5120] = {0};
    // char msgbuf[5120] = {0};
    // int ret;
    
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<this->m_sock<<"read ws message"
            <<ba.length()<<ba;

    //always 00datahereff format ?????????
    memcpy(srcbuf, ba.data(), ba.length());

    if ((unsigned char)(srcbuf[0]) == 0x00 
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
    snprintf(kbuf, sizeof(kbuf)-1, "%u", number * spaces);

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

    md5_buffer(src, ba_src.length(), raw_md5);

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


//////////////////////////
//////////
///////////////////////////
static WebSocketServer2 *user_hook = NULL;
WebSocketServer2::WebSocketServer2(QObject *parent)
    :QThread(parent)
{
    user_hook = this;
    this->serv_ctx = NULL;
    this->quit_srv_loop = false;
    this->outer_conn_seq = 0;
}

WebSocketServer2::~WebSocketServer2()
{
}

void WebSocketServer2::run()
{
    int iret = 0;
    while (!quit_srv_loop) {
        iret = libwebsocket_service(this->serv_ctx, 2000);
        // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
        if (iret == 0) {
            // ok, success
        } else {
            // listen socket deak
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Listen socket maybe dead."
                    <<"Need restart this server.";
        }
    }
}

bool WebSocketServer2::setAllowedHost(QStringList allows)
{
  
    this->allowed_hosts = allows;
  
    return true;
}

qint64 WebSocketServer2::nextPendingConnection()
{
    libwebsocket *backend = NULL;
    qint64 cseq = 0;

    if (this->pending_conns.size() > 0) {
        backend = this->pending_conns.begin().key();
        this->pending_conns.erase(this->pending_conns.begin());

        cseq = ++this->outer_conn_seq;
        this->outer_conns.insert(backend, cseq);
    }

    return cseq;
    // QTcpSocket *p = (QTcpSocket*)backend;
    // return p;
}

unsigned short WebSocketServer2::serverPort()
{
    // return this->ws_serv_sock->serverPort();
    unsigned short port = 0;

	int n;
	int m;
	struct libwebsocket *wsi;
    struct sockaddr_in serv_addr;
    socklen_t addr_size;

	for (n = 0; n < FD_HASHTABLE_MODULUS; n++) {
		for (m = 0; m < this->serv_ctx->fd_hashtable[n].length; m++) {
			wsi = this->serv_ctx->fd_hashtable[n].wsi[m];
            qlog("%d,%d, %p, %d, %d\n", n, m, wsi, wsi->sock, wsi->mode);
            if (wsi->mode == LWS_CONNMODE_SERVER_LISTENER) {

                addr_size = sizeof(serv_addr);
                getsockname(wsi->sock, (struct sockaddr*)&serv_addr, &addr_size);
                port = ntohs(serv_addr.sin_port);
            }
        }
    }
    assert(port != 0);

    // 减少对基础库的改动
    // port = libwebsocket_server_get_port(this->serv_ctx);

    return port;
    return 0;
}

QString WebSocketServer2::serverIpAddr(int type)
{
    QString ipaddr;
    QHostAddress addr;
    QList<QHostAddress> addr_list;
    QList<QString> addr_str_list;
    
    // addr = this->ws_serv_sock->serverAddress();
    // qDebug()<<"ws listen ip addr:"<<addr;
    // if (addr == QHostAddress::Null) {
        
    // } else {
    //     ipaddr = addr.toString();
    // }

    addr_list = QNetworkInterface::allAddresses();
    // qDebug()<<addr_list;
    for (int i = 0 ; i < addr_list.count() ; i ++) {
        addr_str_list.append(addr_list.at(i).toString());
    }
    // qSort(list.begin(), list.end(), qGreater<int>());
    qSort(addr_str_list.begin(), addr_str_list.end(), qGreater<QString>());

    if (addr_str_list.count() == 0) {
    } else if (addr_str_list.count() == 1) {
        // must be 127.0.0.1
        ipaddr = addr_str_list.at(0);
    } else {
        for (int i = 0 ; i < addr_str_list.count(); i ++) {
	  // addr = addr_list.at(i);
	  ipaddr = addr_str_list.at(i);
            if (ipaddr.indexOf(":") != -1) {
                // ipv6 addr
                ipaddr = QString();
                continue;
            } else {
                if (ipaddr.startsWith("127.0")) {
                    ipaddr = QString();
                    continue;
                } else if (!ipaddr.startsWith("172.24.")
                           &&!ipaddr.startsWith("192.168.")
                           &&!ipaddr.startsWith("10.10.")) {
                    // should a big ip addr
		  qDebug()<<"break big ip";
                    break;
                } else if (ipaddr.startsWith("172.24.")) {
		  qDebug()<<"break 172.2";
                    break;
                } else if (ipaddr.startsWith("10.10.")) {
                    break;
                } else if (ipaddr.startsWith("192.168.")) {
                    break;
                } else {
                    // do not want go here
                    Q_ASSERT(1 == 2);
                    break;
                }
            }
            ipaddr = QString();
        }
    }

    return ipaddr;
}

ssize_t WebSocketServer2::recv(qint64 cseq, void *buf, size_t len)
{
    return 0;
}

ssize_t WebSocketServer2::send(qint64 cseq, const void *buf, size_t len)
{
    ssize_t wlen = 0;
    libwebsocket *wsi = this->outer_conns.findRight(cseq).value();
    // Q_ASSERT(sock != NULL);
    Q_ASSERT(cseq != 0);
    // wlen = sock->write((const char *)buf, len);

    if (!this->outer_conns.rightContains(cseq)) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"maybe sess ended already.";        
    } else {
        wlen = libwebsocket_write(wsi, (unsigned char *)buf, len, LWS_WRITE_TEXT);
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<(const char*)buf;
    }

    return wlen;
}

ssize_t WebSocketServer2::wssend(qint64 cseq, const void *buf, size_t len)
{
    ssize_t wlen = 0;
    bool ok;
    char wbuf[1000] = {0};
    libwebsocket *wsi = this->outer_conns.findRight(cseq).value();

    // Q_ASSERT(sock != NULL);
    Q_ASSERT(cseq != 0);

    // ok = sock->putChar(0x00);
    // Q_ASSERT(ok);
    // wlen = sock->write((const char *)buf, len);
    // ok = sock->putChar(0xff);
    // Q_ASSERT(ok);
    // wbuf[0] = 0x00;
    // wbuf[len+1] = 0xff;
    // memcpy(wbuf+1, buf, len);
    
    // wlen = sock->write(wbuf, len+2);

    if (!this->outer_conns.rightContains(cseq)) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"maybe sess ended already.";
    } else {
        wlen = libwebsocket_write(wsi, (unsigned char*)buf, len, LWS_WRITE_TEXT);
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"wlen:"<<wlen<<(const char*)buf;
    }
    return wlen;
}

QString WebSocketServer2::conn_payload_path(qint64 cseq)
{
    QString path;
    libwebsocket *wsi = this->outer_conns.findRight(cseq).value();

    path = this->pending_payload_paths.value(wsi);

    this->pending_payload_paths.remove(wsi);

    return path;
}

QString WebSocketServer2::conn_get_peer_address(qint64 cseq)
{
    QString path;
    libwebsocket *wsi = this->outer_conns.findRight(cseq).value();

    path = "";

    char buf[100] = {0};
    struct sockaddr_in serv_addr;
    socklen_t addr_size;

    addr_size = sizeof(serv_addr);
    ::getsockname(wsi->sock, (struct sockaddr*)&serv_addr, &addr_size);
    const char *pret = ::inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, buf, sizeof(buf)-1);

    path = QString(buf);
    qDebug()<<"get addr from lws wsi conn: "<<path;

    return path;
}

int WebSocketServer2::conn_close(qint64 cseq)
{
    int iret = 0;
    libwebsocket *wsi = this->outer_conns.findRight(cseq).value();

    if (this->outer_conns.rightContains(cseq)) {
        // iret = libwebsocket_write(wsi, NULL, 0, LWS_WRITE_CLOSE);
        libwebsockets_hangup_on_client(this->serv_ctx, 
                                       libwebsocket_get_socket_fd(wsi));
    }

    return 0;
}

int WebSocketServer2::lws_new_connection_established(libwebsocket *wsi)
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
            this->pending_payload_paths.insert(wsi, QByteArray(t->token, t->token_len));
        }
    }
    this->pending_conns.insert(wsi, QByteArray());
    emit newConnection();
    return 0;
}

int WebSocketServer2::lws_connection_closed(libwebsocket *wsi)
{
    // qDebug()<<"backend ws close event:"<<wsi;
    qlog("backend ws close event: %p", wsi);
    qint64 cseq = 0;
    if (this->outer_conns.leftContains(wsi)) {
        cseq = this->outer_conns.findLeft(wsi).value();
        this->outer_conns.removeLeft(wsi);
        emit this->clientSessionClosed(cseq);
    }

    return 0;
}

int WebSocketServer2::lws_ws_message_ready(libwebsocket *wsi, char *msg, size_t len)
{
    int cseq = 0;
    QByteArray nmsg = QByteArray(msg, len);

    cseq = this->outer_conns.findLeft(wsi).value();

    qDebug()<<"ws serv got and broard msg:"<<nmsg<<cseq;

    
    emit this->newWSMessage(nmsg, cseq);
    // emit this->newWSMessage(nmsg, (QTcpSocket*)wsi);
    
    return 0;
}

static int close_testing;

/*
 * This demo server shows how to use libwebsockets for one or more
 * websocket protocols in the same server
 *
 * It defines the following websocket protocols:
 *
 *  dumb-increment-protocol:  once the socket is opened, an incrementing
 *				ascii string is sent down it every 50ms.
 *				If you send "reset\n" on the websocket, then
 *				the incrementing number is reset to 0.
 *
 *  lws-mirror-protocol: copies any received packet to every connection also
 *				using this protocol, including the sender
 */

enum demo_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_DUMB_INCREMENT,
	PROTOCOL_LWS_MIRROR,

    PROTOCOL_LWS_WSO,

	/* always last */
	DEMO_PROTOCOL_COUNT
};


// #define LOCAL_RESOURCE_PATH DATADIR"/libwebsockets-test-server"
// #define LOCAL_RESOURCE_PATH "/etc/""/libwebsockets-test-server"
#define LOCAL_RESOURCE_PATH "/data/TOMSKYPEIVR/xfsdev/kitphone-svn/skyserv"

/* this protocol server (always the first one) just knows how to do HTTP */

static int callback_http(struct libwebsocket_context * context,
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


/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 */

static void
dump_handshake_info(struct lws_tokens *lwst)
{
	int n;
	static const char *token_names[] = {
		/*[WSI_TOKEN_GET_URI]		=*/ "GET URI",
		/*[WSI_TOKEN_HOST]		=*/ "Host",
		/*[WSI_TOKEN_CONNECTION]	=*/ "Connection",
		/*[WSI_TOKEN_KEY1]		=*/ "key 1",
		/*[WSI_TOKEN_KEY2]		=*/ "key 2",
		/*[WSI_TOKEN_PROTOCOL]		=*/ "Protocol",
		/*[WSI_TOKEN_UPGRADE]		=*/ "Upgrade",
		/*[WSI_TOKEN_ORIGIN]		=*/ "Origin",
		/*[WSI_TOKEN_DRAFT]		=*/ "Draft",
		/*[WSI_TOKEN_CHALLENGE]		=*/ "Challenge",

		/* new for 04 */
		/*[WSI_TOKEN_KEY]		=*/ "Key",
		/*[WSI_TOKEN_VERSION]		=*/ "Version",
		/*[WSI_TOKEN_SWORIGIN]		=*/ "Sworigin",

		/* new for 05 */
		/*[WSI_TOKEN_EXTENSIONS]	=*/ "Extensions",

		/* client receives these */
		/*[WSI_TOKEN_ACCEPT]		=*/ "Accept",
		/*[WSI_TOKEN_NONCE]		=*/ "Nonce",
		/*[WSI_TOKEN_HTTP]		=*/ "Http",
	};
	
	for (n = 0; n < WSI_TOKEN_COUNT; n++) {
		if (lwst[n].token == NULL)
			continue;

		fprintf(stderr, "    %s = %s\n", token_names[n], lwst[n].token);
	}
}

/* dumb_increment protocol */

/*
 * one of these is auto-created for each connection and a pointer to the
 * appropriate instance is passed to the callback in the user parameter
 *
 * for this example protocol we use it to individualize the count for each
 * connection.
 */

struct per_session_data__dumb_increment {
	int number;
};

static int
callback_dumb_increment(struct libwebsocket_context * context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	int n;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 +
						  LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	struct per_session_data__dumb_increment *pss = (struct per_session_data__dumb_increment*)user;

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		pss->number = 0;
		break;

	/*
	 * in this protocol, we just use the broadcast action as the chance to
	 * send our own connection-specific data and ignore the broadcast info
	 * that is available in the 'in' parameter
	 */

	case LWS_CALLBACK_BROADCAST:
		n = sprintf((char *)p, "%d", pss->number++);
		n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
		if (n < 0) {
			fprintf(stderr, "ERROR writing to socket");
			return 1;
		}
		if (close_testing && pss->number == 50) {
			fprintf(stderr, "close tesing limit, closing\n");
			libwebsocket_close_and_free_session(context, wsi,
						       LWS_CLOSE_STATUS_NORMAL);
		}
		break;

	case LWS_CALLBACK_RECEIVE:
		fprintf(stderr, "rx %d\n", (int)len);
		if (len < 6)
			break;
		if (strcmp((char*)in, "reset\n") == 0)
			pss->number = 0;
		break;
	/*
	 * this just demonstrates how to use the protocol filter. If you won't
	 * study and reject connections based on header content, you don't need
	 * to handle this callback
	 */

	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
		dump_handshake_info((struct lws_tokens *)(long)user);
		/* you could return non-zero here and kill the connection */
		break;

	default:
		break;
	}

	return 0;
}


/* lws-mirror_protocol */

#define MAX_MESSAGE_QUEUE 64

struct per_session_data__lws_mirror {
	struct libwebsocket *wsi;
	int ringbuffer_tail;
};

struct a_message {
	void *payload;
	size_t len;
};

static struct a_message ringbuffer[MAX_MESSAGE_QUEUE];
static int ringbuffer_head;


static int
callback_lws_mirror(struct libwebsocket_context * context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	int n;
	struct per_session_data__lws_mirror *pss = (struct per_session_data__lws_mirror*)user;

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		pss->ringbuffer_tail = ringbuffer_head;
		pss->wsi = wsi;
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (close_testing)
			break;
		if (pss->ringbuffer_tail != ringbuffer_head) {

			n = libwebsocket_write(wsi, (unsigned char *)
				   ringbuffer[pss->ringbuffer_tail].payload +
				   LWS_SEND_BUFFER_PRE_PADDING,
				   ringbuffer[pss->ringbuffer_tail].len,
								LWS_WRITE_TEXT);
			if (n < 0) {
				fprintf(stderr, "ERROR writing to socket");
				exit(1);
			}

			if (pss->ringbuffer_tail == (MAX_MESSAGE_QUEUE - 1))
				pss->ringbuffer_tail = 0;
			else
				pss->ringbuffer_tail++;

			if (((ringbuffer_head - pss->ringbuffer_tail) %
				  MAX_MESSAGE_QUEUE) < (MAX_MESSAGE_QUEUE - 15))
				libwebsocket_rx_flow_control(wsi, 1);

			libwebsocket_callback_on_writable(context, wsi);

		}
		break;

	case LWS_CALLBACK_BROADCAST:
		n = libwebsocket_write(wsi, (unsigned char*)in, len, LWS_WRITE_TEXT);
		if (n < 0)
			fprintf(stderr, "mirror write failed\n");
		break;

	case LWS_CALLBACK_RECEIVE:

		if (ringbuffer[ringbuffer_head].payload)
			free(ringbuffer[ringbuffer_head].payload);

		ringbuffer[ringbuffer_head].payload =
				malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
						  LWS_SEND_BUFFER_POST_PADDING);
		ringbuffer[ringbuffer_head].len = len;
		memcpy((char *)ringbuffer[ringbuffer_head].payload +
					  LWS_SEND_BUFFER_PRE_PADDING, in, len);
		if (ringbuffer_head == (MAX_MESSAGE_QUEUE - 1))
			ringbuffer_head = 0;
		else
			ringbuffer_head++;

		if (((ringbuffer_head - pss->ringbuffer_tail) %
				  MAX_MESSAGE_QUEUE) > (MAX_MESSAGE_QUEUE - 10))
			libwebsocket_rx_flow_control(wsi, 0);

		libwebsocket_callback_on_writable_all_protocol(
					       libwebsockets_get_protocol(wsi));
		break;
	/*
	 * this just demonstrates how to use the protocol filter. If you won't
	 * study and reject connections based on header content, you don't need
	 * to handle this callback
	 */

	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
		dump_handshake_info((struct lws_tokens *)(long)user);
		/* you could return non-zero here and kill the connection */
		break;

	default:
		break;
	}

	return 0;
}

////
struct per_session_data__wso {
    struct libwebsocket *wsi;
    int seq;
};

static int
callback_wso(struct libwebsocket_context * context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
    int n;
    struct per_session_data__wso *pwso = (struct per_session_data__wso*)user;
    
    switch (reason) {
    case LWS_CALLBACK_ESTABLISHED:
        pwso->seq = 0;
        pwso->wsi = wsi;
        user_hook->lws_new_connection_established(wsi);
        break;

    case LWS_CALLBACK_CLOSED:
        user_hook->lws_connection_closed(wsi);
        break;
    case LWS_CALLBACK_RECEIVE:
        n = user_hook->lws_ws_message_ready(wsi, (char*)in, len);

        break;

    case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
        break;
    }

    return 0;
}

/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
		0			/* per_session_data_size */
	},
	{
		"dumb-increment-protocol",
		callback_dumb_increment,
		sizeof(struct per_session_data__dumb_increment),
	},
	{
		"lws-mirror-protocol",
		callback_lws_mirror,
		sizeof(struct per_session_data__lws_mirror)
	},
	{
		"wso",
		callback_wso,
		sizeof(struct per_session_data__wso)
	},
	{
		NULL, NULL, 0		/* End of list */
	}
};

bool WebSocketServer2::listen(unsigned short port)
{
    // port = 8080;

    this->serv_ctx = libwebsocket_create_context(port, NULL, protocols, NULL, NULL, NULL, -1, -1, 0);
    Q_ASSERT(this->serv_ctx != NULL);

    qlog("server listen port: %d\n", this->serverPort()); 

    this->start();
    
    return true;
}

