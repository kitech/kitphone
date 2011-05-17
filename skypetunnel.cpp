// skypetunnel.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-09-08 14:12:07 +0800
// Version: $Id: skypetunnel.cpp 880 2011-05-16 12:55:07Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include <QFutureWatcher>
#include <QFuture>

#include <boost/signals2.hpp>

#include "simplelog.h"
#include "metauri.h"
#include "skype.h"

#include "skypetunnel.h"

#include "volctl.h"

SkypeTunnel::SkypeTunnel(QObject *parent)
    : QObject(parent)
{
    this->mSkype = NULL;
    this->mStreamReconnectTimer = NULL;
    this->mStreamReconnectRetry = 0;
    this->mycall = false;
    this->mNetAM = new QNetworkAccessManager();
}

SkypeTunnel::~SkypeTunnel()
{

}
SkypeAsyncConnector *aconn = nullptr;
void SkypeTunnel::setSkype(Skype *skype)
{
    Q_ASSERT(skype != NULL);

    if (this->mSkype == skype) {

#ifdef WIN32
        aconn = new SkypeAsyncConnector();
        aconn->mSkype = this->mSkype;
        aconn->start();
#else
        this->mSkype->connectToSkype();
#endif

        // QFuture<bool> future = QtConcurrent::run(boost::bind(&Skype::connectToSkype, this->mSkype));
        // QFutureWatcher<bool> *future_watcher = new QFutureWatcher<bool>();
        // future_watcher->setFuture(future);
        // QObject::connect(future_watcher, SIGNAL(finished()), this, SLOT(onSkypeAsyncConnectFinished()));
    } else {
        this->mSkype = skype;
        QObject::connect(this->mSkype, SIGNAL(skypeError(int, QString, QString)),
                         this, SLOT(onSkypeError(int, QString, QString)));

        QObject::connect(this->mSkype, SIGNAL(newStreamCreated(QString, int)),
                         this, SLOT(onNewStreamCreated(QString, int)));
        
        QObject::connect(this->mSkype, SIGNAL(streamClosed()),
                         this, SLOT(onStreamClosed()));

#ifdef WIN32
        // TODO thread destructure
        // this->mSkype->connectToSkype();
        aconn = new SkypeAsyncConnector();
        aconn->mSkype = this->mSkype;
        aconn->start();
#else
        this->mSkype->connectToSkype();
#endif

        // 在线程中处理skype连接有问题，linux下是程序崩溃，win下同程序不能完全退出。
        // QFuture<bool> future = QtConcurrent::run(boost::bind(&Skype::connectToSkype, this->mSkype));
        // QFutureWatcher<bool> *future_watcher = new QFutureWatcher<bool>();
        // future_watcher->setFuture(future);
        // QObject::connect(future_watcher, SIGNAL(finished()), this, SLOT(onSkypeAsyncConnectFinished()));


        QObject::connect(this->mSkype, SIGNAL(packageArrived(QString, int, QString)),
                         this, SLOT(onSkypePackageArrived(QString, int, QString)));
        QObject::connect(this->mSkype, SIGNAL(newForwardCallArrived(QString, QString, int)),
                         this, SLOT(onNewCallArrived(QString, QString, int)));
        QObject::connect(this->mSkype, SIGNAL(forwardCallAnswered(int, QString, QString)),
                         this, SLOT(onSkypeCallAnswered(int, QString, QString)));

        QObject::connect(this->mSkype, SIGNAL(callHangup(QString, QString, int)),
                         this, SLOT(onCallHangup(QString, QString, int)));
    }
}

void SkypeTunnel::setStreamPeer(QString streamPeerName)
{
    this->mStreamPeerName = streamPeerName;
}

void SkypeTunnel::setPhoneNumber(QString phoneNumber)
{
    this->mPhoneNumber = phoneNumber;
}

void SkypeTunnel::onSkypeAsyncConnectFinished()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QFutureWatcher<bool> *future_watcher = static_cast<QFutureWatcher<bool>*>(sender());
    QFuture<bool> future = future_watcher->future();

    bool connected = this->mSkype->isConnected();
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<connected<<future.results();

    future_watcher->deleteLater();
}

void SkypeTunnel::onSkypeError(int errNo, QString msg, QString cmd)
{
    qDebug()<<errNo<<msg;
}

void SkypeTunnel::onSkypeConnected(QString skypeName)
{
    
}

void SkypeTunnel::onSkypeDisconnected(QString skypeName)
{

}

void SkypeTunnel::onNewStreamCreated(QString contactName, int stream)
{
    if (this->mStreamReconnectTimer != NULL) {
        QTimer *tmer = this->mStreamReconnectTimer;
        this->mStreamReconnectTimer = NULL;
        tmer->stop();
        delete tmer;
    }
    this->mStreamReconnectRetry = 0;
}

