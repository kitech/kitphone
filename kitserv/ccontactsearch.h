
#ifndef CContactSearch_INCLUDED_HPP
#define CContactSearch_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CContactSearch : public ContactSearch
{
public:
        typedef DRef<CContactSearch, ContactSearch> Ref;
        CContactSearch(unsigned int oid, SERootObject* root);
        ~CContactSearch() {}

        void OnChange(int prop);
        void OnNewResult(const ContactRef& contact, const uint& rankValue);
};



#endif //CContactSearch_INCLUDED_HPP

