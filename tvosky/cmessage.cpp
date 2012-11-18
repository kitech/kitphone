
#include "cmessage.h"

CMessage::CMessage(unsigned int oid, SERootObject* root)
                : Message(oid, root)
{
}

void CMessage::OnChange(int prop)
{
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        
        String author = GetStrProp(Message::P_AUTHOR);
        fprintf(stdout,"MESSAGE.%s:%s = %s\n", 
                (const char*)author, 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
        fflush(stdout);
}

