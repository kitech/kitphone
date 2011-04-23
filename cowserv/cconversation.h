
#ifndef CConversation_INCLUDED_HPP
#define CConversation_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CConversation : public Conversation
{
public:
        typedef DRef<CConversation, Conversation> Ref;
        CConversation(unsigned int oid, SERootObject* root);
        ~CConversation() {}

        void OnChange(int prop);

    void PickUpCall();

    ParticipantRefs CallerList;
    bool IsLiveSessionUp;
    ConversationRef LiveSession;  // this is last live session, not current
};

#endif //CConversation_INCLUDED_HPP

