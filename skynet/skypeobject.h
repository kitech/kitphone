// skypeobject.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-24 16:43:26 +0800
// Version: $Id$
// 

#ifndef _SKYPEOBJECT_H_
#define _SKYPEOBJECT_H_

#include <QtCore>

enum SkypeObject {
    SO_UNKNOWN, SO_USER, SO_PROFILE, SO_CHAT, SO_CALL, SO_MESSAGE /* dep, by CHATMESSAGE*/, 
    SO_CHATMEMBER, SO_CHATMESSAGE, SO_VOICEMAIL, SO_SMS, SO_APPLICATION,
    SO_GROUP, SO_FILETRANSFER, SO_CONNECTION
};
enum UserProperty {
    UP_ONLINESTATUS=1
};

/////////////////////////////////////////

class SkypeMeta
{
 public:
  SkypeMeta() {}
  virtual ~SkypeMeta() {}

  static int CallStatusToInt(QString status);
};

struct SkypeUser
{
public:
    SkypeUser() {}
    virtual ~SkypeUser() {}

public:
    QString HANDLE;
    QString FULLNAME;
    QString BIRTHDAY; // YYYYMMDD
    QString SEX; // UNKNOWN, MALE, FEMAL
    QString LANGUAGE;
    QString COUNTRY;
    QString PROVINCE;
    QString CITY;
    QString PHONE_HOME;
    QString PHONE_MOBILE;
    QString HOMEPAGE;
    QString ABOUT;
    bool HASCALLEQUIPMENT;
    bool IS_VIDEO_CAPABLE;
    bool IS_VOICEMAIL_CAPABLE;
    int BUDDYSTATUS;
    bool ISAUTHORIZED;
    bool ISBLOCKED;
    int ONLINESTATUS;
    QString SKYPEOUT;
    QString SKYPEME;
    unsigned int LASTONLINETIMESTAMP;
    bool CAN_LEAVE_VM;
    QString SPEEDDIAL;
    QString RECEIVEDAUTHREQUEST;
    QString MODD_TEXT;
    QString RICH_MOOD_TEXT;
    QString ALIASES;
    int TIMEZONE;
    bool IS_CF_ACTIVE;
    int NROF_AUTHED_BUDDIES;
};

////////////////////////////////////////

enum CallType {
    CT_INCOMING_PSTN = 1, CT_OUTGOING_PSTN, CT_INCOMING_P2P, CT_OUTGOING_P2P
};
enum CallStatus {
    CS_UNPLACED=1, CS_ROUTING, CS_EARLYMEDIA, CS_FAILD,
    CS_RINGING, CS_INPROGRESS, CS_ONHOLD, CS_REMOTEHOLD, CS_LOCALHOLD, 
    CS_FINISHED,
    CS_MISSED, CS_REFUSED, CS_BUSY, CS_CANCELLED,
    CS_TRANSFERRING, CS_TRANSFERRED, CS_VM_BUFFERING_GREETING,
    CS_VM_PLAYING_GREETING, CS_VM_RECORDING, CS_VM_UPLOADING,
    CS_VM_SENT, CS_VM_CANCELLED, CS_VM_FAILD, CS_WAITING_REDIAL_COMMAND,
    CS_REDIAL_PENDING
};
enum CallTransferStatus {
    CTS_UNPLACED=1, CTS_ROUTING, CTS_RINGING, CTS_INPROGRESS, CTS_FINISHED, CTS_TRANSFFERED
};
enum VideoStatus {
    VS_VIDEO_DONE=1, VS_VIDEO_SEND_ENABLED, VS_VIDEO_RECV_ENABLED,
    VS_VIDEO_BOTH_ENABLED
};
enum VideoSendStatus {
    VSS_NOT_AVAILABLE = 1, VSS_AVAILABLE, VSS_STARTING, 
    VSS_REJECTED, VSS_RUNNING, VSS_STOPPING, VSS_PAUSED
};

enum ChatStatus {
    CS_LEGACY_DIALOG = 1, CS_DIALOG, CS_MULTI_SUBSCRIBED, CS_UNSUBSCRIBED
};
enum ChatMyStatus {
    CMS_CONNECTING = 1, CMS_WAITING_REMOTE_ACCEPT, CMS_ACCEPT_REQUIRED, 
    CMS_PASSWORD_REQUIRED, CMS_SUBSCRIBED, CMS_UNSUBSCRIBED, CMS_CHAT_DISBANDED,
    CMS_QUEUED_BECAUSE_CHAT_IS_NULL, CMS_APPLICATION_DENIED, CMS_KICKED,
    CMS_BANNED, CMS_RETRY_CONNECTING
};

enum ChatMessageStatus {
    CMS_SENDING = 1, CMS_SENT, CMS_RECEIVED, CMS_READ
};
enum ChatMessageLeaveReason {
    CMLR_USER_NOT_FOUND = 1, CMLR_USER_INCAPABLE, CMLR_ADDER_MUST_BE_FRIEND,
    CMLR_ADDED_MUST_BE_AUTHORIZED, CMLR_UNSUBSCRIBE
};

