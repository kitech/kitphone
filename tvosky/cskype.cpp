
#include "cskype.h"
#include "helper-functions.h"

#include "preference.h"

CSkype::CSkype(int num_threads)
    : QObject(0), Skype(num_threads),
      activeAccount(0), 
      current_conversation(0),
      last_message(0)
{
    this->codec = QTextCodec::codecForName("UTF-8");
}

/** creators */
Account* CSkype::newAccount(int oid) {
    CAccount *account = new CAccount(oid, this);
    QObject::connect(account, SIGNAL(accountStateChanged(int)),
                     this->main_win, SLOT(onLoginStateChanged(int)));
    return account;
    // return new CAccount(oid, this);
}
Message* CSkype::newMessage(int oid) {
    return new CMessage(oid, this);
}
Conversation* CSkype::newConversation(int oid) {
    return new CConversation(oid, this);
}
Contact* CSkype::newContact(int oid) {
    CContact *contact = new CContact(oid, this);
    QObject::connect(contact, SIGNAL(contactStateChanged(int)),
                     this->main_win, SLOT(onContactStateChanged(int)));
    return contact;
    // return new CContact(oid, this);
}
Participant* CSkype::newParticipant(int oid) {
    return new CParticipant(oid, this);
}
ContactSearch* CSkype::newContactSearch(int oid) {
    return new CContactSearch(oid, this);
}
Transfer* CSkype::newTransfer(int oid) {
    return new CTransfer(oid, this);
}
Voicemail * CSkype::newVoicemail(int oid) {
    return new CVoicemail(oid, this);
}
Sms * CSkype::newSms(int oid) {
    return new CSms(oid, this);
}
ContactGroup * CSkype::newContactGroup(int oid) {
    return new CContactGroup(oid, this);
} 
Video * CSkype::newVideo(int oid) {
    return new CVideo(oid, this);
}        

//////////
void CSkype::set_last_message(const Message::Ref& message) {
    Mutex::ScopedLock l(m_ev_mutex);
    last_message = message;
}

void CSkype::OnConversationListChange(const Conversation::Ref& conversation, const Conversation::LIST_TYPE& type, const bool& added)
{
    Conversation::Ref c = conversation;
    Conversation *cv = static_cast<Conversation*>(conversation.operator->());
    printf("SKYPE.OnConversationListChange:%s,list_type %d, was %s\n",
           (const char*)c->GetStrProp(Conversation::P_DISPLAYNAME), type, added ? "added":"removed");
    emit this->conversationListChanged(new PReference(conversation), (int)type, added);
        
    Conversation::LOCAL_LIVESTATUS conv_live_status;
    conversation->GetPropLocalLivestatus(conv_live_status);
        
    if (conv_live_status == Conversation::RINGING_FOR_ME) {
        Sid::String identity;
        conversation->GetPropIdentity(identity);
        printf("Incoming call from %s, enter Ca to answer, Ce to refuse\n", 
               (const char*)identity);         
    }        	
}

void CSkype::OnContactGoneOffline(const Contact::Ref& contact)
{
    printf("SKYPE.OnContactGoneOffline:%s\n", (const char*)(CContact::Ref(contact))->skypeName);
}

void CSkype::OnContactOnlineAppearance(const Contact::Ref& contact)
{
    printf("SKYPE.OnContactOnlineAppearance:%s\n", (const char*)(CContact::Ref(contact))->skypeName);
}

void CSkype::OnMessage(const Message::Ref& message, const bool& changesInboxTimestamp, const Message::Ref& supersedesHistoryMessage, const ConversationRef& conversation)
{
    if (!message) return;
    set_last_message(message);

    List_String dbg = message->getPropDebug(Message::P_TYPE, message->GetUintProp(Message::P_TYPE));        
    printf("SKYPE.OnMessage.%s Message::TYPE = Message::%s\n", 
           (const char*)message->GetStrProp(Message::P_AUTHOR), 
           (const char*)dbg[2]);
                
    fflush(stdout);

    Message::TYPE msg_type; 
    message->GetPropType(msg_type);
    String author;
    message->GetPropAuthor(author);
    switch (msg_type) {
    case Message::POSTED_VOICE_MESSAGE: {
        printf("New voice message from %s\n",(const char*)author);
        break;
    }
    case Message::POSTED_TEXT: {
        String txt;
        message->GetPropBodyXml(txt);
        printf("CHAT.%s: %s\n", (const char*)author, (const char*)txt);
        break;
    }
    case Message::POSTED_FILES: {
        Transfer::Refs transfers;
        if (message->GetTransfers(transfers)) {
            for (unsigned int i = 0; i < (uint)transfers.size(); i++) {
                Transfer::Ref transfer = transfers[i];
                String filename;
                transfer->GetPropFilename(filename);
                printf("POSTED.%s: %s\n", 
                       (const char*)filename, 
                       (const char*)author);
            }
        }
        break;
    }
    default: break;
    }

    fflush(stdout);
}

