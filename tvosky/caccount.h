#ifndef CAccount_INCLUDED_HPP
#define CAccount_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"
#include "ccontactgroup.h"

#include <QtCore>
class CAccount : public QObject, public Account
{
    Q_OBJECT;
public:
        typedef DRef<CAccount, Account> Ref;

        CAccount(unsigned int oid, SERootObject* root);
        ~CAccount() {}

        void OnChange(int prop);
        Account::STATUS getStatus();

private:
        ContactGroup::Ref auth_reqs_group;
signals:
    void accountStateChanged(int state);
};

#endif //CAccount_INCLUDED_HPP

