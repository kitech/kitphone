/***************************************************************
 * skype.cpp
 * @Author:      Jonathan Verner (jonathan.verner@matfyz.cz)
 * @Author:      flyfish (liuguangzhao@users.sf.net)
 * @License:     GPL v2.0 or later
 * @Created:     2008-04-30.
 * @Revision:    $Id: skype.cpp 179 2010-10-20 08:27:32Z drswinghead $
 * Description:
 * Usage:
 * TODO:
 * CHANGES:
 ***************************************************************/
#include <QtCore>
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QTimer>


#include "skycit.h"
#include "skypecommand.h"

Skycit::Skycit(QString AppName)
  : appPrefix(AppName) 
{
    this->mAttached = false;
    this->mNamePublished = false;
    this->mConnected = false;
    this->waitingForResponse = false;
    this->run_as_server = true; // 默认是服务器模式

    TimeOut = 10000;
    pingTimer = new QTimer(this);
    contactsUpToDate = false;

    QObject::connect(this, SIGNAL(connected(QString)), this, SLOT(onConnected(QString)));
    QObject::connect(this, SIGNAL(connectionLost(QString)), this, SLOT(onDisconnected(QString)));
    QObject::connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));

    QObject::connect(&sk, SIGNAL(skypeNotFound()), this, SIGNAL(skypeNotFound()));
    QObject::connect(&sk, SIGNAL(newMsgFromSkype(const QString)),
                     this, SLOT(processMessage(const QString)));
}

Skycit::~Skycit()
{
    if (this->pingTimer != NULL) {
        delete this->pingTimer;
        this->pingTimer = 0;
    }
}

QStringList Skycit::getContacts() {
    if (this->mConnected) {
        if (contactsUpToDate) return contacts;
        doCommand( SkypeCommand::GET_CONTACT_LIST());
        return contacts;
    }
    return contacts;
}


void Skycit::timeOut() {
    qDebug() << "Timeout while waiting for event #"<<waitForResponseID;
    localEventLoop.exit(2);
}

void Skycit::ping() { 
    // sk.sendMsgToSkype( SkypeCommand::PING() );
    // check if last ping has response
    QString key;
    QString value;

    this->rrSetsMutex.lock();
    QHash<QString,QString>::iterator it;
    for (it = this->requestResponseSets.begin(); it != this->requestResponseSets.end(); it++) {
        key = it.key();
        value = it.value();
        if (value.right(5) == " PING") {
            qDebug()<<"may be ping timeout, skype instance dispearred";
            // this->requestResponseSets.remove(key);  // will crash
            // this->requestResponseSets.erase(it);
            break;
        }
    }
    if (it != this->requestResponseSets.end()) {
        this->requestResponseSets.erase(it);
    }
    this->rrSetsMutex.unlock();
    this->doCommand(SkypeCommand::PING());
}

void Skycit::onPingTimeOut()
{
    
}

void Skycit::readIncomingData(QString contactName, int streamNum) {
    sk.sendMsgToSkype( SkypeCommand::READ_AP2AP(appName, contactName, streamNum) );
}

