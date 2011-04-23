
#ifndef CVoicemail_INCLUDED_HPP
#define CVoicemail_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CVoicemail : public Voicemail
{
public:
        typedef DRef<CVoicemail,Voicemail> Ref;
        CVoicemail(unsigned int oid, SERootObject* root);
        ~CVoicemail() {}

        void OnChange(int prop);
        void sync() {
                m_sync.wait(); // no need to reset event?
        }
private:
        Event m_sync;
};

#endif //CVoicemail_INCLUDED_HPP

