
#ifndef CContact_INCLUDED_HPP
#define CContact_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CContact : public Contact
{
public:
        typedef DRef<CContact, Contact> Ref;
        CContact(unsigned int oid, SERootObject* root);
        ~CContact() {}

        void OnChange(int prop);
        String skypeName;
};

#endif //CContact_INCLUDED_HPP

