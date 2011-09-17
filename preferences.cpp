// preferences.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-14 09:15:31 +0000
// Version: $Id$
// 

#include "simplelog.h"
#include "asyncdatabase.h"

#include "mosipphone.h"

#include "preferences.h"

Preferences::Preferences()
{
    this->reset();
}

Preferences::~Preferences()
{

}


void Preferences::reset()
{
    this->use_tls = false;
    this->sip_protocol = SP_UDP;

    this->use_stun = false;
    this->use_turn = false;
    this->use_ice = false;
    this->stun_server = "";

    this->me_type = ME_PJSIP;
    this->skype_mode = false;
}

void Preferences::save()
{
}

void Preferences::load()
{

}

void Preferences::save(boost::shared_ptr<AsyncDatabase> &m_adb)
{
    
}

void Preferences::load(boost::shared_ptr<AsyncDatabase> &m_adb)
{

}

void Preferences::saveDone(boost::shared_ptr<SqlRequest> req)
{

}

void Preferences::loadDone(boost::shared_ptr<SqlRequest> req)
{

}



/////////////////////
// TODO escape string
bool MosipPhone::savePreferences(Preferences *prefs)
{
    Q_ASSERT(prefs != NULL);

    QString str;
    QStringList sqls;

    const QString tpl = "INSERT OR REPLACE INTO kp_options (key, value) VALUES ('%1', '%2')";

    sqls << QString(tpl).arg("use_tls").arg(prefs->use_tls);
    sqls << QString(tpl).arg("sip_protocol").arg(prefs->sip_protocol);
    sqls << QString(tpl).arg("use_stun").arg(prefs->use_stun);
    sqls << QString(tpl).arg("use_turn").arg(prefs->use_turn);
    sqls << QString(tpl).arg("use_ice").arg(prefs->use_ice);

    str = this->m_adb->escapseString(QString::fromStdString(prefs->stun_server));
    // sqls << QString(tpl).arg("stun_server").arg(prefs->stun_server.c_str());
    sqls << QString(tpl).arg("stun_server").arg(str);

    sqls << QString(tpl).arg("use_codec").arg(QString::fromStdString(prefs->use_codec));

    sqls << QString(tpl).arg("me_type").arg(prefs->skype_mode);
    sqls << QString(tpl).arg("skype_mode").arg(prefs->skype_mode);
 
    qLogx()<<sqls;
    
    boost::shared_ptr<SqlRequest> reqx(new SqlRequest());
    {
        // get accounts list
        reqx->mCbFunctor = boost::bind(&MosipPhone::onSavePreferencesDone, this, _1);
        reqx->mCbObject = this;
        reqx->mCbSlot = SLOT(onSavePreferencesDone(boost::shared_ptr<SqlRequest>));
        // reqx->mSql = QString("SELECT * FROM kp_options WHERE 1=1 ORDER BY key DESC");
        reqx->mSqls = sqls;
        reqx->mReqno = this->m_adb->execute(reqx->mSqls);
        this->mRequests.insert(reqx->mReqno, reqx);
    }

    return true;
}

bool MosipPhone::loadPreferences(Preferences *prefs)
{
    Q_ASSERT(prefs != NULL);

    boost::shared_ptr<SqlRequest> reqx(new SqlRequest());
    {
        // get accounts list
        reqx->mCbFunctor = boost::bind(&MosipPhone::onLoadPreferencesDone, this, _1);
        reqx->mCbObject = this;
        reqx->mCbSlot = SLOT(onLoadPreferencesDone(boost::shared_ptr<SqlRequest>));
        reqx->mSql = QString("SELECT * FROM kp_options WHERE 1=1 ORDER BY key DESC");
        // reqx->mSql = sqls;
        reqx->mCbData = prefs;
        reqx->mReqno = this->m_adb->execute(reqx->mSql);
        this->mRequests.insert(reqx->mReqno, reqx);
    }

    return true;
}

bool MosipPhone::onSavePreferencesDone(boost::shared_ptr<SqlRequest> req)
{
    qLogx()<<"";

    return true;
}

bool MosipPhone::onLoadPreferencesDone(boost::shared_ptr<SqlRequest> req)
{
    qLogx()<<"";

    Preferences *prefs = (Preferences*)(req->mCbData);
    QSqlRecord rec;
    for (int i = 0; i < req->mResults.count(); ++i) {
        rec = req->mResults.at(i);
        
        if (rec.value("key").toString() == "use_tls") {
            prefs->use_tls = rec.value("value").toBool();
        } else 
        if (rec.value("key").toString() == "sip_protocol") {
            prefs->sip_protocol = rec.value("value").toInt();
        } else
        if (rec.value("key").toString() == "use_stun") {
            prefs->use_stun = rec.value("value").toBool();
        } else 
        if (rec.value("key").toString() == "use_turn") {
            prefs->use_turn = rec.value("value").toBool();
        } else 
        if (rec.value("key").toString() == "use_ice") {
            prefs->use_ice = rec.value("value").toBool();
        } else
        if (rec.value("key").toString() == "stun_server") {
            prefs->stun_server = rec.value("value").toString().toStdString();
        } else 
        if (rec.value("key").toString() == "use_codec") {
            prefs->use_codec = rec.value("value").toString().toStdString();
        } else 
            if (rec.value("key").toString() == "me_type") {
                prefs->me_type = rec.value("value").toInt();
            } else 
        if (rec.value("key").toString() == "skype_mode") {
            prefs->skype_mode = rec.value("value").toBool();
        }
    }

    return true;
}

bool Preferences::setCodecs(const std::vector<std::string> &codecs)
{
    this->media_codecs = codecs;
    return true;
}
