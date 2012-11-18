// skconversation.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 10:48:36 +0800
// Version: $Id: skconversation.h 574 2010-11-17 05:23:57Z drswinghead $
// 

#ifndef _SKCONVERSATION_H_
#define _SKCONVERSATION_H_

#include "skype-embedded_2.h"

//---------------------------------------------------------------------------------------
// Subclassing Conversation

class MyConversation : public Conversation
{
public:
    typedef DRef<MyConversation, Conversation> Ref;
    MyConversation(unsigned int oid, SERootObject* root);
    ~MyConversation() {};

    void OnChange(int prop);

    bool CallFinished;
};

#endif /* _SKCONVERSATION_H_ */
