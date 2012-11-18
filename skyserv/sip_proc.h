// sip_proc.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-30 23:18:43 +0800
// Version: $Id$
// 

#ifndef _SIP_PROC_H_
#define _SIP_PROC_H_

extern bool quit_sip_proc;

void *sip_main_proc(void *args);

char *new_rpc_command(int cmdno, int *cmdlen, const char *fmt, ...);

#endif /* _SIP_PROC_H_ */
