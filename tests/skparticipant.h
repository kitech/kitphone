// skparticipant.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 10:51:49 +0800
// Version: $Id: skparticipant.h 574 2010-11-17 05:23:57Z drswinghead $
// 

#ifndef _SKPARTICIPANT_H_
#define _SKPARTICIPANT_H_

#include "skype-embedded_2.h"

//---------------------------------------------------------------------------------------
// Subclassing Participant

class MyParticipant : public Participant
{
public:
    typedef DRef<MyParticipant, Participant> Ref;
    MyParticipant(unsigned int oid, SERootObject* root);
    ~MyParticipant() {};

    void OnChange(int prop, const SEString& value);
};

#endif /* _SKPARTICIPANT_H_ */
