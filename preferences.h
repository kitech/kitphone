// preferences.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-14 09:15:17 +0000
// Version: $Id$
// 

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <iostream>

#include <QHash>
#include <QString>
#include <QSqlRecord>

#include <boost/signals2.hpp>
#include <boost/smart_ptr.hpp>

class SqlRequest;
class AsyncDatabase;


class Preferences 
{
public:
    Preferences();
    virtual ~Preferences();

    void reset();
    void save();
    void load();

    // database
    void save(boost::shared_ptr<AsyncDatabase> &m_adb);
    void load(boost::shared_ptr<AsyncDatabase> &m_adb);

    // boost slot
    void saveDone(boost::shared_ptr<SqlRequest> req);
    void loadDone(boost::shared_ptr<SqlRequest> req);

    bool setCodecs(const std::vector<std::string> &codecs);

    enum SipProto {
        SP_MIN = 0,
        SP_UDP,
        SP_TCP,
        SP_TLS,
        SP_STCP,
        SP_IPV6,
        SP_UDP6,
        SP_TCP6,
        SP_MAX
    };

    ////// vars
    bool use_tls;
    int sip_protocol;
    bool use_stun;
    bool use_turn;
    bool use_ice;
    std::string stun_server; // TODO, support multi stun servers
    std::vector<std::string> media_codecs;
    std::string use_codec;
    
    enum NetworkVoiceEngine {
        NVE_MIN = 0,
        NVE_PJSIP,
        NVE_VOPP,
        NVE_SKYCIT,
        NVE_SKYKIT,
        NVE_MAX
    };
    int nve_type;
    bool skype_mode;
};

#endif /* _PREFERENCES_H_ */