void Skycit::processMessage(const QString &message) {
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    // QString u8msg = codec->toUnicode(message.toAscii());
    qDebug() <<__FILE__<<__LINE__<< "SKYPE: <=" << message; // <<QDateTime::currentDateTime(); // << u8msg;
    emit this->commandResponse(this->skypeName, message);

    SkypeResponse cmd;
    QString requestID;
    QString requestCmd;

    requestID = SkypeCommand::getID(message);

    this->rrSetsMutex.lock();
    if (this->requestResponseSets.contains(requestID)) {
        requestCmd = this->requestResponseSets.value(requestID);
        this->requestResponseSets.remove(requestID);
    }
    this->rrSetsMutex.unlock();

    if ( ! cmd.parse(message) ) {
        emit skypeError( -1, "Error parsing Skype output", requestCmd);
        if ( waitingForResponse ) localEventLoop.exit(1);
        return;
    }

    if ( cmd.type() == SK_CONTACT_LIST ) {
        contacts = cmd.getContacts();
        contactsUpToDate = true;
    }

    if ( waitingForResponse && cmd.responseID() == waitForResponseID ) {
        qDebug() << "Received event "<<message<< cmd.responseID() <<" we've been waiting for.";
        // qDebug() << "Response received:"<<message;
        localEventLoop.exit(0);

        if (!this->mConnected) {
            if (cmd.type() == SK_CONNSTATUS) {
                if (cmd.statusType() == SS_ONLINE) {
                    emit this->connected(this->skypeName);
                } else if(cmd.statusType() == SS_OFFLINE) {
                    emit this->connectionLost(this->skypeName);
                }
            }
        }
        if (!this->mConnected && cmd.type() == SK_OK) {
            emit this->connected(this->skypeName);
        }

        return;
    }

    if (cmd.type() == SK_OK) {
        if (requestCmd.split(" ").at(1) == "NAME") {
            this->onPublished(requestCmd.split(" ").at(2));
        }
        return;
    }
    if ( cmd.type() == SK_UNKNOWN ) { 
        qDebug()<<__FILE__<<__LINE__<<"UNKNOWN cmd type:"<<cmd.data();
        return;
    }

    if ( cmd.type() == SK_ERROR ) { 
        emit skypeError(cmd.errorCode(), cmd.errorMsg(), requestCmd);
        return;
    }
    // if ( cmd.appName() != appName ) return;

    if (cmd.type() == SK_PROTOCOL) {
        this->protocolNumber = cmd.protocolNum();
        if (!this->mConnected) {
            emit this->connected(this->skypeName);
        }
        return;
    }

    if (cmd.type() == SK_CURRENTUSERHANDLE) {
        this->appName = "skynet_" + this->appPrefix;// + "_" + cmd.contactName();
        qDebug()<<"unique appName:"<<this->appName;
        this->skypeName = cmd.contactName();

        emit this->realConnected(cmd.contactName());

        bool ok = doCommand( SkypeCommand::CREATE_AP2AP(appName) );
        Q_ASSERT(ok);
        return;
    }

    if (this->run_as_server) {
        // 在桌面上作为客户端使用时不能这么用，否则用户无法聊天发信息了。
        if (cmd.type() == SK_CHAT) {
            this->doCommand(SkypeCommand::ALTER_CHAT_PROP(cmd.chat.NAME, "LEAVE", ""));
            return;
        }

        if (cmd.type() == SK_CHATMESSAGE) {
            if (cmd.chatMessage.STATUS == CMS_RECEIVED) {
                this->doCommand(SkypeCommand::SET_CHATMESSAGE_PROP(cmd.chatMessage.ID, "SEEN", ""));
            }
            return;
        }
    }

    if ( cmd.type() == SK_READY_TO_READ ) {
        readIncomingData( cmd.contactName(), cmd.streamNum() );
        return;
    }

    if ( cmd.type() == SK_DATA ) {
        if ( streams.contains( cmd.contactName() ) ) {
            streams[cmd.contactName()].append( cmd.data() );
        } else { // should not happen (a SK_STREAMS message should always arrive before)
            qDebug() << "ASSERT: Data arriving before stream Created (" << cmd.contactName() <<":"<< cmd.streamNum() << cmd.data() << ")"; 
            streams[cmd.contactName()] = cmd.data();
            this->activeStreams.insert(cmd.contactName(), cmd.streamNum());
        }
        emit dataInStream( cmd.contactName() );
        return;
    }

    if (cmd.type() == SK_DATAGRAM) {
        // qDebug()<<"Got udp package";
        if (this->dataGrams.contains(cmd.streamNum())) {
            this->dataGrams[cmd.streamNum()] = cmd.data();
        } else {
            this->activeStreams.insert(cmd.contactName(), cmd.streamNum());
            this->dataGrams[cmd.streamNum()] = cmd.data();
        }
        emit this->packageArrived(cmd.contactName(), cmd.streamNum(), cmd.data());
        return;
    }

    if ( cmd.type() == SK_STREAMS ) {
        QByteArray data;
        if (cmd.contactName().isEmpty()) {
        } else {
            if (! streams.contains( cmd.contactName() ) ) {
                streams.insert( cmd.contactName(), data );
            }
            this->activeStreams.insert(cmd.contactName(), cmd.streamNum());
            emit newStreamCreated(cmd.contactName(), cmd.streamNum());
        }
        return;
    }
    // detect incoming router or forwarding call
    if (cmd.type() == SK_CALL) {
        // if (this->handlerName() == "liuguangzhao01") {
        if (this->mRouters.contains(this->handlerName())) {
            this->processRouteCallMessage(message, cmd);
        } else {
            this->processForwardCallMessage(message, cmd);
        }
        return;
    }

    if (cmd.type() == SK_CONNSTATUS) {
        if (cmd.statusType() == SS_ONLINE) {
            if (!this->mConnected) {
                emit this->connected(this->skypeName);
            }
        } else if(cmd.statusType() == SS_OFFLINE) {
            emit this->connectionLost(this->skypeName);
        }
        emit this->ConnectStatus(cmd.statusText(), cmd.statusType());
        return;
    }

    if (cmd.type() == SK_USERSTATUS) {
        emit this->UserStatus(cmd.statusText(), cmd.statusType());
    }

    if (cmd.type() == SK_CLOSE_STREAM) {
        // qDebug()<<__FILE__<<__LINE__<<"Maybe stream disconnected:"<<cmd.streamNum();
        emit this->streamClosed();
    }
}

