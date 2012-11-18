
#ifndef CContact_INCLUDED_HPP
#define CContact_INCLUDED_HPP

#include <QObject>

#include "skype-embedded_2.h"
#include "skype-object.h"

class CContact : public QObject, public Contact
{
    Q_OBJECT;
public:
    typedef DRef<CContact, Contact> Ref;
    CContact(unsigned int oid, SERootObject* root);
    ~CContact() {}

    void OnChange(int prop);
    String skypeName;

signals:
    void contactStateChanged(int prop);
};

#endif //CContact_INCLUDED_HPP

