#ifdef VIDEO_TRANSPORT_SYSV

#include "sysvshmtransport.h"

#include <sys/shm.h>


SysVSHMTransport::SysVSHMTransport()
{
}

SysVSHMTransport::~SysVSHMTransport()
{
}


bool SysVSHMTransport::Allocate( bufferstruct *b )
{
	unsigned long rgb32 = 0x32334942;
	if ( b->colorspace == rgb32 || b->colorspace== 0x00)
		b->pitch = 4*b->width;
	else
		return false;
	b->bufid = shmget( IPC_PRIVATE, b->pitch*b->height, IPC_CREAT|0777);
	if ( b->bufid == -1 )
		return false;
	b->type = SkypekitVideoTransportBase::SysVSHMBuffer;
	b->clientdata = shmat( b->bufid, 0, 0 );
	return true;
}

void SysVSHMTransport::FreeBuffer( bufferstruct *b )
{
	if ( b->clientdata )
		shmdt( b->clientdata );
	b->clientdata =NULL;
	shmctl( b->bufid, IPC_RMID, NULL );
	b->bufid = -1;
}

bool SysVSHMTransport::Present( bufferstruct *b, long long ts )
{
	return Present( b->clientdata, b, ts );
}

bool SysVSHMTransport::Present( void *data, bufferstruct *b, long long ts )
{
	return true;
}
#endif
