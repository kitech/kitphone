// metauri.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-02 11:02:28 +0800
// Version: $Id: metauri.h 167 2010-09-10 10:06:58Z drswinghead $
// 

#ifndef _METAURI_H_
#define _METAURI_H_

#include <QtCore>

// MetaLink
class MetaUri 
{
public:
    MetaUri();
    virtual ~MetaUri();

    void dump();
    QString toString();
    static MetaUri fromString(QString str);
    bool match(MetaUri *mu);
    
public:
    QString url;
    QString nameMd5;
    QString contentMd5;
    quint64 fileSize;
    QString owner;
    bool valid;
    time_t mtime;
};

class SkypePackage
{
public:
    SkypePackage() {
        this->seq = -1;
        this->type = -1;
    }
    virtual ~SkypePackage() { }

    bool isValid();
    QString toString();
    static SkypePackage fromString(QString str);

public:
    enum {
        SPT_MIN = 100,
        SPT_PING,
        SPT_PONG,
        SPT_MU_ADD,
        SPT_MU_ADD_RESP,
        SPT_MU_UPDATE,
        SPT_MU_UPDATE_RESP,
        SPT_MU_DELETE,
        SPT_MU_DELETE_RESP,
        SPT_MU_SEARCH,
        SPT_MU_SEARCH_RESULT,
        SPT_STREAM_CLOSE,
        SPT_STREAM_CLOSE_RESULT,
        SPT_GW_SELECT,
        SPT_GW_SELECT_RESULT,
        SPT_GW_RELEASE,
        SPT_GW_RELEASE_RESULT,
        SPT_MAX = 999
    };

    enum {
        SPR_MIN = 100,
        SPR_OK,
        SPR_MAX = 999
    };

    int seq;
    int type;
    QString data;
};

#endif /* _METAURI_H_ */
