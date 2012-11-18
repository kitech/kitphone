
#ifdef GL_WINDOW

#include "glwindow.h"

#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLContext.h>

#include <IOSurface/IOSurfaceAPI.h>
#include <OpenGL/CGLIOSurface.h>

#include <unistd.h>

#ifdef VIDEO_TRANSPORT_IOSURFACE
static bool get_gl_format_for_fourcc(unsigned int fourcc,GLenum &int_format,GLenum &ext_format,GLenum &data_type)
{
	switch (fourcc)
	{
		case SFT_FOURCC_RGB32:
			int_format=GL_RGB8;
			ext_format=GL_BGRA;
			data_type=GL_UNSIGNED_INT_8_8_8_8_REV;
			return true;
#ifdef __APPLE__
		case SFT_FOURCC_UYVY:
			int_format=GL_RGB8;
			ext_format=GL_YCBCR_422_APPLE;
			data_type=GL_UNSIGNED_SHORT_8_8_APPLE;
			return true;
		case SFT_FOURCC_YUY2:
			int_format=GL_RGB8;
			ext_format=GL_YCBCR_422_APPLE;
			data_type=GL_UNSIGNED_SHORT_8_8_REV_APPLE;
			return true;
#endif
		default:
			return false;
	}
}
#endif

glWindow *glWindow::first = NULL;

glWindow::glWindow() :
	next( NULL ),
	textureWidth( 0 ),
	textureHeight( 0 )
{
	printf(" Creating main win\n" );
	int dummy = 1;
	const char *dargv[] = { "Test" };
	glutInit( &dummy, (char **)dargv );
	glutInitDisplayMode(GLUT_DOUBLE);

	glutInitWindowSize( 640, 480 );
	win_id = glutCreateWindow("video");
	glGenTextures(1,&tex_id);
	glutDisplayFunc( displayFunc );
	glutIdleFunc(idleFunc);
		
	unsigned long rgb32 = 0x32334942;
	SetPreferences( &rgb32, 1 );
	//Start ();
	
	next = first;
	first = this;
	printf(" Creating main win ready\n" );
}

glWindow::~glWindow()
{
	if ( first == this )
		first = this->next;
	else {
		glWindow *w=first;
		while (w) {
			if ( w->next == this ) {
				w->next = this->next;
				break;
			}
		}
	}
}

#include <stdio.h>

void glWindow::idleFunc( void )
{
}

void glWindow::displayFunc( void )
{
}

bool glWindow::Present( void *data, bufferstruct *b, long long ts )
{
	GLenum texturetarget;
	int owidth, oheight;
	if ( b->type == SkypekitVideoTransportBase::IOSurfaceBuffer ) {
#ifdef VIDEO_TRANSPORT_IOSURFACE
		owidth = b->width; oheight = b->height;
		printf( "Present iosurface\n" );
		texturetarget = GL_TEXTURE_RECTANGLE_ARB;
		glEnable(texturetarget);
		if ( !b->clientdata ) {
			IOSurfaceRef m_surface = IOSurfaceLookup(b->bufid);
			if ( !m_surface ) {
				printf( "Could not get surface\n" );
				return true; // We could not get surface, but we still wan't to releaes buffer.
			}
			GLuint tex_id;
			glGenTextures(1,&tex_id);
			printf( "tex_id: %i\n", tex_id );
			b->clientdata = (void *)tex_id;
			glBindTexture(texturetarget, tex_id);
			GLenum int_format,ext_format,data_type;
			if ( !get_gl_format_for_fourcc((b->colorspace?b->colorspace:SFT_FOURCC_RGB32),int_format,ext_format,data_type) ) {
				printf( "Could not get formats based on fourcc: %lx\n", b->colorspace );
				return true;
			}
			CGLError err = CGLTexImageIOSurface2D(CGLGetCurrentContext(),GL_TEXTURE_RECTANGLE_ARB,int_format,b->width,b->height,ext_format,data_type,m_surface,0);
			if(err != kCGLNoError) {
				printf("Error creating IOSurface texture: %s & %x\n", CGLErrorString(err), glGetError());
			}
		} else {
			glBindTexture(texturetarget, (GLuint)b->clientdata);
			printf( "rebind tex_id: %i\n", (GLuint)b->clientdata );
		}
#endif
	} else {
		owidth = 1; oheight = 1;
		texturetarget = GL_TEXTURE_2D;
		glEnable(texturetarget);
		glBindTexture( texturetarget, tex_id );
		if ( textureWidth != b->width || textureHeight != b->height ) {
			textureWidth = b->width; textureHeight = b->height;
			glTexParameteri(texturetarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(texturetarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(texturetarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(texturetarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
			glTexImage2D(texturetarget, 0, GL_RGB8, b->width, b->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data );
			glutReshapeWindow(textureWidth, textureHeight);
		} else {
			glTexSubImage2D(texturetarget, 0, 0, 0, b->width, b->height, GL_BGRA, GL_UNSIGNED_BYTE, data );
		}
	}


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, owidth, oheight, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);
	glTexCoord2f(b->width,0);
	glVertex2f(b->width,0);
	glTexCoord2f(b->width, b->height);
	glVertex2f(b->width, b->height);
	glTexCoord2f(0,b->height);
	glVertex2f(0,b->height);
	glTexCoord2f(0,0);
	glVertex2f(0,0);
	glEnd();
	glBindTexture(texturetarget,0);
	glDisable(texturetarget);
	glutSwapBuffers();
	return true;
}

void window_run_events()
{
	glWindow *w;
	for( w = glWindow::first;w;w=w->next ) {
		w->ReceiveCommand( true );
	}
	if ( glWindow::first ) {
		glutCheckLoop();
		usleep(10000);
	}
}
#endif