void Skycit::processRouteCallMessage(const QString &message, SkypeResponse &cmd)
{
    if (cmd.callStatusKey() == "TRANSFER_STATUS") {
        this->activeCalls[cmd.callID().toInt()].TRANSFER_STATUS = cmd.callStatusValue();
    }
    if (cmd.callStatusValue() == "UNPLACED") {
        emit this->newCallArrived(QString(), QString(), cmd.callID().toInt());
    } else if (cmd.callStatusValue() == "RINGING") {
        if (this->activeCalls.contains(cmd.callID().toInt())) {
            if (cmd.callStatusKey() == "STATUS") {
                emit this->routeCallMissed(cmd.callID().toInt(), 
                                           this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                           this->handlerName());
                this->activeCalls[cmd.callID().toInt()].STATUS = CS_RINGING;
                this->activeCalls.remove(cmd.callID().toInt());
                // TRANSFER_STATUS
            } else {

            }
        } else {
            this->activeCalls.insert(cmd.callID().toInt(), SkypeCall());
            this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "TYPE"), false);
        }
    } else if (cmd.callStatusKey() == "TYPE") {
        if (cmd.callStatusValue() == "INCOMING_PSTN") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_INCOMING_PSTN;
        } else if (cmd.callStatusValue() == "OUTGOING_PSTN") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_OUTGOING_PSTN;
        } else if (cmd.callStatusValue() == "INCOMING_P2P") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_INCOMING_P2P;
        } else if (cmd.callStatusValue() == "OUTGOING_P2P") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_OUTGOING_P2P;
        } else {
        }
        this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "TRANSFERRED_BY"), false);
    } else if (cmd.callStatusKey() == "TRANSFERRED_BY") {
        this->activeCalls[cmd.callID().toInt()].TRANSFERRED_BY = cmd.callStatusValue();
        this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "PARTNER_HANDLE"), false);
            
    } else if (cmd.callStatusKey() == "PARTNER_HANDLE") {
        this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE = cmd.callStatusValue();
        emit this->newRouteCallArrived(cmd.callStatusValue(), this->handlerName(), 
                                       this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER,
                                       cmd.callID().toInt());
    } else if (cmd.callStatusKey() == "CONF_PARTICIPANTS_COUNT"
               && cmd.callStatusValue() == "2") {
        if (this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.isEmpty()) {
            QString propName = "CONF_PARTICIPANT";
            this->doCommand(SkypeCommand::GET_CALL_PROP_EX(cmd.callID(), propName, "0"));
            this->doCommand(SkypeCommand::GET_CALL_PROP_EX(cmd.callID(), propName, "1"));
        }
    } else if (cmd.callStatusKey() == "CONF_PARTICIPANT") {
        if (this->activeCalls.contains(cmd.callID().toInt())) {
            if (!cmd.call.PSTN_NUMBER.startsWith("aaa")) {
                // such like yat-sen
            } else {
                if (this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.isEmpty()) {
                    this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER = cmd.call.PSTN_NUMBER;
                    // this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "TYPE"), false);
                } else {
                    // maybe call finished, because we have PSTN_NUMBER already
                }
            }
        } else {
            // not possible
        }
    }  else if (cmd.callStatusValue() == "INPROGRESS") {
        emit this->routeCallAnswered(cmd.callID().toInt(), 
                                this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                this->handlerName());
        this->activeCalls[cmd.callID().toInt()].STATUS = CS_INPROGRESS;
        if (cmd.callStatusKey() == "TRANSFER_STATUS") {
            this->setCallHangup(cmd.callID()); // speed up transferred status
        }
    } else if (cmd.callStatusValue() == "MISSED") {
        // emit this->routeCallMissed(cmd.callID().toInt(), 
        //                            this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
        //                            this->handlerName());
        // this->activeCalls[cmd.callID().toInt()].STATUS = CS_MISSED;
        // this->activeCalls.remove(cmd.callID().toInt());
    } else if (cmd.callStatusValue() == "REFUSED") {
      emit this->routeCallRefused(cmd.callID().toInt(),
				  this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
				  this->handlerName());
    } else if (cmd.callStatusValue() == "TRANSFERRED") {
        // transfered signal
        emit this->routeCallTransferred(cmd.callID().toInt(), 
                                        this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                        this->handlerName(), 
                                        this->activeCalls[cmd.callID().toInt()].TRANSFER_STATUS);
        this->activeCalls[cmd.callID().toInt()].STATUS = CS_MISSED;
        this->activeCalls.remove(cmd.callID().toInt());
    }
}

