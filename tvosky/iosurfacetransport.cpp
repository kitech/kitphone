
#ifdef VIDEO_TRANSPORT_IOSURFACE

#include "iosurfacetransport.h"

#include <CoreVideo/CVPixelBuffer.h>

static unsigned int skype_fourcc_to_bpp(unsigned int fourcc)
{
	switch (fourcc) {
		case SFT_FOURCC_RGB32:
			return 4;
		case SFT_FOURCC_UYVY:
		case SFT_FOURCC_YUY2:
			return 2;
		default:
			return 0;
	}
}


static unsigned int skype_fourcc_to_pixelformat(unsigned int fourcc,unsigned int &bpp)
{
	bpp=skype_fourcc_to_bpp(fourcc);
	switch (fourcc)
	{
		case SFT_FOURCC_RGB32:
			return kCVPixelFormatType_32ABGR;
		case SFT_FOURCC_UYVY:
			return 'uyvy';
		case SFT_FOURCC_YUY2:
			return kCVPixelFormatType_422YpCbCr8;
		default:
			return 0;
	}
}


IOSurfaceTransport::IOSurfaceTransport()
{
}

IOSurfaceTransport::~IOSurfaceTransport()
{
}


bool IOSurfaceTransport::Allocate( bufferstruct *b )
{
	unsigned int pxlfmt;
	unsigned int bpp;
	pxlfmt=skype_fourcc_to_pixelformat((b->colorspace?b->colorspace:SFT_FOURCC_RGB32),bpp);
	if ( !pxlfmt )
		return false;

	b->pitch = IOSurfaceAlignProperty(kIOSurfaceBytesPerRow,bpp*b->width);
	CFNumberRef c_width,c_height,c_bpp,c_pitch,c_pxlfmt;
	SInt32 tmp_width=b->width;
	c_width=CFNumberCreate(0,kCFNumberSInt32Type,&tmp_width);
	SInt32 tmp_height=b->height;
	c_height=CFNumberCreate(0,kCFNumberSInt32Type,&tmp_height);
	SInt32 tmp_bpp=bpp;
	c_bpp=CFNumberCreate(0,kCFNumberSInt32Type,&tmp_bpp);
	SInt32 tmp_pitch=b->pitch;
	c_pitch=CFNumberCreate(0,kCFNumberSInt32Type,&tmp_pitch);
	OSType tmp_pxlfmt=pxlfmt;
	c_pxlfmt=CFNumberCreate(0,kCFNumberSInt32Type,&tmp_pxlfmt);

	CFDictionaryRef prop=CFDictionaryCreate(0,
			(const void*[]){kIOSurfaceWidth,kIOSurfaceHeight,kIOSurfaceBytesPerElement,kIOSurfaceBytesPerRow,kIOSurfacePixelFormat,kIOSurfaceIsGlobal},
			(const void*[]){c_width,c_height,c_bpp,c_pitch,c_pxlfmt,kCFBooleanTrue},
			6,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
	// TODO m_surface might be set into clientdata...
	IOSurfaceRef m_surface = IOSurfaceCreate(prop);
	CFRelease(c_width);
	CFRelease(c_height);
	CFRelease(c_bpp);
	CFRelease(c_pitch);

	b->bufid = IOSurfaceGetID( m_surface );
	b->clientdata = NULL;

	b->type = SkypekitVideoTransportBase::IOSurfaceBuffer;

	return true;
}

void IOSurfaceTransport::FreeBuffer( bufferstruct *b )
{
	b->bufid = -1;
	printf( "TODO: IOSurface FreeBuffer unimplemented\n" );
}

bool IOSurfaceTransport::Present( bufferstruct *b, long long ts )
{
	IOSurfaceRef m_surface = IOSurfaceLookup(b->bufid);
	if ( !m_surface ) {
		return true; // We could not get surface, but we still wan't to releaes buffer.
	}

	return Present( IOSurfaceGetBaseAddress(m_surface), b, ts );
}

bool IOSurfaceTransport::Present( void *data, bufferstruct *b, long long ts )
{
	return true;
}
#endif
