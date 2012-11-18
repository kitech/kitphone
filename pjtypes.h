// pjtypes.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-03 09:40:34 +0000
// Version: $Id$
// 

#ifndef _PJTYPES_H_
#define _PJTYPES_H_

#include <QObject>
#include <QString>

// pjsip 数据类型到qt类型的转换，或者互相转换工具函数
// 使用inline

extern "C" {
	#include <pjlib.h>
	#include <pjlib-util.h>
	#include <pjmedia.h>
	#include <pjmedia-codec.h>
	#include <pjsip.h>
	#include <pjsip_simple.h>
	#include <pjsip_ua.h>
	#include <pjsua-lib/pjsua.h>
}

inline QString PJSTR2Q(pj_str_t str) {
    return QString::fromAscii(str.ptr, str.slen);
}

inline char *PJSTR2A(pj_str_t str) {
    if (str.ptr[str.slen] != '\0')
        str.ptr[str.slen] =  '\0';
    return str.ptr;
}

inline std::string PJSTR2S(pj_str_t str) {
    return std::string(str.ptr, str.slen);
}

#endif /* _PJTYPES_H_ */
