// preference.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-29 17:30:56 +0800
// Version: $Id$
// 
#ifndef _PREFERENCE_H_
#define _PREFERENCE_H_

class PReference
{
public:
    PReference(SEReference &r) {
        this->_ref = r;
    }
    PReference(const SEReference &r) {
        this->_ref = r;
    }
    ~PReference() {

    }

    SEReference &ref() {
        return this->_ref;
    }

    Account::Ref toAccount() {
        if (this->_ref) {
            return Account::Ref(this->_ref->getOID());
        } else {
            return Account::Ref(0);
        }
    }
    Conversation::Ref toConversation() {
        if (this->_ref) {
            return Conversation::Ref(this->_ref->getOID());
        } else {
            return Conversation::Ref(0);
        }
    }

    Contact::Ref toContact() {
        if (this->_ref) {
            return Contact::Ref(this->_ref->getOID());
        } else {
            return Contact::Ref(0);
        }
    }

    ContactGroup::Ref toContactGroup() {
        if (this->_ref) {
            return ContactGroup::Ref(this->_ref->getOID());
        } else {
            return ContactGroup::Ref(0);
        }
    }

    ContactSearch::Ref toContactSearch() {
        if (this->_ref) {
            return ContactSearch::Ref(this->_ref->getOID());
        } else {
            return ContactSearch::Ref(0);
        }
    }

    Message::Ref toMessage() {
        if (this->_ref) {
            return Message::Ref(this->_ref->getOID());
        } else {
            return Message::Ref(0);
        }
    }

    Participant::Ref toParticipant() {
        if (this->_ref) {
            return Participant::Ref(this->_ref->getOID());
        } else {
            return Participant::Ref(0);
        }
    }

    Sms::Ref toSms() {
        if (this->_ref) {
            return Sms::Ref(this->_ref->getOID());
        } else {
            return Sms::Ref(0);
        }
    }

    Transfer::Ref toTransfer() {
        if (this->_ref) {
            return Transfer::Ref(this->_ref->getOID());
        } else {
            return Transfer::Ref(0);
        }
    }

    Video::Ref toVideo() {
        if (this->_ref) {
            return Video::Ref(this->_ref->getOID());
        } else {
            return Video::Ref(0);
        }
    }

    Voicemail::Ref toVoicemail() {
        if (this->_ref) {
            return Voicemail::Ref(this->_ref->getOID());
        } else {
            return Voicemail::Ref(0);
        }
    }

private:
    SEReference _ref;
};


#endif /* _PREFERENCE_H_ */
