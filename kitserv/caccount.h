#ifndef CAccount_INCLUDED_HPP
#define CAccount_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"
#include "ccontactgroup.h"


class CAccount : public Account
{
public:
        typedef DRef<CAccount, Account> Ref;

        CAccount(unsigned int oid, SERootObject* root);
        ~CAccount() {}

        void OnChange(int prop);
        Account::STATUS getStatus();

private:
        ContactGroup::Ref auth_reqs_group;
};

#endif //CAccount_INCLUDED_HPP

