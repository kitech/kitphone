// libwebsockets_extra.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-04 07:17:49 +0000
// Version: $Id$
// 

#ifndef _LIBWEBSOCKETS_EXTRA_H_
#define _LIBWEBSOCKETS_EXTRA_H_

#include "libwebsockets.h"

////////////////////////////////////
////// first wrapper for payloadable context
struct libwebsocket_context * libwebsocket_create_context_ex (int port, const char * interf, struct libwebsocket_protocols * protocols, struct libwebsocket_extension * extensions, const char * ssl_cert_filepath, const char * ssl_private_key_filepath, int gid, int uid, unsigned int options, void *user_data);

void *libwebsocket_context_user_data(libwebsocket_context *ctx);

unsigned short libwebsocket_context_server_port(libwebsocket_context *ctx);

std::string libwebsocket_get_request_path(libwebsocket *wsi);

std::string libwebsocket_get_peer_address(libwebsocket *wsi);

std::string libwebsocket_get_cpath(libwebsocket *wsi);

int libwebsocket_get_connect_state(libwebsocket *wsi);

bool libwebsocket_client_is_connected(libwebsocket *wsi);

#endif /* _LIBWEBSOCKETS_EXTRA_H_ */
