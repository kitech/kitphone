// skypekit.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 09:42:51 +0800
// Version: $Id: skypekit.h 574 2010-11-17 05:23:57Z drswinghead $
// 

#ifndef _SKYPEKIT_H_
#define _SKYPEKIT_H_

#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif

#include "skype-embedded_2.h"

using namespace Sid;

//---------------------------------------------------------------------------------------
// Subclassing Skype

class MySkype : public Skype
{
public:
    MySkype();
    virtual ~MySkype();

	ContactGroupRef allContactGroup;
	ContactRefs allContacts; 

    // Every time an account object is created, we will return instance of MyAccount
    Account* newAccount(int oid);

    Conversation* newConversation(int oid);// {return new MyConversation(oid, this);}
    Participant* newParticipant(int oid);//  {return new MyParticipant(oid, this);}

    void OnConversationListChange(
                                  const ConversationRef &conversation,
                                  const Conversation::LIST_TYPE &type,
                                  const bool &added);

    bool SetupAudioDevices(uint MicNr, uint SpkNr);

public:
    bool LoggedIn; //  
    bool LoggedOut; //
    bool CallFinished; //bool CallFinished   = false;
    bool IsLiveSessionUp; // bool IsLiveSessionUp    = false;
};


#endif /* _SKYPEKIT_H_ */
