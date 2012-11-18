
#include "ccontact.h"

CContact::CContact(unsigned int oid, SERootObject* root)
    : QObject(0),
      Contact(oid, root),
      skypeName("")
{
        GetIdentity(skypeName);
}

void CContact::OnChange(int prop)
{
    emit this->contactStateChanged(prop);
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        
        fprintf(stdout,"CONTACT.%s:%s = %s\n", 
                (const char*)skypeName, 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
        
        fflush(stdout);
}

