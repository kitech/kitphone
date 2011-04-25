#ifndef _SKYPE_H
#define _SKYPE_H

/***************************************************************
 * skype.h
 * @Author:      Jonathan Verner (jonathan.verner@matfyz.cz)
 * @Author:      flyfish (liuguangzhao@users.sf.net)
 * @License:     GPL v2.0 or later
 * @Created:     2008-04-30.
 * @Revision:    $Id: skype.h 179 2010-10-20 08:27:32Z drswinghead $
 * Description:
 * Usage:
 * TODO:
 * CHANGES:
 ***************************************************************/

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

#include "libng/qbihash.h"

#include "skypeobject.h"
#include "skypecommand.h"
#include "skypecommon.h"

// TODO to async mode
// TODO drop blocking mode, has some problem on win32
class Skype : public QObject { 
    Q_OBJECT;
private:
    SkypeCommon sk;

    QString skypeName;
    QString appPrefix;
    QString appName;
    int protocolNumber;
    bool mAttached;
    bool mNamePublished;
    bool mConnected;

    QHash<QString, QByteArray> streams; // 
    // QHash<QString, int> activeStream; // <skype_id, stream_id>
    QHash<int, QString> dataGrams;// <stream_id, udp_data>
    KBiHash<QString, int> activeStreams;  // <skype_id, stream_id>
    QHash<int, SkypeCall> activeCalls; // <call_id, call object>
    QHash<QString, QString> requestResponseSets; // <request id, request cmd line>
    QMutex rrSetsMutex;

    QStringList contacts;
    bool contactsUpToDate;

    bool waitingForResponse;
    QString waitForResponseID;
    QEventLoop localEventLoop;
    QTimer *pingTimer;
    int TimeOut;
    SkypeResponse response;

    bool run_as_server;

protected:
    void readIncomingData(QString contact, int stream);
    bool doCommand(QString cmd, bool blocking = false);
    int waitForResponse ( QString commandID );

public:
    explicit Skype(QString AppName);
    virtual ~Skype();
    bool connectToSkype();
    bool disconnectFromSkype();
    QString handlerName() { return this->skypeName;}
    // bool isAttached() { return this->mAttached; }
    bool isConnected() { return this->mConnected; }
    bool setAutoAway(bool auto_away);
    int setMute(bool mute);
    bool getAudioOut();
    bool setAudioOut(QString what);

    void newStream(QString contact);
    bool writeToStream(QByteArray data, QString contactName); // deprecated
    bool writeToSock(QString contactName, QByteArray data) { return writeToStream( data, contactName ); };
    QByteArray readFromStream(QString contact);
    bool sendPackage(QString contactName, int streamNum, QString data);
    bool sendPackage(QString contactName, QString data);

    QStringList getContacts();
    QString callFriend(QString contactName); // return value???
    int answerCall(QString callID);
    int setCallHold(QString callID);
    int setCallResume(QString callID);
    int setCallHangup(QString callID);
    int setCallMediaInputPort(QString callID, unsigned short port);
    int setCallMediaOutputPort(QString callID, unsigned short port);
    int setCallInputNull(QString callID);
    int setCallOutputNull(QString callID);   
    int setCallInputFile(QString callID, QString file);
    int setCallOutputFile(QString callID, QString file);
    int setCallDTMF(QString callID, QString code);
    int setCallForward(QString callID, QString toh);

    bool isTransferredCall(QString callID);
    QString callPartnerHandlerName(QString callID);
    int getCallIdByPartnerName(QString callerName);

    void clearChatHistory();
    void clearVoiceMailHistory();
    void clearCallHistory();

    // temporary for outer logical
    QStringList mRouters;
    void setSpecifiedRouters(QStringList routers) {
        this->mRouters = routers;
    }
    void setRunAsClient() {
        this->run_as_server = false;
    }
public slots:
    void onCommandRequest(QString cmd);

signals:
    void connected(QString skypeName);
    void realConnected(QString skypeName); // skypeName is real not null
    void connectionLost(QString skypeName);
    void skypeError(int errNo, QString Msg, QString cmd);
    void dataInStream(QString contactName);
    void newStreamCreated(QString contactName, int num);
    void streamClosed();

    void ConnectStatus(QString str_status, int int_status);
    void UserStatus(QString str_status, int int_status);

    void commandRequest(QString cmd);
    // void commandResponse(QString cmd);
    void commandResponse(QString skypeName, QString cmd);

    void packageSent(QString contactName, QString data);
    void packageArrived(QString contactName, int stream, QString data);

    void dtmfSended(int callID, QString callerName, QString calleeName, QString dtmf);
    // void newCallArrived(QString callerName, QString calleeName, int callID);
    void newRouteCallArrived(QString callerName, QString calleeName, QString callee_phone, int callID);
    void newForwardCallArrived(QString callerName, QString calleeName, int callID);
    void newCallArrived(QString callerName, QString calleeName, int callID);
    void onCallAcceptCalleeDone(QString contactName, int callID);
    void callHangup(QString contactName, QString callerName, int callID);
    void routeCallAnswered(int callID, QString caller_name, QString callee_name);
    void routeCallTransferred(int callID, QString caller_name, QString callee_name, QString last_status);
    void routeCallMissed(int callID, QString caller_name, QString callee_name);
    void routeCallRefused(int callID, QString caller_name, QString callee_name);
    void forwardCallAnswered(int callID, QString caller_name, QString callee_name);
    void forwardCallHold(int callID, QString caller_name, QString callee_name);
    void forwardCallUnhold(int callID, QString caller_name, QString callee_name);
    // void callParticipantArrived(int callID, QString caller_name, QString callee_name, QString participant);
    // void forwardCallPstnArrived(int callID, QString caller_name, QString callee_name, QString pstn);

protected slots:
    void onPublished(QString pubName); // only needed for linux, not include win32
    void onConnected(QString skypeName);
    void onDisconnected(QString skypeName);
    void processMessage(const QString &message);
    void processRouteCallMessage(const QString &message, SkypeResponse &cmd);
    void processForwardCallMessage(const QString &message, SkypeResponse &cmd);
    void processAP2APMessage(const QString &message);
    void processChatMessage(const QString &message);
    void processChatMessageMessage(const QString &message);
    void timeOut();
    void ping();
    void onPingTimeOut();

private:
    bool attachToSkype();
    bool publishToSkype();
};



#endif /* _SKYPE_H */
