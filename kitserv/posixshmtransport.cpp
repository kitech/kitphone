
#ifdef VIDEO_TRANSPORT_POSIX

#include "posixshmtransport.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


int PosixSHMTransport::bufnum = 0;

PosixSHMTransport::PosixSHMTransport()
{
}

PosixSHMTransport::~PosixSHMTransport()
{
}


bool PosixSHMTransport::Allocate( bufferstruct *b )
{
	unsigned long rgb32 = 0x32334942;
	if ( b->colorspace == rgb32 || b->colorspace== 0x00)
		b->pitch = 4*b->width;
	else
		return false;
	char fname[50];
	sprintf( fname, "skype_video_frame_nr_%i", bufnum );
	int fd = shm_open(fname,O_RDWR|O_CREAT,0600);
	if ( fd == -1 )
		return false;
	b->bufid = bufnum++;
	b->type = SkypekitVideoTransportBase::PosixSHMBuffer;

	ftruncate( fd, b->pitch*b->height );
	struct bufdata *bdata = new struct bufdata;
	bdata->fd = fd;
	bdata->size = b->pitch*b->height;
	bdata->mem = mmap( 0, bdata->size, PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0 );
	b->clientdata = bdata;
	return true;
}

void PosixSHMTransport::FreeBuffer( bufferstruct *b )
{
	struct bufdata *bdata = (struct bufdata *)b->clientdata;
	munmap( bdata->mem, bdata->size );
	close( bdata->fd );
	char fname[50];
	sprintf( fname, "skype_video_frame_nr_%i", b->bufid );
	shm_unlink( fname );
	delete bdata;
	b->clientdata = NULL;
	b->bufid = -1;
}

bool PosixSHMTransport::Present( bufferstruct *b, long long ts )
{
	struct bufdata *bdata = (struct bufdata *)b->clientdata;
	return Present( bdata->mem, b, ts );
}

bool PosixSHMTransport::Present( void *data, bufferstruct *b, long long ts )
{
	return true;
}
#endif
