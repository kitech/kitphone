
#ifndef CMessage_INCLUDED_HPP
#define CMessage_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CMessage : public Message
{
public:
        typedef DRef<CMessage, Message> Ref;
        CMessage(unsigned int oid, SERootObject* root);
        ~CMessage() {}

        void OnChange(int prop);
private:


};

#endif //CMessage_INCLUDED_HPP

