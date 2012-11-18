
#include "cvoicemail.h"

CVoicemail::CVoicemail(unsigned int oid, SERootObject* root)
                : Voicemail(oid, root)
{        
}

void CVoicemail::OnChange(int prop)
{
        String value = GetProp(prop);
        List_String dbg = getPropDebug(prop, value);
        fprintf(stdout,"VOICEMAIL.%s:%s = %s\n", 
                (const char*)GetStrProp(Voicemail::P_PARTNER_HANDLE), 
                (const char*)dbg[1], 
                (const char*)dbg[2]);
        
        if (prop == Voicemail::P_STATUS) {
                switch (atoi((const char*)value)) {
                case Voicemail::RECORDING:
                        m_sync.notify();
                        break;
                case Voicemail::RECORDED:
                        m_sync.notify();
                        break;
                        //		case Voicemail::PLAYED:		 m_sync.Trigger(); break;
                default:
                        break;
                }
        }
}

