
#ifndef CSms_INCLUDED_HPP
#define CSms_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CSms : public Sms
{
public:
        typedef DRef<CSms, Sms> Ref;
        CSms(unsigned int oid, SERootObject* root);
        ~CSms() {}

        void OnChange(int prop);
};

#endif //CSms_INCLUDED_HPP

