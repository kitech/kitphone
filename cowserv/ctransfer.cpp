
#include "ctransfer.h"

CTransfer::CTransfer(unsigned int oid, SERootObject* root)
                : Transfer(oid, root)
{
        fprintf(stdout,"New Transfer oid %d\n", getOID());
}

void CTransfer::OnChange(int prop)
{
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        fprintf(stdout,"TRANSFER.%s:%s = %s\n", 
                (const char*)GetStrProp(P_FILEPATH),
                (const char*)dbg[1], 
                (const char*)dbg[2]);
}
