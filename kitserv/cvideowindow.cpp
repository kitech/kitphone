
#if 0

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cvideowindow.h"
#ifdef __APPLE__
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLContext.h>
#endif

int VideoWindow::ns_initialized=0;
VideoWindow *VideoWindow::first=0;

// GLUT incovenience
VideoWindow *VW_map_id_to_window(int id)
{
	VideoWindow *tmp=VideoWindow::first;
	while (tmp)
	{
		if (tmp->win_id==id) return tmp;
		tmp=tmp->next;
	}
	return 0;
}

void VW_idlefunc(void)
{
	VideoWindow *tmp=VideoWindow::first;
	while (tmp)
	{
		glutPostWindowRedisplay(tmp->win_id);
		tmp=tmp->next;
	}
	usleep(10000);
}

void VW_displayfunc(void)
{
	VideoWindow *tmp=VW_map_id_to_window(glutGetWindow());
	if (tmp) tmp->display();
}

void VideoWindow::display()
{
	unsigned int src_width,src_height;
	if (transport->hasNewFrame())
	{
		transport->updateGLTexture(tex_id);
		transport->getSrcDimensions(src_width,src_height);
		// test to see if it is bigger than current surface?
		// at this point we should recreate the surface with proper size,
		// however this is not implemented here
		src_width=(src_width>transport->getWidth())?transport->getWidth():src_width;
		src_height=(src_height>transport->getHeight())?transport->getHeight():src_height;
		if (src_height!=height || src_width!=width)
		{
			width=src_width;
			height=src_height;
			glutReshapeWindow(width,height);
		}
	}

	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,tex_id);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,width,height,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0,1,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f(width,0);
	glVertex2f(width,0);
	glTexCoord2f(width,height);
	glVertex2f(width,height);
	glTexCoord2f(0,height);
	glVertex2f(0,height);
	glTexCoord2f(0,0);
	glVertex2f(0,0);
	glEnd();
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);


	glutSwapBuffers();
}

void VideoWindow::idle()
{
	if (transport->hasNewFrame()) glutPostWindowRedisplay(win_id);
}

void VideoWindow::run_events()
{
	if (ns_initialized)
	{
#ifdef __APPLE__
		glutCheckLoop();
#else
		glutMainLoopEvent();
#endif
	}
}

void VideoWindow::initialize()
{
	if (ns_initialized++) return;
	int dummy=0;
	glutInit(&dummy,0);
	glutInitDisplayMode(GLUT_DOUBLE);
}

void VideoWindow::uninitialize()
{
	if (!ns_initialized) return;
	//if (--ns_initialized) return;
	// no uninit needed
}

VideoWindow::VideoWindow(SkypekitFrameTransport *tr) :
	width(tr->getWidth()),
	height(tr->getHeight()),
	tex_id(0),
	transport(tr),
	next(0)
{
	tr->resetGLTexture();
	initialize();

	glutInitWindowSize(width,height);
	win_id=glutCreateWindow("video");
	glGenTextures(1,&tex_id);
	glutDisplayFunc(VW_displayfunc);
	glutIdleFunc(VW_idlefunc);
#ifdef __APPPLE__
	// osX does not have the vertical sync on by default
	GLint swap=1;
	CGLSetParameter(CGLGetCurrentContext(),kCGLCPSwapInterval,&swap);
#endif
	next=first;
	first=this;
}

VideoWindow::~VideoWindow()
{
	glutDestroyWindow(win_id);
	glDeleteTextures(1,&tex_id);
	if (first==this)
	{
		first=this->next;
	} else {
		VideoWindow *tmp=first;
		while (tmp)
		{
			if (tmp->next==this)
			{
				tmp->next=this->next;
				break;
			}
		}
	}
	uninitialize();
}
#endif
