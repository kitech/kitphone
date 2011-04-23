
#ifndef CParticipant_INCLUDED_HPP
#define CParticipant_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

#include <QtCore>
class CParticipant : public QObject, public Participant
{
    Q_OBJECT;
public:
        typedef DRef<CParticipant,Participant> Ref;
        CParticipant(unsigned int oid, SERootObject* root);
        ~CParticipant() {}

        void OnChange(int prop);
	void OnIncomingDTMF(const DTMF& dtmf);
};

#endif //CParticipant_INCLUDED_HPP

