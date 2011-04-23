// sip_entry.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-25 11:38:04 +0800
// Version: $Id$
// 

#ifndef _SIP_ENTRY_H_
#define _SIP_ENTRY_H_

int init_sip_app_contex();
int set_sip_call_sample_rate(int rate);
int sip_call_phone(void *ctx, char *arg_str);
int check_pjsip_config_validation();

#endif /* _SIP_ENTRY_H_ */