void Skycit::processForwardCallMessage(const QString &message, SkypeResponse &cmd)
{
    if (cmd.callStatusValue() == "UNPLACED") {
        emit this->newCallArrived(QString(), QString(), cmd.callID().toInt());
    } else if (cmd.callStatusKey() == "STATUS" && cmd.callStatusValue() == "RINGING") {
        this->activeCalls.insert(cmd.callID().toInt(), SkypeCall());
        this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "TYPE"), false);
        // this->doCommand(SkypeCommand::SET_CALL_PROP(cmd.callID(), "DTMF", "6"), false);
    } else if (cmd.callStatusKey() == "TRANSFER_STATUS" && cmd.callStatusValue() == "RINGING") {
        
    } else if (cmd.callStatusKey() == "TYPE") {
        if (cmd.callStatusValue() == "INCOMING_PSTN") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_INCOMING_PSTN;
        } else if (cmd.callStatusValue() == "OUTGOING_PSTN") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_OUTGOING_PSTN;
        } else if (cmd.callStatusValue() == "INCOMING_P2P") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_INCOMING_P2P;
        } else if (cmd.callStatusValue() == "OUTGOING_P2P") {
            this->activeCalls[cmd.callID().toInt()].TYPE = CT_OUTGOING_P2P;
        } else {
        }
        this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "TRANSFERRED_BY"), false);
    } else if (cmd.callStatusKey() == "TRANSFERRED_BY") {
        this->activeCalls[cmd.callID().toInt()].TRANSFERRED_BY = cmd.callStatusValue();
        this->doCommand(SkypeCommand::GET_CALL_PROP(cmd.callID(), "PARTNER_HANDLE"), false);
    } else if (cmd.callStatusKey() == "PARTNER_HANDLE") {
        this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE = cmd.callStatusValue();
        if (!this->activeCalls[cmd.callID().toInt()].TRANSFERRED_BY.isEmpty()) {
            if (this->activeCalls[cmd.callID().toInt()].TYPE == CT_OUTGOING_P2P) {
                // emit this->newCallArrived(this->handlerName(), cmd.callStatusValue(), cmd.callID().toInt());
                emit this->newForwardCallArrived(this->handlerName(), cmd.callStatusValue(), cmd.callID().toInt());
            } else {
                // emit this->newCallArrived(cmd.callStatusValue(), this->handlerName(), cmd.callID().toInt());
                emit this->newForwardCallArrived(cmd.callStatusValue(), this->handlerName(), cmd.callID().toInt());
            }
        }
    } else if (cmd.callStatusKey() == "PSTN_NUMBER") {
        
    } else if (cmd.callStatusValue() == "FINISHED" 
               || cmd.callStatusValue() == "REFUSED") {
        emit this->callHangup(this->handlerName(), this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, cmd.callID().toInt());
    } else  if (cmd.callStatusKey() == "ANSWER") {
        // emit this->callAnswered(cmd.callID().toInt(), 
        //                         this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
        //                         this->handlerName());
        // dup state with INPROGRESS???
    } else if (cmd.callStatusValue() == "INPROGRESS") {
      if (this->activeCalls[cmd.callID().toInt()].STATUS == CS_REMOTEHOLD
	  || this->activeCalls[cmd.callID().toInt()].STATUS == CS_LOCALHOLD) {
        emit this->forwardCallUnhold(cmd.callID().toInt(), 
                                this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                this->handlerName());
      } else {
        emit this->forwardCallAnswered(cmd.callID().toInt(), 
                                this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                this->handlerName());
      }
        this->activeCalls[cmd.callID().toInt()].STATUS = CS_INPROGRESS;
    } else if (cmd.callStatusValue() == "REMOTEHOLD") {
        emit this->forwardCallHold(cmd.callID().toInt(), 
                                this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                this->handlerName());

        this->activeCalls[cmd.callID().toInt()].STATUS = CS_REMOTEHOLD;
    } else if (cmd.callStatusValue() == "LOCALHOLD") {
        emit this->forwardCallHold(cmd.callID().toInt(), 
                                this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                                this->handlerName());

        this->activeCalls[cmd.callID().toInt()].STATUS = CS_LOCALHOLD;
    } else if (cmd.callStatusKey() == "DTMF") {
        // emit this->dtmfSended(cmd.callID().toInt(), cmd.callStatusValue());
        emit this->dtmfSended(cmd.callID().toInt(), 
                              this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE, 
                              this->handlerName(), cmd.callStatusValue());
        // if (this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.isEmpty()) {
        //     if (cmd.callStatusValue() == "*") {
        //         this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER += cmd.callStatusValue();
        //     } else {
        //         // normal dtmf
        //     }
        // } else {
        //     if (this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.startsWith("*")
        //         && this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.endsWith("#")) {
        //         // finished pstn transport
        //         // normal dtmf
        //     } else {
        //         this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER += cmd.callStatusValue();        
        //         if (cmd.callStatusValue() == "#") {
        //             // pstn transport finish event
        //             emit this->forwardCallPstnArrived(cmd.callID().toInt(), 
        //                                               this->activeCalls[cmd.callID().toInt()].PARTNER_HANDLE,
        //                                               this->handlerName(), 
        //                                               this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.mid(1, this->activeCalls[cmd.callID().toInt()].PSTN_NUMBER.length() - 2));
        //         } else {
        //             // pstn transporting go on
        //         }
        //     }
        // }
    } else if (cmd.callStatusKey() == "FAILUREREASON") {

    } else {
    } 
}

