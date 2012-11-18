
#ifdef X11_WINDOW

#include "x11window.h"

#include <sys/shm.h>


X11Window *X11Window::first=NULL;

X11Window::X11Window()
{
	init();
	next = first;
	first = this;
}

X11Window::~X11Window()
{
	if ( first == this )
		first = this->next;
	else {
		X11Window *w = first;
		while( w ) {
			if ( w->next == this ) {
				w->next = this->next;
				break;
			}
		}
	}
}

#include <stdio.h>
void X11Window::init()
{
	display = XOpenDisplay( NULL );
	//XvQueryPortAttributes( display,  );
	
	win = XCreateSimpleWindow( display, XRootWindow( display, DefaultScreen(display)), 100, 100, 200, 400, 0, BlackPixel(display,0), BlackPixel(display,0) );
	XSelectInput( display, win, ExposureMask );

	//XSelectInput( display, win, );
	XGCValues xgcv;
	gc = XCreateGC( display, win, 0L, &xgcv );
	XMapWindow( display, win );
	XFlush( display );
	for( int i=0;i<10;i++ ) {
		imglist[i].img = NULL;
	}
	unsigned long rgb32 = 0x32334942;
	SetPreferences( &rgb32, 1 );
	Start();
}

#include <errno.h>

bool X11Window::Allocate( bufferstruct *b )
{
	int li;
	for( li=0;li<10;li++ ) {
		if ( !imglist[li].img )
			break;
	}
	if ( li == 10 )
		return false;
	int screen = DefaultScreen( display );
	imglist[li].img = XShmCreateImage (display, DefaultVisual(display, screen), DefaultDepth(display,screen), ZPixmap, NULL, &(imglist[li].seginfo), b->width, b->height);
	b->bufid = imglist[li].seginfo.shmid = shmget (IPC_PRIVATE, imglist[li].img->bytes_per_line * imglist[li].img->height, IPC_CREAT|0777);
	if ( b->bufid == -1 ) {
		// TODO XDestroyImage( imglist[li].img  );
		imglist[li].img = NULL;
		return false;
	}
	b->pitch = imglist[li].img->bytes_per_line;
	b->clientdata = (void *)&(imglist[li]);
	imglist[li].seginfo.shmaddr = imglist[li].img->data = (char *)shmat (imglist[li].seginfo.shmid, 0, 0);
	b->type = SkypekitVideoTransportBase::SysVSHMBuffer;

	imglist[li].seginfo.readOnly = True;
	imglist[li].bstr = b;
	XShmAttach( display, &imglist[li].seginfo );
	return true;
}

void X11Window::FreeBuffer( bufferstruct *b )
{
	struct imgstr *img = (struct imgstr *)b->clientdata;
	shmdt( img->img->data );
	printf( "TODO: Releasing of X11 buffer\n" );
}

bool X11Window::Present( bufferstruct *b, long long ts )
{
	struct imgstr *imgs = (struct imgstr *)b->clientdata;
	XShmPutImage( display, win, gc, imgs->img, 0, 0, 0, 0, b->width, b->height, True );
	return false;
}

void X11Window::run_events() 
{
	if ( !display )
		return;
	int CompletionType = XShmGetEventBase(display)+ShmCompletion;
	while ( XPending(display ) ) {
		XEvent event;
		XNextEvent( display, &event );
		if ( event.type == CompletionType ) {
			XShmCompletionEvent *ce = (XShmCompletionEvent *)&event;
			SearchAndFreeBuffer( ce->shmseg );
		}
	}
}

void X11Window::SearchAndFreeBuffer( ShmSeg seg )
{
	for( int i=0;i<10;i++ ) {
		if ( imglist[i].seginfo.shmseg == seg ) {
			AddFreeBuffer( imglist[i].bstr );
			return;
		}
	}
	if ( next )
		next->SearchAndFreeBuffer( seg );
}

void window_run_events()
{
	if ( X11Window::first )
		X11Window::first->run_events();
}

#endif
