// websocket_proc.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-09 15:11:26 +0800
// Version: $Id$
// 

#ifndef _WEBSOCKET_PROC_H_
#define _WEBSOCKET_PROC_H_

void *websocket_server_proc(void *args);
int websocket_send_data(int fd, char *data, int len);

#endif /* _WEBSOCKET_PROC_H_ */