// shoud not be here, maybe in upper level
void Skycit::processAP2APMessage(const QString &message)
{
    
}

void Skycit::processChatMessage(const QString &message)
{
    
}

void Skycit::processChatMessageMessage(const QString &message)
{
    
}


int Skycit::waitForResponse( QString cID ) 
{
    waitingForResponse = true;
    waitForResponseID = cID;
    //QTimer *timer = new QTimer(this);
    QTimer::singleShot(TimeOut, this, SLOT(timeOut()));
    int result = localEventLoop.exec();
    waitingForResponse = false;
    return result;
}

bool Skycit::doCommand(QString cmd, bool blocking) 
{
    QString cID = SkypeCommand::prependID( cmd );
    QString ID = SkypeCommand::getID( cID );
    qDebug() <<__FILE__<<__LINE__<< "SKYPE: =>" << cID;
    emit this->commandRequest(cmd);

    this->rrSetsMutex.lock();
    this->requestResponseSets.insert(ID, cID);
    this->rrSetsMutex.unlock();

    sk.sendMsgToSkype( cID );
    if ( blocking ) {
        qDebug() << "Waiting for response to message "<<ID;
        int result = waitForResponse( ID );
        qDebug() << "Result of waiting" << result;
        if ( result == 0 ) {
            if ( response.type() != SK_ERROR ) return true;
        }
        return false;
    } else return true;
}

void Skycit::onCommandRequest(QString cmd)
{
    this->doCommand(cmd);
}

