// wryip.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-13 22:39:01 +0800
// Version: $Id$
// 


#ifndef _WRYIP_H_
#define _WRYIP_H_

int searchIPCountry(const char *ipaddr, char *country);
int searchIPProvince(const char *ipaddr, char *province);
int searchIPCity(const char *ipaddr, char *province, char *city);

#endif /* _WRYIP_H_ */
