
#ifndef CSkype_INCLUDED_HPP
#define CSkype_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"
#include "caccount.h"
#include "cparticipant.h"
#include "cmessage.h"
#include "cconversation.h"
#include "ccontact.h"
#include "ccontactsearch.h"
#include "ctransfer.h"
#include "cvoicemail.h"
#include "csms.h"
#include "ccontactgroup.h"
#include "cvideo.h"

#include <QObject>

class PReference;

class CSkype : public QObject, public Skype
{
    Q_OBJECT;
public:
    CSkype(int num_threads = 1);
    virtual ~CSkype() {}

    /** creators */
    Account* newAccount(int oid);
    Message* newMessage(int oid);
    Conversation* newConversation(int oid);
    Contact* newContact(int oid);
    Participant* newParticipant(int oid);
    ContactSearch* newContactSearch(int oid);
    Transfer* newTransfer(int oid);
    Voicemail * newVoicemail(int oid);
    Sms * newSms(int oid);
    ContactGroup * newContactGroup(int oid);
    Video * newVideo(int oid);

    void OnConversationListChange(const Conversation::Ref& conversation, const Conversation::LIST_TYPE& type, const bool& added);
    void OnContactGoneOffline(const Contact::Ref& contact);
    void OnContactOnlineAppearance(const Contact::Ref& contact);
    void OnMessage(const Message::Ref& message, const bool& changesInboxTimestamp, const Message::Ref& supersedesHistoryMessage, const ConversationRef& conversation);

    bool loggedIn();
    bool loggedOut();
        
    CConversation::Ref get_current_conversation();
    void set_current_conversation(const Conversation::Ref& c);
    bool select_conversation(const String skypename);
        
    void OnApp2AppDatagram(const String& appname, const String& stream, const Binary& data);
    void OnApp2AppStreamListChange(const String& appname, const APP2APP_STREAMS& listType, const List_String& streams);

    void OnProxyAuthFailure(const PROXYTYPE& type);

    CAccount::Ref get_active_account();
    CMessage::Ref get_last_message() {
        Sid::Mutex::ScopedLock l(m_ev_mutex);
        return last_message;
    };
    void set_last_message(const Message::Ref&);

    void login_default_account();  
               
    //ConnectionStateListener callbacks       
    virtual void Disconnected() { 
        //cleanup
        printf("\n::: Disconnected from skypekit, enter 'r' to reconnect...\n\n");         
        // dont invalidate them: they can be used by another thread
        //  activeAccount = CAccount::Ref(); 
        //  current_conversation = CConversation::Ref();  
        //  last_message = CMessage::Ref();                        
    }    
    virtual void Connected() { printf("::: Connected to skypekit\n"); }       
    virtual void Connecting() { printf("::: Connecting to skypekit\n"); }
        
public:
    CAccount::Ref activeAccount;
    CConversation::Ref current_conversation;
    CMessage::Ref last_message;
    Sid::Mutex m_ev_mutex; // useful when several event threads

public:
    QTextCodec *codec;
    CContact::Ref find_contact(SEString &skypename);
    QObject *main_win;

signals:
    // void conversationListChanged(Conversation *conversation, int, bool);
    void conversationListChanged(PReference *conversation, int, bool);
    void contactGoneOffline(const Contact::Ref& contact);
    void contactOnlineAppearance(const Contact::Ref& contact);
    void messageArrived(const Message::Ref& message, const bool& changesInboxTimestamp, const Message::Ref& supersedesHistoryMessage, const ConversationRef& conversation);
};

#endif //CSkype_INCLUDED_HPP

