
#include "cconversation.h"
#include "cmessage.h"
#include "main.h"

CConversation::CConversation(unsigned int oid, SERootObject* root)
                : Conversation(oid, root)
{     
    this->IsLiveSessionUp = false;
}

void CConversation::OnChange(int prop)
{        
    SEReference r = this->ref();
    skype_helper_invoke_conversation_onchange(this->root, r, prop);

    String value = GetProp(prop);
    List_String dbg = getPropDebug(prop, value);
        
    Sid::String displ_name = GetStrProp(Conversation::P_DISPLAYNAME);
        // fprintf(stdout,"CONVERSATION.%s:%s = %s\n", 
        //         (const char*)displ_name, 
        //         (const char*)dbg[1], 
        //         (const char*)dbg[2]);
                
        // fflush(stdout);
}

void CConversation::PickUpCall()
{
    // CallerList we keep in Conversation class - so that it won't get garbage collected
  // while the conversation object exists in the wrapper.
    this->GetParticipants(CallerList, Conversation::OTHER_CONSUMERS);
    SEString PartList = "";
    for (unsigned I = 0; I < CallerList.size(); I++)
    {
        PartList = PartList + " " + (const char*)CallerList[I]->GetProp(Participant::P_IDENTITY);
    };

    if (!IsLiveSessionUp)
    {
        printf("Incoming call from: %s \n", (const char*)PartList);
        this->JoinLiveSession();
    }
    else
    {
        printf("Another call is coming in from : %s \n", (const char*)PartList);
        printf("As we already have a live session up, we will reject it. \n");
        this->LeaveLiveSession(true);
    };
}