void SkypeTunnel::onStreamClosed()
{
    Q_ASSERT(!this->mStreamPeerName.isEmpty());
    this->mSkype->newStream(this->mStreamPeerName);
    if (this->mStreamReconnectTimer == NULL) {
        this->mStreamReconnectTimer = new QTimer();
        QObject::connect(this->mStreamReconnectTimer, SIGNAL(timeout()),
                         this, SLOT(reconnectStream()));
        this->mStreamReconnectTimer->start(3000);
    }
}

void SkypeTunnel::reconnectStream()
{
    qDebug()<<QDateTime::currentDateTime();
    if (this->mStreamReconnectRetry > 5) {
        qDebug()<<"retry connect stream:"<<this->mStreamReconnectRetry;
        // what can i do?
    }
    this->mStreamReconnectRetry ++;
    this->mSkype->newStream(this->mStreamPeerName);
}

void SkypeTunnel::skypeCallFriend(QString router, QString num)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<QDateTime::currentDateTime();
    QString sendUrl;
    QNetworkRequest req;

    this->mycall = true;
    sendUrl = QString("http://202.108.12.211/wso_test/skype_gateway.php?func=notepair&caller_name=%1&callee_phone=%2&rand=%3")
        .arg(this->mSkype->handlerName()).arg(num).arg(qrand());
    
    req.setUrl(sendUrl);
    QNetworkReply *rep = this->mNetAM->get(req);
    this->mPresendResult.insert(rep, QByteArray());
    QObject::connect(rep, SIGNAL(finished()), this, SLOT(onPresendFinished()));
    QObject::connect(rep, SIGNAL(readyRead()), this, SLOT(onPresendReadyRead()));
    rep->setProperty("router", router);
    rep->setProperty("caller_name", this->mSkype->handlerName());
    rep->setProperty("callee_phone", num);

    // move to response
    // this->mSkype->callFriend(router);
}

void SkypeTunnel::onNewCallArrived(QString callerName, QString calleeName, int callID)
{
    qDebug()<<__FILE__<<__LINE__<<calleeName<<callID;
    if (calleeName.startsWith("+")) {
        // int ok = this->mSkype->setCallHold(QString("%1").arg(callID));
        int ok = this->mSkype->setCallHangup(QString("%1").arg(callID));
        // QString ret = this->mSkype->callFriend(contactName);

        QString num = calleeName; // this->mainUI.lineEdit_2->text();
        SkypePackage sp;
        sp.seq = SkypeCommand::nextID().toInt();
        sp.type = SkypePackage::SPT_GW_SELECT;
        sp.data = num;

        QString spStr = sp.toString();
        qDebug()<<spStr;
        ok = this->mSkype->sendPackage("drswinghead", spStr);
    } else {
        // pstn // shound be move to answered
    }
}

void SkypeTunnel::onSkypeCallAnswered(int callID, QString callerName, QString calleeName)
{
    qDebug()<<__FILE__<<__LINE__<<calleeName<<callID;

    // this->mSkype->setMute(true);
    // pstn // shound be move to answered

    // this->mVolume = GetVolume();

    // this->mPhoneNumber = this->mPhoneNumber.right(this->mPhoneNumber.length() - strlen("99008668056") - 1);
    // SetVolume(1);
    // QTimer::singleShot(5000, this, SLOT(restoreVolume()));

    // this->mSkype->setCallDTMF(QString::number(callID), QString("*"));
    // for (int i = 0 ; i < this->mPhoneNumber.length(); ++i) {
    //     this->mSkype->setCallDTMF(QString::number(callID), this->mPhoneNumber.mid(i, 1));
    // }
    // this->mSkype->setCallDTMF(QString::number(callID), QString("#"));        
    // this->mSkype->setMute(false);
    // qDebug()<<"Set to orignal volume:"<<origVol<<SetVolume(origVol);
}

void SkypeTunnel::onCallHangup(QString contactName, QString callerName, int callID)
{
    qLogx()<<contactName<<callID;

    QString num = callerName;
    SkypePackage sp;
    sp.seq = SkypeCommand::nextID().toInt();
    sp.type = SkypePackage::SPT_GW_RELEASE;
    sp.data = num;

    QString spStr = sp.toString();
    qDebug()<<spStr;
    // this->mSkype->sendPackage("drswinghead", spStr);

    this->mycall = false;
}

void SkypeTunnel::onDtmfSended(int callID, QString dtmf)
{
    qDebug()<<__FILE__<<__LINE__<<dtmf<<callID;    
    if (dtmf == "#") {
        // SetVolume(this->mVolume);
        // qDebug()<<"Set to orignal volume:"<<this->mVolume<<SetVolume(this->mVolume);
    }
}

