
#ifndef CContactGroup_INCLUDED_HPP
#define CContactGroup_INCLUDED_HPP

#include <QObject>

#include "skype-embedded_2.h"
#include "skype-object.h"

class CContactGroup : public QObject, public ContactGroup
{
    Q_OBJECT;
public:
    typedef DRef<CContactGroup, ContactGroup> Ref;
    CContactGroup(unsigned int oid, SERootObject* root);
    ~CContactGroup() {}

    void OnChange(int prop);
    void OnChange(const ContactRef& contact);

signals:
    void contactGroupChanged(int prop);
    void contactGroupChanged(Contact *contact);
};

#endif //CContactGroup_INCLUDED_HPP

