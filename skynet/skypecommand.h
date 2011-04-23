#ifndef _SKYPECOMMAND_H
#define _SKYPECOMMAND_H

/***************************************************************
 * skypeCommand.h
 * @Author:      Jonathan Verner (jonathan.verner@matfyz.cz)
 * @License:     GPL v2.0 or later
 * @Created:     2008-04-25.
 * @Last Change: 2008-04-25.
 * @Revision:    $Id: skypecommand.h 179 2010-10-20 08:27:32Z drswinghead $
 * Description:
 * Usage:
 * TODO:
 * CHANGES:
 ***************************************************************/

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "skypeobject.h"

// todo 
enum SkypeResponses { SK_OK, SK_ERROR, SK_INFO, SK_READY_TO_READ, 
                      SK_DATA, SK_DATAGRAM, SK_STREAMS, SK_UNKNOWN, SK_NO_COMMAND, 
                      SK_STATUS, SK_ECHO, SK_END_OF_DATA, SK_CLOSE_STREAM, 
                      SK_PARSE_ERROR, SK_PING, SK_CONTACT_LIST, SK_GROUP,
                      SK_PROTOCOL,SK_CALL, SK_CONNSTATUS, SK_USER, SK_USERSTATUS,
                      SK_CURRENTUSERHANDLE, SK_FILETRANSFER, SK_CHAT, SK_CHATMESSAGE,
                      SK_SMS
};

enum SkypeCommands{
    CK_UNKNOWN, CK_OK, CK_ERROR, CK_PROTOCOL, CK_CURRENTUSERHANDLE, CK_USER,
    CK_GROUP, CK_APPLICATION
};

enum SkypeStatus {
    SS_UNKNOWN, SS_OFFLINE, SS_INVISIBLE, SS_DND, SS_NA, SS_AWAY, SS_SKYPEME, SS_ONLINE
};

class SkypeResponse {
public:
    // user object
    // group object
    // message object
    SkypeUser user;
    SkypeChat chat;
    SkypeChatMember member;
    SkypeChatMessage chatMessage;
    SkypeMessage message;
    SkypeCall call;
    SkypeApplication ap2ap;
    SkypeVoiceMail vmail;
    SkypeSMS sms;

private:
    enum SkypeResponses Type;
    enum SkypeCommands cmd;
    QString Msg;

    // connection object
    int StatusType;
    QString StatusText;

    // application object
    int StreamNum, ProtocolNum, ErrorCode;
    QString ContactName, AppName;
    QString ErrorMsg;
    QString ResponseID;
    QByteArray Data;
    QStringList Contacts;

    // call object
    QString CallID;
    QString CallStatusKey;
    QString CallStatusValue;
    QString CallDuration;

    void clear();

public:
    SkypeResponse();
    virtual ~SkypeResponse();
    bool parse(QString msg);
    bool parseMisc(QString msg);
    bool parseApp(QString msg);
    bool parseCall(QString msg);
    bool parseChat(QString msg);
    bool parseChatMessage(QString msg);

    int statusType() { return this->StatusType; }
    QString statusText() { return this->StatusText; }

    QString responseID() {return ResponseID;};
    int protocolNum() {return ProtocolNum;};
    int streamNum() {return StreamNum;};
    QString appName() { return AppName; };
    QString contactName() {return ContactName;};
    QByteArray data() {return Data;};
    QStringList getContacts() { return Contacts; };
    int errorCode() {return ErrorCode;};
    QString errorMsg() {return ErrorMsg;};
    enum SkypeResponses type() {return Type;};
    QString streamID(); // returns contactName:streamNum
    QString _debugState();

    QString callID() { return this->CallID; }
    QString callDuration() { return this->CallDuration; }
    QString callStatusKey() { return this->CallStatusKey; }
    QString callStatusValue() { return this->CallStatusValue; }
};

class SkypeCommand {
private:
    static int ID;
public:
    static QString PUBLISH_SA_NAME(QString AppName);
    static QString GET_CURRENTUSERHANDLE();
    static QString CREATE_AP2AP(QString appName);
    static QString DELETE_AP2AP(QString AppObject);
    static QString CONNECT_AP2AP(QString appName, QString contactName);
    static QString GET_CONTACT_LIST();
    static QString WRITE_AP2AP(QString appName, QString contactName, int streamNum, QByteArray data);
    static QString READ_AP2AP(QString appName, QString contactName, int streamNum);
    static QString SEND_AP2AP(QString appName, QString contactName, int streamNum, QString data);
    static QString RECV_AP2AP(QString appName, QString contactName, int streamNum);
    static QString DISCONNECT_AP2AP(QString appName, QString contactName, int streamNum);
    static QString PING();
    static QString PROTOCOL(int protocolNum);
    static QString SET_AUTOAWAY(bool auto_away);
    static QString SET_MUTE(bool mute);
    static QString GET_AUDIO_OUT();
    static QString SET_AUDIO_OUT(QString what);
    static QString CALL(QString contactName);
    static QString GET_CALL_PROP(QString callID, QString propName);
    static QString GET_CALL_PROP_EX(QString callID, QString propName, QString subProp);
    static QString SET_CALL_PROP(QString callID, QString propName, QString propValue);
    static QString ALTER_CALL_STATUS(QString callID, QString propName);
    static QString ALTER_CALL_SET_INPUT_PORT(QString callID, QString port);
    static QString ALTER_CALL_SET_OUTPUT_PORT(QString callID, QString port);
    static QString ALTER_CALL_SET_INPUT_SOUNDCARD(QString callID, QString card);
    static QString ALTER_CALL_SET_OUTPUT_SOUNDCARD(QString callID, QString card);
    static QString ALTER_CALL_SET_INPUT_FILE(QString callID, QString file);
    static QString ALTER_CALL_SET_OUTPUT_FILE(QString callID, QString file);
    static QString ALTER_CALL_SET_DTMF(QString callID, QString code);
    static QString ALTER_CALL_SET_TRANSFER(QString callID, QString byh, QString toh);
    static QString SET_CALL_DTMF(QString callID, QString code);
    static QString ALTER_CHAT_PROP(QString chatID, QString prop, QString value);
    static QString SET_CHATMESSAGE_PROP(QString msgID, QString prop, QString value);
    static QString CLEAR_CHATHISTORY();
    static QString CLEAR_VOICEMAILHISTORY();
    static QString CLEAR_CALLHISTORY();
    static QString prependID(QString command, QString myID);
    static QString prependID(QString command);
    static QString nextID();
    static QString getID(QString command);
    static QString streamID(QString contactName, int streamNum);
    static void    removeID(QString &msg);
};





#endif /* _SKYPECOMMAND_H */