bool Skycit::connectToSkype() 
{ 
    if (!this->mAttached) {
        this->attachToSkype();
    }

    if (!this->mAttached) {
        return false;
    }

#if defined(Q_OS_WIN)
    // windows will detech app name automatically, so do not need a  "NAME xxx" command
    this->onPublished(QString());
#else
    return this->publishToSkype();
#endif

    return true;
}

// private:
bool Skycit::attachToSkype()
{
    return this->mAttached = this->sk.attachToSkype();
}

// private:
bool Skycit::publishToSkype()
{
    return this->doCommand(SkypeCommand::PUBLISH_SA_NAME(this->appPrefix));
}

bool Skycit::disconnectFromSkype() 
{
    if ( ! this->mConnected) return true;
    // if ( ! doCommand( SkypeCommand::DELETE_AP2AP(appName) ) ) return false;
    pingTimer->stop();
    QObject::disconnect(pingTimer, 0, this, 0);
    QObject::disconnect(&sk, 0, this, 0);
    this->mConnected = false;
    return true;
}

bool Skycit::setAutoAway(bool auto_away)
{
    int ok = this->doCommand(SkypeCommand::SET_AUTOAWAY(auto_away));
    return ok;
}

int Skycit::setMute(bool mute)
{
    int ok = this->doCommand(SkypeCommand::SET_MUTE(mute));
    return ok;
}

bool Skycit::getAudioOut()
{
    int ok = this->doCommand(SkypeCommand::GET_AUDIO_OUT());
    return ok;
}

bool Skycit::setAudioOut(QString what)
{
    int ok = this->doCommand(SkypeCommand::SET_AUDIO_OUT(what));
    return ok;
}

void Skycit::newStream(QString contact) 
{ 
    int ok = doCommand( SkypeCommand::CONNECT_AP2AP( appName, contact ) );
    Q_ASSERT(ok);
}

bool Skycit::writeToStream(QByteArray data, QString contactName ) 
{
    // if ( ! activeStream.contains( contactName ) )  return false; // We are not connected to contactName
    if (!this->activeStreams.leftContains(contactName)) return false;

    // doCommand( SkypeCommand::WRITE_AP2AP( appName, contactName, activeStream[contactName],data ), false );
    doCommand( SkypeCommand::WRITE_AP2AP( appName, contactName, this->activeStreams[contactName], data), false);
    return true;
}

QByteArray Skycit::readFromStream(QString contactName) {
    QByteArray ret;
    ret.clear();
    if ( streams.contains( contactName ) ) { 
        //   qDebug() << "DEBUG: streams["<<contactName<<"]="<< streams[contactName];
        ret.append( streams[contactName] );
        streams[contactName].clear();
    } 
    return ret;
}

QString Skycit::callFriend(QString contactName)
{
    int ok = doCommand(SkypeCommand::CALL(contactName), false);
    // Q_ASSERT(ok);
    return QString();
}

int Skycit::answerCall(QString callID)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_STATUS(callID, "ANSWER"), false);
    return 0;
}

int Skycit::setCallHold(QString callID)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_STATUS(callID, "HOLD"), false);
    // int ok = doCommand(SkypeCommand::SET_CALL_PROP(callID, "STATUS", "ONHOLD"), false);
    return 0;
}

int Skycit::setCallResume(QString callID)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_STATUS(callID, "RESUME"), false);
    // int ok = doCommand(SkypeCommand::SET_CALL_PROP(callID, "STATUS", "RESUME"), false);
    return 0;
}

int Skycit::setCallHangup(QString callID)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_STATUS(callID, "HANGUP"), false);
    // int ok = doCommand(SkypeCommand::SET_CALL_PROP(callID, "STATUS", "HANGUP"), false);
    return 0;
}

int Skycit::setCallMediaInputPort(QString callID, unsigned short port)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_SET_INPUT_PORT(callID, QString("%1").arg(port)));
    return 0;
}

int Skycit::setCallMediaOutputPort(QString callID, unsigned short port)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_SET_OUTPUT_PORT(callID, QString("%1").arg(port)));
    return 0;
}

int Skycit::setCallInputNull(QString callID)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_SET_INPUT_SOUNDCARD(callID, QString("NULL")));
    return ok;
}
int Skycit::setCallOutputNull(QString callID)
{
    int ok = doCommand(SkypeCommand::ALTER_CALL_SET_OUTPUT_SOUNDCARD(callID, QString("NULL")));
    return ok;
}

