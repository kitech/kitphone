// libwebsockets_extra.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-04 07:18:37 +0000
// Version: $Id$
// 

#include <assert.h>
#include <string>

#include "private-libwebsockets.h"
#include "libwebsockets_extra.h"

////// first wrapper for payloadable context
struct libwebsocket_context * libwebsocket_create_context_ex (int port, const char * interf, struct libwebsocket_protocols * protocols, struct libwebsocket_extension * extensions, const char * ssl_cert_filepath, const char * ssl_private_key_filepath, int gid, int uid, unsigned int options, void *user_data)
{
    struct libwebsocket_context *ctx = NULL;

    ctx = libwebsocket_create_context(port, interf, protocols, extensions, ssl_cert_filepath, ssl_private_key_filepath, gid, uid, options);
    if (ctx != NULL) {
        ctx->user_data = user_data;
    }

    return ctx;
}

void *libwebsocket_context_user_data(libwebsocket_context *ctx)
{
    if (ctx != NULL) 
        return ctx->user_data;

    return NULL;
}

unsigned short libwebsocket_context_server_port(libwebsocket_context *ctx)
{
    unsigned short port = 0;

	int n;
	int m;
	struct libwebsocket *wsi;
    struct sockaddr_in serv_addr;
    socklen_t addr_size;

    // qLogx()<<FD_HASHTABLE_MODULUS;
	for (n = 0; n < FD_HASHTABLE_MODULUS; n++) {
		for (m = 0; m < ctx->fd_hashtable[n].length; m++) {
			wsi = ctx->fd_hashtable[n].wsi[m];


            addr_size = sizeof(serv_addr);
            ::getsockname(wsi->sock, (struct sockaddr*)&serv_addr, &addr_size);
            port = ntohs(serv_addr.sin_port);

            // qLogx()<<n<<m<<wsi<<wsi->sock<<wsi->mode<<port;

            if (wsi->mode == LWS_CONNMODE_SERVER_LISTENER) {
               break;
            } else {
                port = 0;
            }
            port = 0;
        }
        if (port != 0) {
            break;
        }
    }
    assert(port != 0);

    // 减少对基础库的改动
    // port = libwebsocket_server_get_port(this->serv_ctx);

    return port;

}

std::string libwebsocket_get_request_path(libwebsocket *wsi)
{
    std::string req_path;
    struct lws_tokens *t;
    for (int i = 0 ; i < WSI_TOKEN_COUNT; i++) {
        t = &wsi->utf8_token[i];
        if (t->token_len == 0) {
            break;
        }
        fprintf(stderr, "dumping hdr: %s, %d\n", t->token, t->token_len);
        if (i == WSI_TOKEN_GET_URI) {
            // should be c_path token
            req_path = std::string(t->token, t->token_len);
            // this->pending_payload_paths.insert(wsi, QByteArray(t->token, t->token_len));
        }
    }
    return req_path;
}

std::string libwebsocket_get_peer_address(libwebsocket *wsi)
{
    std::string addr;

    char buf[100] = {0};
    struct sockaddr_in serv_addr;
    socklen_t addr_size;

    if (wsi) {
        addr_size = sizeof(serv_addr);
        ::getsockname(wsi->sock, (struct sockaddr*)&serv_addr, &addr_size);
        const char *pret = ::inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, buf, sizeof(buf)-1);
    }
    addr = std::string(buf);

    return addr;
}

std::string libwebsocket_get_cpath(libwebsocket *wsi)
{
    std::string cpath;

    cpath = std::string(wsi->c_path);

    return cpath;
}

int libwebsocket_get_connect_state(libwebsocket *wsi)
{
    return wsi->state;
}

bool libwebsocket_client_is_connected(libwebsocket *wsi)
{
    if (//wsi->state == WSI_STATE_CLIENT_UNCONNECTED
        // || 
        wsi->state == WSI_STATE_DEAD_SOCKET) {
        return false;
    }

    return true;
}

