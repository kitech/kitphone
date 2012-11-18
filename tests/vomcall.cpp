// vomcall.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-16 22:25:16 +0800
// Version: $Id: vomcall.cpp 583 2010-11-24 04:05:37Z drswinghead $
// 

/*
  usage:
  -p skport
  -u loginskype
  -w loginpassword
  -n phonenumber
  -r router
  -v
  -h
 */

#include "getopt_pp_standalone.h"

#include "skype-embedded_2.h"
#include <sidg_skylibrefs.hpp>

#include "skaccount.h"
#include "skconversation.h"
#include "skparticipant.h"
#include "skypekit.h"
#include "app_token.h"

#include "vomcall.h"

#define SK_APP_TOKEN "xDbixo3eTW"

VomCall::VomCall(int pArgc, char **pArgv, QObject *parent)
    : QObject(parent)
{
    this->mArgc = pArgc;
    this->mArgv = pArgv;

    this->mycall = false;
    this->mNetAM = new QNetworkAccessManager();
}

VomCall::~VomCall()
{

}

SEString inetAddr               = "127.0.0.1";
uint portNum                    = 8963;

static void printHelp()
{
    printf("Usage: \n");
}
bool VomCall::parseArgs()
{
    using namespace GetOpt;

    uint port = 0;
    std::string login_name;
    std::string login_password;
    std::string phone_number;
    std::string router;
    bool help;
    bool verbose;

    GetOpt_pp ops(this->mArgc, this->mArgv);
    ops >> Option('p', "port", port);
    ops >> Option('u', "username", login_name);
    ops >> Option('w', "password", login_password);
    ops >> Option('n', "phone", phone_number);
    ops >> Option('r', "router", router);

    ops >> Option('h', help);
    ops >> Option('v', verbose);

    if (port == 0 || login_name.empty() || login_password.empty()
        || phone_number.empty() || router.empty()) {
        return false;
    }

    this->mPort = port;
    this->mLoginName = login_name;
    this->mLoginPassword = login_password;
    this->mPhoneNumber = phone_number;
    this->mRouter = router;

    return true;
}

bool VomCall::login()
{
    if (!this->parseArgs()) {
        qDebug()<<"Error invalid command line params";
        exit(-1);
    }

    bool ok;
    int ret;
    SEString MyAccountName = "drswinghead";
    SEString MyAccountPsw = "xxxxxxx";
    MyAccount *pacc = NULL;

    printf("Creating skype ..\n");
    this->mSkykit = new MySkype();
    portNum = this->mPort;
    ret = this->mSkykit->init(SK_APP_TOKEN, inetAddr, portNum, "streamlog.txt");
    if (ret == 0) {
        // ok
    } else if (ret == 3) {
        // skypekit instance not exists
    } else {
        // dont known now
    }
    this->mSkykit->start();

    MyAccountName = this->mLoginName.c_str();
    MyAccountPsw = this->mLoginPassword.c_str();
    printf("Getting account ..\n");
    if (this->mSkykit->GetAccount(MyAccountName, this->acc)) {
        printf("Logging in..\n");
        pacc = this->acc->self();
        QObject::connect(pacc, SIGNAL(loginCompelete()),
                         this, SLOT(onLoginComplete()));

        ret = this->acc->LoginWithPassword(MyAccountPsw, false, false);
        qDebug()<<"login return:"<<ret;
        if (ret == 0) {
            // ok
        } else if (ret == 1) {
            // login faild
        } else {
            // dont known what error
        }
        // while (!LoggedIn) { Delay(1); }; // Loop until LoggedIn
        // while (!LoggedOut) { Delay(1); }; // Loop until LoggedOut
    } else {
        printf("Account does not exist\n");
    }
   
    return true;
}

bool VomCall::logout()
{
    printf("Logging out..\n");
    this->acc->Logout(false);

    return true;
}

