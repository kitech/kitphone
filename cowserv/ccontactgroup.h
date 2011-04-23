
#ifndef CContactGroup_INCLUDED_HPP
#define CContactGroup_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CContactGroup : public ContactGroup
{
public:
        typedef DRef<CContactGroup, ContactGroup> Ref;
        CContactGroup(unsigned int oid, SERootObject* root);
        ~CContactGroup() {}

        void OnChange(int prop);
        void OnChange(const ContactRef& contact);
};

#endif //CContactGroup_INCLUDED_HPP

