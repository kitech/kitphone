// skypekit.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 09:43:02 +0800
// Version: $Id: skypekit.cpp 574 2010-11-17 05:23:57Z drswinghead $
// 

#include "skaccount.h"
#include "skconversation.h"
#include "skparticipant.h"
#include "skypekit.h"

MySkype::MySkype()
    : Skype() 
{
    this->LoggedIn = false;
    this->LoggedOut = false;
    this->CallFinished = false;
    this->IsLiveSessionUp = false;
}

MySkype::~MySkype()
{
}

// Every time an account object is created, we will return instance of MyAccount
Account* MySkype::newAccount(int oid)
{ 
    return new MyAccount(oid, this);
}

Conversation* MySkype::newConversation(int oid) 
{
    return new MyConversation(oid, this);
}
Participant* MySkype::newParticipant(int oid)
{
    return new MyParticipant(oid, this);
}

void MySkype::OnConversationListChange(
    const ConversationRef &conversation,
    const Conversation::LIST_TYPE &type,
    const bool &added)
{
  SEString DisplayName;
  Conversation::LOCAL_LIVESTATUS LiveStatus;
  conversation->GetPropDisplayname(DisplayName);
  conversation->GetPropLocalLivestatus(LiveStatus);

  if (type == Conversation::LIVE_CONVERSATIONS)
  {
    SEStringList Dbg;
    Dbg = conversation->getPropDebug(Conversation::P_LOCAL_LIVESTATUS, LiveStatus);
    printf("Live status changed to %s (from MySkype::OnConversationListChange)\n", (const char*)Dbg[2]);

        if (LiveStatus == Conversation::IM_LIVE)
        {
            printf("Live session is up.\n");
            IsLiveSessionUp = true;
        };

        if ((LiveStatus == Conversation::RECENTLY_LIVE) || (LiveStatus == Conversation::NONE))
        {
            printf("Call finished.\n");
            IsLiveSessionUp = false;
      CallFinished = true;
        };
  };
};

bool MySkype::SetupAudioDevices(uint MicNr, uint SpkNr)

{
  SEStringList SpkHandles, SpkNames, SpkProductIDs;
  SEStringList MicHandles, MicNames, MicProductIDs;

  GetAvailableOutputDevices (SpkHandles, SpkNames,  SpkProductIDs);
  GetAvailableRecordingDevices (MicHandles, MicNames, MicProductIDs);

  if (MicNr > (MicHandles.size() + 1))
  {
    printf("Invalid mic device no. (%d) passed to MySkype::SetupAudioDevices\n", MicNr);
    return false;
  };

  if (SpkNr > (SpkHandles.size() + 1))
  {
    printf("Invalid speaker device no. (%d) passed to MySkype::SetupAudioDevices\n", SpkNr);
    return false;
  };

  printf("Setting mic to %s\n", (const char*)MicNames[MicNr]);
  printf("Setting speakers to %s\n", (const char*)SpkNames[SpkNr]);

  SelectSoundDevices(MicHandles[MicNr], SpkHandles[SpkNr], SpkHandles[SpkNr]);
  SetSpeakerVolume(100);
  return true;
};
