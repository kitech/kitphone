
#include "cparticipant.h"
#include "main.h"

CParticipant::CParticipant(unsigned int oid, SERootObject* root)
                : Participant(oid, root)
{
        fprintf(stdout,"New participant oid %d\n", getOID());
}

void CParticipant::OnChange(int prop)
{
    SEReference r = this->ref();
    skype_helper_invoke_participant_onchange(this->root, r, prop);

        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        String identity = GetStrProp(Participant::P_IDENTITY);
        // fprintf(stdout,"PARTICIPANT.%s:%s = %s\n", 
        //         (const char*)identity, 
        //         (const char*)dbg[1], 
        //         (const char*)dbg[2]);
}

void CParticipant::OnIncomingDTMF(const DTMF& dtmf)
{
    SEReference r = this->ref();
    skype_helper_invoke_participant_onincoming_dtmf(this->root, r, dtmf);

        String identity = GetStrProp(Participant::P_IDENTITY);
	fprintf(stdout,"PARTICIPANT.%s.OnIncomingDTMF dtmf = %d\n", 
               (const char*)identity, (int)dtmf);
}