void SkypeTunnel::restoreVolume()
{
    if (GetVolume() == this->mVolume/2) {
        qDebug()<<"Set to orignal volume:"<<this->mVolume<<SetVolume(this->mVolume);
    } else {
        qDebug()<<"Set to orignal volume:"<<this->mVolume<<SetVolume(this->mVolume/2);
        QTimer::singleShot(3000, this, SLOT(restoreVolume()));
    }
}

void SkypeTunnel::onSkypePackageArrived(QString contactName, int stream, QString data)
{
    qDebug()<<contactName<<stream<<data;
    SkypePackage sp = SkypePackage::fromString(data);
    Q_ASSERT(sp.isValid());

    this->processRequest(contactName, stream, &sp);
}

void SkypeTunnel::processRequest(QString contactName, int stream, SkypePackage *sp)
{
    MetaUri mu;
    MetaUri rmu; // response
    SkypePackage rsp; // response package
    QString rspStr;
    QString callee_name;
    QUrl turl;
    QString rcode;

    switch (sp->type) {
    case SkypePackage::SPT_MU_ADD:
        mu = MetaUri::fromString(sp->data);
        // add to storage here;
        mu.dump();
        rsp.seq = sp->seq; // the same as request
        rsp.type = SkypePackage::SPT_MU_ADD_RESP;
        rsp.data = QString("OK");

        rspStr = rsp.toString();
        this->mSkype->sendPackage(contactName, stream, rspStr);
        break;
    case SkypePackage::SPT_MU_DELETE:
        break;
    case SkypePackage::SPT_GW_SELECT:
        qDebug()<<"SPT_GW_SELECT: "<<sp->data;
        rsp.seq = sp->seq;
        rsp.type = SkypePackage::SPT_GW_SELECT_RESULT;
        // rsp.data = QString("drswinghead");   
        rsp.data = sp->data;
        
        rspStr = rsp.toString();
        this->mSkype->sendPackage(contactName, stream, rspStr);
        break;
    case SkypePackage::SPT_GW_SELECT_RESULT:
        turl = QString("http://www.qtchina.net/skype.cgi?%1").arg(sp->data);
        // callee_name = sp->data;
        rcode = turl.queryItemValue("ret");
        callee_name = turl.queryItemValue("gateway");
        qDebug()<<"get a ivr gate way: "<<sp->data<<turl;
        qDebug()<<"ready for call,...";
        // this->mSkype->callFriend(callee_name);
        qDebug()<<"ready for called done";
        break;
    case SkypePackage::SPT_GW_RELEASE_RESULT:
        turl = QString("http://www.qtchina.net/skype.cgi?%1").arg(sp->data);
        rcode = turl.queryItemValue("ret");
        callee_name = turl.queryItemValue("gateway");

        qDebug()<<"released: gateway="<<callee_name;
        break;
    default:
        Q_ASSERT(1==2);
        break;
    };
}

void SkypeTunnel::onPresendFinished()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sender()<<QDateTime::currentDateTime();

    QString router;
    QString arouter; // auto router from server
    QString caller_name;
    QString callee_phone;
    QStringList resp_lines;
    
    QNetworkReply *rep = (QNetworkReply*)sender();
    QByteArray ba = this->mPresendResult.value(rep);

    router = rep->property("router").toString();
    caller_name = rep->property("caller_name").toString();
    callee_phone = rep->property("callee_phone").toString();
    
    this->mPresendResult.remove(rep);

    resp_lines = QString(ba).trimmed().split('\n');
    qDebug()<<"presend response:"<<ba<<rep->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // qDebug()<<resp_lines;
    switch(rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
    case 200:
        if (ba.startsWith("200 ")) {
            if (resp_lines.count() >= 3) {
                arouter = resp_lines.at(2).trimmed();
            }
            if (!arouter.isEmpty()) {
                this->mSkype->callFriend(arouter);
            } else {
                this->mSkype->callFriend(router);
            }
        } else {
            qDebug()<<"presend call pair server faild:";
        }
        break;
    default:
        qDebug()<<"presend call pair network faild:";
        break;
    }
}

void SkypeTunnel::onPresendReadyRead()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sender();

    QNetworkReply *rep = (QNetworkReply*)sender();
    QByteArray ba = rep->readAll();

    this->mPresendResult[rep] += ba;
}

/////////////////////
SkypeAsyncConnector::SkypeAsyncConnector(QObject *parent)
    : QThread(parent) 
{
    this->mSkype = nullptr;
}

SkypeAsyncConnector::~SkypeAsyncConnector()
{

}

void SkypeAsyncConnector::run()
{
    bool bok = this->mSkype->connectToSkype();
    
    this->exec();
}
