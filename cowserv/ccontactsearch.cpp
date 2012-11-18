
#include "ccontactsearch.h"

CContactSearch::CContactSearch(unsigned int oid, SERootObject* root)
                : ContactSearch(oid, root)
{
        fprintf(stdout,"New contact search oid %d\n", getOID());
        fflush(stdout);
}

void CContactSearch::OnChange(int prop)
{
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        fprintf(stdout,"CONTACTSEARCH.%d:%s = %s\n", getOID(), 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
        fflush(stdout);
}

void CContactSearch::OnNewResult(const ContactRef& contact, const uint& rankValue)
{
        Sid::String identity; 
        contact->GetIdentity(identity);
        printf("ContactSearch.%d.OnNewResult (%u): %s\n", getOID(), rankValue, (const char*)identity); 
}

