// skconversation.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 10:49:13 +0800
// Version: $Id: skconversation.cpp 574 2010-11-17 05:23:57Z drswinghead $
// 

#include "skconversation.h"


MyConversation::MyConversation(unsigned int oid, SERootObject* root)
    : Conversation(oid, root)
{
    this->CallFinished = false;
}

void MyConversation::OnChange(int prop)
{
    if (prop == Conversation::P_LOCAL_LIVESTATUS)
    {
    Conversation::LOCAL_LIVESTATUS LiveStatus;
    this->GetPropLocalLivestatus(LiveStatus);

    SEStringList Dbg;
    Dbg = this->getPropDebug(Conversation::P_LOCAL_LIVESTATUS, LiveStatus);
    printf("Live status changed to %s (from MyConversation::OnChange)\n", (const char*)Dbg[2]);
    
    if ((LiveStatus == Conversation::RECENTLY_LIVE) || (LiveStatus == Conversation::NONE))
    {
      printf("Call finished.\n");
      CallFinished = true;
    };
    };
};

