
#include "cvideo.h"

CVideo::CVideo(unsigned int oid, SERootObject* root)
                : Video(oid, root)
{
        fprintf(stdout,"New video oid %d\n", getOID());
        fflush(stdout);
}

void CVideo::OnChange(int prop)
{
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        
        fprintf(stdout,"Video.%s = %s\n", 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
        fflush(stdout);
}

