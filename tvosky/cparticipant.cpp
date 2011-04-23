
#include "cparticipant.h"

CParticipant::CParticipant(unsigned int oid, SERootObject* root)
    : QObject(0), Participant(oid, root)
{
        fprintf(stdout,"New participant oid %d\n", getOID());
}

void CParticipant::OnChange(int prop)
{
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        String identity = GetStrProp(Participant::P_IDENTITY);
        fprintf(stdout,"PARTICIPANT.%s:%s = %s\n", 
                (const char*)identity, 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
}

void CParticipant::OnIncomingDTMF(const DTMF& dtmf)
{
        String identity = GetStrProp(Participant::P_IDENTITY);
	fprintf(stdout,"PARTICIPANT.%s.OnIncomingDTMF dtmf = %d\n", 
               (const char*)identity, (int)dtmf);
}