enum ChatRole {
    CR_CREATOR = 1, CR_MASTER, CR_HELPER, CR_USER, CR_LISTENER, CR_APPLICANT
};

enum OnlineStatus {
    OS_ONLINE=1,
    OS_OFFLINE
};

class SkypeCall 
{
public:
    SkypeCall() {}
    virtual ~SkypeCall() {}

public:

public:
    int CALL_ID;
    unsigned int TIMESTAMP;
    QString PARTNER_HANDLE;
    QString PARTNER_DISPNAME;
    int CONF_ID;
    int TYPE;
    int STATUS;
    int VIDEO_STATUS;
    int VIDEO_SEND_STATUS;
    int FAILUREREASON;
    QString SUBJECT;
    QString PSTN_NUMBER;
    int DURATION;
    int PSTN_STATUS;
    int CONF_PARTICIPANTS_COUNT;
    int CONF_PARTICIPANT;
    int VM_DURATION;
    int VM_ALLOWED_DURATION;
    int RATE;
    int RATE_CURRENCY;
    int RATE_PRECISION;
    QString INPUT;
    QString OUTPUT;
    QString CAPTURE_MIC;
    bool VAA_INPUT_STATUS;
    QString FORWARDED_BY;
    QString TRANSFER_ACTIVE;
    QString TRANSFER_STATUS;
    QString TRANSFERRED_BY;
    QString TRANSFERRED_TO;
    QString SEEN;
    QString DTMF;
    int JOIN_CONFERENCE;
};


////////////////
// depcreated object, replaced by CHATMESSAGE object
enum MessageType { MT_AUTHREQUEST=1, MT_TEXT, MT_CONTACTS, MT_UNKNOWN};
enum MessageStatus { MS_SENDING =1, MS_SENT, MS_FAILED, MS_RECEIVED, 
                     MS_READ, MS_IGNORED, MS_QUEUED };
class SkypeMessage
{
public:
    SkypeMessage() {}
    virtual ~SkypeMessage() {}

public:
    int MESSAGE_ID;
    unsigned int TIMESTAMP;
    QString PARTNER_HANDLE;
    QString PARTNER_DISPNAME;
    int CONF_ID;
    int TYPE;
    int FAILUREREASON;
    QString BODY;
    QString SEEN;
};


//////////////////////////
class SkypeChat 
{
public:
    SkypeChat() {}
    virtual ~SkypeChat() {}

public:
    QString NAME;
    unsigned int TIMESTAMP;
    QString ADDER;
    int STATUS;
    QString POSTERS;
    QString MEMBERS;
    QString TOPIC;
    QString TOPICXML;
    QString CHATMESSAGES;
    QString ACTIVEMEMBERS;
    QString FRIENDLYNAME;
    QString RECENTCHATMESSAGES;
    bool BOOKMARKED;
    
    QVector<QString> MEMBEROBJECTS;
    QString PASSWORDHINT;
    QString GUIDELINES;
    int OPTIONS;
    QString DESCRIPTION;
    QString DIALOG_PARTNER;
    unsigned int ACTIVITY_TIMESTAMP;
    int TYPE;
    int MYSTATUS;
    int MYROLE;
    QString BLOB;
    QString APPLICANTS;
};

///////////////////////
class SkypeChatMember
{
public:
    SkypeChatMember() {}
    virtual ~SkypeChatMember() {}

public:
    QString CHATNAME;
    QString IDENTITY;
    int ROLE;
    bool IS_ACTIVE;
};

///////////////
class SkypeChatMessage
{
public:
    SkypeChatMessage() {}
    virtual ~SkypeChatMessage() {}

public:
    QString ID;
    unsigned TIMESTAMP;
    QString PARTNER_HANDLE;
    QString PARTNER_DISPNAME;
    QString FROM_HANDLE;
    QString FROM_DISPNAME;
    int type;
    QString CHATNAME;
    QString USERS;
    bool IS_EDITABLE;
    QString EDITED_BY;
    unsigned int EDITED_TIMESTAMP;
    int OPTIONS;
    int ROLE;
    bool SEEN;
    QString BODY;
    int STATUS;
};


////////////////
class SkypeVoiceMail
{
public:
    SkypeVoiceMail() {}
    virtual ~SkypeVoiceMail() {}

public:
    
};


///////////
class SkypeSMS
{
public:
    SkypeSMS() {}
    virtual ~SkypeSMS() {}

public:
    
};

//////////////
class SkypeApplication
{
public:
    SkypeApplication() {}
    virtual ~SkypeApplication() {}

public:
    
};

////////////////
class SkypeFileTransfer
{
public:
    SkypeFileTransfer() {}
    virtual ~SkypeFileTransfer() {}

public:
    
};

///////////////////////

class SkypeProfile
{
public:
    SkypeProfile() {}
    virtual ~SkypeProfile() {}

public:
    
};

#endif /* _SKYPEOBJECT_H_ */