int Skycit::setCallInputFile(QString callID, QString file)
{
    int ok = this->doCommand(SkypeCommand::ALTER_CALL_SET_INPUT_FILE(callID, file));
    return ok;
}

int Skycit::setCallOutputFile(QString callID, QString file)
{
    int ok = this->doCommand(SkypeCommand::ALTER_CALL_SET_OUTPUT_FILE(callID, file));
    return ok;
}

int Skycit::setCallDTMF(QString callID, QString code)
{
    // int ok = this->doCommand(SkypeCommand::SET_CALL_DTMF(callID, code));
    int ok = this->doCommand(SkypeCommand::ALTER_CALL_SET_DTMF(callID, code));
    return ok;
}
int Skycit::setCallForward(QString callID, QString toh)
{
    this->doCommand(SkypeCommand::GET_CALL_PROP_EX(callID, QString("CAN_TRANSFER"), toh));
    int ok = this->doCommand(SkypeCommand::ALTER_CALL_SET_TRANSFER(callID, this->handlerName(), toh));
    return ok;
}

bool Skycit::isTransferredCall(QString callID)
{
    if (this->activeCalls.contains(callID.toInt())) {
        if (this->activeCalls[callID.toInt()].TRANSFERRED_BY.length() > 0) {
            return true;
        }
    }
    return false;
}

QString Skycit::callPartnerHandlerName(QString callID)
{
  QString partner;

  if (this->activeCalls.contains(callID.toInt())) {
    partner = this->activeCalls.value(callID.toInt()).PARTNER_HANDLE;
  }

  return partner;
}

int Skycit::getCallIdByPartnerName(QString callerName)
{
    int call_id = 0;

    QHash<int, SkypeCall>::iterator iter;
    for (iter = this->activeCalls.begin();
         iter != this->activeCalls.end();
         iter ++) {
        if (iter.value().PARTNER_HANDLE == callerName) {
            // 找其中最大的一个。
            if (iter.key() > call_id) {
                call_id = iter.key();
            }
        }
    }

    return call_id;
}

void Skycit::clearChatHistory()
{

}
void Skycit::clearVoiceMailHistory()
{

}

void Skycit::clearCallHistory()
{

}


bool Skycit::sendPackage(QString contactName, int streamNum, QString data)
{
    QString cmd = SkypeCommand::SEND_AP2AP(this->appName, contactName, streamNum, data);
    if (!this->doCommand(cmd)) {
        // Q_ASSERT(1 == 2);
        return false;
    }
    return true;    
}

// for client, should use only one stream
bool Skycit::sendPackage(QString contactName, QString data)
{
    // qDebug()<<this->activeStream;
    qDebug()<<"current usable streams:"<<this->activeStreams;
    // TODO stream_id should be dynamic detect
    // Q_ASSERT(this->activeStream.count() == 1);
    // Q_ASSERT(this->activeStreams.count() == 1);
    if (this->activeStreams.count() == 0) {
        return false;
    }
    // QList<QString> keys = this->activeStream.keys();
    QList<QString> keys = this->activeStreams.leftValues();
    // int streamNum = this->activeStream.value(keys.at(0));
    int streamNum = this->activeStreams.leftToRight(keys.at(0));
    return this->sendPackage(contactName, streamNum, data);
}

void Skycit::onPublished(QString pubName)
{
    this->mNamePublished = true;
    this->doCommand(SkypeCommand::PROTOCOL(50));    
}

void Skycit::onConnected(QString skypeName)
{
    Q_UNUSED(skypeName);
    // bool ok = doCommand( SkypeCommand::CREATE_AP2AP(appName) );
    // Q_ASSERT(ok); 

    // if ( ! doCommand( SkypeCommand::CONNECT_TO_SKYPE(appName) ) ) return false;
 
    // if ( ! doCommand( SkypeCommand::CREATE_AP2AP(appName) ) ) return false;
    this->mConnected = true;
    pingTimer->start(20000);

    // this->doCommand(SkypeCommand::GET_CURRENTUSERHANDLE());
    this->setAutoAway(false);
}

void Skycit::onDisconnected(QString skypeName) 
{
    this->pingTimer->stop();
    this->mConnected = false;
    this->mNamePublished = false;
}

// #include "skype.moc"
