#ifndef _X11WINDOW_H_
#define _X11WINDOW_H_

#include <SkypekitVideoTransportClient.hpp>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

class X11Window : public SkypekitVideoTransportClient {
	public:
		X11Window();
		~X11Window();

		void init();
		
		bool Allocate( bufferstruct *b );
		void FreeBuffer( bufferstruct *b );
		bool Present( bufferstruct *b, long long ts );
	
		void run_events();

		X11Window *next;
		static X11Window *first;

	private:
		Display *display;
		Window win;
		GC gc;

		struct imgstr {
			XImage *img;
			XShmSegmentInfo seginfo;
			bufferstruct *bstr;
		} imglist[10];

		void SearchAndFreeBuffer( ShmSeg seg );

};

#endif