void VomCall::onLoginComplete()
{
    // this->callSkype(this->mCallTarget.c_str());

    QString sendUrl;
    QNetworkRequest req;

    this->mycall = true;
    sendUrl = QString("http://202.108.12.211/ftsearch/skype_gateway.php?func=notepair&caller_name=%1&callee_phone=%2&rand=%3")
        .arg(this->mLoginName.c_str()).arg(this->mPhoneNumber.c_str()).arg(qrand());
    
    req.setUrl(sendUrl);
    QNetworkReply *rep = this->mNetAM->get(req);
    this->mPresendResult.insert(rep, QByteArray());
    QObject::connect(rep, SIGNAL(finished()), this, SLOT(onPresendFinished()));
    QObject::connect(rep, SIGNAL(readyRead()), this, SLOT(onPresendReadyRead()));
    rep->setProperty("router", this->mRouter.c_str());
    rep->setProperty("caller_name", this->mLoginName.c_str());
    rep->setProperty("callee_phone", this->mPhoneNumber.c_str());
}

bool VomCall::callSkype(const char *pSkypeName)
{
    MyConversation::Ref convs;
    SEStringList CallTargets;
    ParticipantRefs ConvParticipantList;
    SEString MyCallTarget;
    int ret;
    
    MyCallTarget = "yat-sen";
    MyCallTarget = pSkypeName;
    CallTargets.append(MyCallTarget);
    ret = this->mSkykit->GetConversationByParticipants(CallTargets, convs, true, true);
    qDebug()<<"get convs:"<<ret;

    ret = convs->GetParticipants(ConvParticipantList, Conversation::ALL);
    qDebug()<<"get participant:"<<ret;

    for (unsigned int i = 0; i < ConvParticipantList.size(); i++) {
        if (ConvParticipantList[i]->GetProp(Participant::P_IDENTITY) == MyCallTarget) {
            qDebug()<<"Calling "<<(const char*)MyCallTarget;
            ret = ConvParticipantList[i]->Ring(MyCallTarget, false, 0, 10, false);
            break;
        }  
    }

    return true;
}
/*
    QString sendUrl;
    QNetworkRequest req;

    this->mycall = true;
    sendUrl = QString("http://202.108.12.211/ftsearch/skype_gateway.php?func=notepair&caller_name=%1&callee_phone=%2&rand=%3")
        .arg(this->mSkype->handlerName()).arg(num).arg(qrand());
    
    req.setUrl(sendUrl);
    QNetworkReply *rep = this->mNetAM->get(req);
    this->mPresendResult.insert(rep, QByteArray());
    QObject::connect(rep, SIGNAL(finished()), this, SLOT(onPresendFinished()));
    QObject::connect(rep, SIGNAL(readyRead()), this, SLOT(onPresendReadyRead()));
    rep->setProperty("router", router);
    rep->setProperty("caller_name", this->mSkype->handlerName());
    rep->setProperty("callee_phone", num);

 */
void VomCall::onPresendFinished()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sender()<<QDateTime::currentDateTime();

    QString router;
    QString caller_name;
    QString callee_phone;
    
    QNetworkReply *rep = (QNetworkReply*)sender();
    QByteArray ba = this->mPresendResult.value(rep);

    router = rep->property("router").toString();
    caller_name = rep->property("caller_name").toString();
    callee_phone = rep->property("callee_phone").toString();
    
    this->mPresendResult.remove(rep);

    qDebug()<<"presend response:"<<ba<<rep->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    switch(rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
    case 200:
        if (ba.startsWith("200 ")) {
            // this->mSkype->callFriend(router);
            this->callSkype(this->mRouter.c_str());
        } else {
            qDebug()<<"presend call pair server faild:";
        }
        break;
    default:
        qDebug()<<"presend call pair network faild:";
        break;
    }
}

void VomCall::onPresendReadyRead()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sender();

    QNetworkReply *rep = (QNetworkReply*)sender();
    QByteArray ba = rep->readAll();

    this->mPresendResult[rep] += ba;
}
