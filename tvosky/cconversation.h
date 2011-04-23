
#ifndef CConversation_INCLUDED_HPP
#define CConversation_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

#include <QtCore>

class CConversation : public QObject, public Conversation
{
    Q_OBJECT;
public:
    typedef DRef<CConversation, Conversation> Ref;
    CConversation(unsigned int oid, SERootObject* root);
    ~CConversation() {}

    void OnChange(int prop);
};

#endif //CConversation_INCLUDED_HPP

