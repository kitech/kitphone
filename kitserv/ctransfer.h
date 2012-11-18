
#ifndef CTransfer_INCLUDED_HPP
#define CTransfer_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CTransfer : public Transfer
{
public:
        typedef DRef<CTransfer, Transfer> Ref;
        CTransfer(unsigned int oid, SERootObject* root);
        ~CTransfer() {}

        void OnChange(int prop);
private:
};

#endif //CTransfer_INCLUDED_HPP

