// skparticipant.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 10:52:30 +0800
// Version: $Id: skparticipant.cpp 574 2010-11-17 05:23:57Z drswinghead $
// 

#include "skparticipant.h"

MyParticipant::MyParticipant(unsigned int oid, SERootObject* root) : Participant(oid, root) {};

void MyParticipant::OnChange(int prop, const SEString& value)
{
    if (prop == Participant::P_VOICE_STATUS)
    {
    SEString Identity;
    GetPropIdentity(Identity);
    Participant::VOICE_STATUS VoiceStatus;
    GetPropVoiceStatus(VoiceStatus);

    printf("Participant %s voice status = %d\n", (const char*)Identity, VoiceStatus);

    };
};