bool CSkype::loggedIn()
{
    return activeAccount && activeAccount->getStatus() == Account::LOGGED_IN;
}

bool CSkype::loggedOut()
{
    return !activeAccount || activeAccount->getStatus() <= Account::LOGGED_OUT_AND_PWD_SAVED;
}

CConversation::Ref CSkype::get_current_conversation()
{
    if (!loggedIn() || !current_conversation) {
        printf("You are not logged in or there is no selected conversation (use cc)\n");
        return CConversation::Ref();
    }

    return current_conversation;
}

void CSkype::set_current_conversation(const Conversation::Ref& c)
{
    current_conversation = c;
}

CAccount::Ref CSkype::get_active_account()
{
    if (!(activeAccount && activeAccount->getStatus() == Account::LOGGED_IN)) {
        printf("You are not logged in\n");
        return CAccount::Ref();
    }
    return activeAccount;
}

bool CSkype::select_conversation(const String skypename)
{
    if (!loggedIn()) {
        printf("You are not logged in\n");
        return false;
    }
                
    Conversation::Refs conversations;
    if (!GetConversationList(conversations)) {
        printf("Unable to get conversation list\n");
        return false;
    }
          
    for (uint i = 0; i < (uint)conversations.size(); i++) {
        Conversation::Ref conversation = conversations[i];
        if (conversation->GetStrProp(Conversation::P_IDENTITY) == skypename) {
            set_current_conversation(conversation);
            return true;
        }
    } 
                 
    return false;  
}

void CSkype::login_default_account()
{
    String accname;
    if (!GetDefaultAccountName(accname) || accname.isEmpty()|| !GetAccount(accname, activeAccount)) {
        printf("Default account is not available, use aL command to login manually\n");
        return;
    }
        
    if (activeAccount->getStatus() != Account::LOGGED_OUT_AND_PWD_SAVED)
        {
            return;
        }
                   
    activeAccount->Login(Contact::ONLINE);
        
    printf("\nLogging in with a default account: %s\n", 
           (const char*)activeAccount->GetStrProp(Account::P_SKYPENAME)); 
}

void CSkype::OnApp2AppDatagram(const String& appname, const String& stream, const Binary& data)
{
    printf("OnApp2AppDatagram %s %s\n", (const char*)appname, (const char*)stream);
    if (data.size() != 0)
        {
            const char * p = (const char*)data;
            for (unsigned int i = 0; i < data.size(); i++) 
                printf("%d %c\n", i, p[i]);
        }
}

void CSkype::OnProxyAuthFailure(const PROXYTYPE& type)
{
    printf("SKYPE.OnProxyAuthFailure %d\n", type);
}

void CSkype::OnApp2AppStreamListChange(const String& appname, const APP2APP_STREAMS& listType, const List_String& streams)
{
    printf("OnApp2AppStreamListChange %s type %d\n", (const char*)appname, listType);
    for (unsigned int i = 0; i < streams.size(); i++)
        printf("%d %s\n", i, (const char*)streams[i]);
}

//////////////////////////////
CContact::Ref CSkype::find_contact(SEString &skypename)
{
    CSkype *skype = this;

    if (!skype->loggedIn()) {
        printf("You are not logged in\n");
        return Contact::Ref(0);
    }

    ContactGroup::Ref cg;
    Contact::Refs contacts;
    if (!skype->GetHardwiredContactGroup(ContactGroup::ALL_KNOWN_CONTACTS, cg)
        || !cg->GetContacts(contacts)) {
        printf("Unable to get contact list\n");
        return Contact::Ref(0);
    }

    Sid::String identity;
    for (uint i = 0; i < (uint)contacts.size(); i++) {
        contacts[i]->GetIdentity(identity);
        if (contacts[i] && identity == skypename) {
            return contacts[i];
        }
    }

    return Contact::Ref(0);    
}
