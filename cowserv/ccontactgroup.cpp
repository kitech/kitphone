
#include "ccontactgroup.h"

CContactGroup::CContactGroup(unsigned int oid, SERootObject* root)
                : ContactGroup(oid, root)
{ 	
}

void CContactGroup::OnChange(int prop)
{
        String value = GetProp(prop);
        String mTypeName = getPropDebug(ContactGroup::P_TYPE, GetUintProp(ContactGroup::P_TYPE))[2];
        List_String dbg = getPropDebug(prop, value);
        fprintf(stdout,"CONTACTGROUP.%s:%s = %s\n", 
                (const char*)mTypeName, 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
        fflush(stdout);
}

void CContactGroup::OnChange(const ContactRef& contact)
{
	String identity; 
	bool added = false;
	contact->GetIdentity(identity); 
	contact->IsMemberOf(ref(), added);  
	String mTypeName = getPropDebug(ContactGroup::P_TYPE, GetUintProp(ContactGroup::P_TYPE))[2];
         
	printf("CONTACTGROUP.%s contact %s was %s\n", 
               (const char*)mTypeName, 
               (const char*)identity, added ? "added" : "removed");
               	
	if ((ContactGroup::TYPE)GetUintProp(ContactGroup::P_TYPE) == ContactGroup::CONTACTS_WAITING_MY_AUTHORIZATION) {
		printf("New auth request from %s is received\n", (const char*)identity); 
	}
}

