#include "caccount.h"
#include "main.h"

CAccount::CAccount(unsigned int oid, SERootObject* root)
                : Account(oid, root)
{        
        fprintf(stdout,"New account oid %d\n", getOID());
        fflush(stdout);
}

void CAccount::OnChange(int prop)
{
    SEReference r = this->ref();
    skype_helper_invoke_account_onchange(this->root, r, prop);

        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        
        String skypename = GetStrProp(Account::P_SKYPENAME);
        fprintf(stdout,"ACCOUNT.%s:%s = %s\n", 
                (const char*)skypename, 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
                
        fflush(stdout);

        if (prop == P_STATUS) {
                if ((Account::STATUS)GetUintProp(Account::P_STATUS) == Account::LOGGED_IN) {
                        //(re)create CONTACTS_WAITING_MY_AUTHORIZATION upon login so the new authrequest events will be received
                        // if (skype->GetHardwiredContactGroup(ContactGroup::CONTACTS_WAITING_MY_AUTHORIZATION, auth_reqs_group)) {
                        //         uint nrofcontacts;
                        //         auth_reqs_group->GetPropNrofcontacts(nrofcontacts);                            
                        // }                      
                }
        }
}

Account::STATUS CAccount::getStatus()
{
        return (Account::STATUS)GetUintProp(Account::P_STATUS);
}

