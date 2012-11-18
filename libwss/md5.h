// md5.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-22 16:32:54 +0800
// Version: $Id$
// 
#ifndef _MD5_H_
#define _MD5_H_

#ifdef __cplusplus
extern "C" {
#endif

void
MD5(const unsigned char *input, int ilen, unsigned char *output);

#ifdef __cplusplus
};
#endif

#endif /* _MD5_H_ */
