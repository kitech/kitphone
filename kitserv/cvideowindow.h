
#ifndef GL_VIDEOWINDOW_H
#define GL_VIDEOWINDOW_H

#ifdef SKYPEKIT_SURFACE_RENDERING

// This is an example implementation, it uses GLUT for windowing, which is not ideal.

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#else
#include <GL/freeglut.h>
#include <GL/gl.h>
#endif

#include "SkypekitFrameTransport.hpp"

class VideoWindow
{
public:
	VideoWindow(SkypekitFrameTransport *tr);
	~VideoWindow();

	static void run_events();

private:
	static void initialize();
	static void uninitialize();

	unsigned int width,height;
	GLuint tex_id;
	static int ns_initialized;

	SkypekitFrameTransport *transport;

	// GLUT
	friend VideoWindow *::VW_map_id_to_window(int id);
	friend void ::VW_idlefunc(void);
	friend void ::VW_displayfunc(void);
	void idle();
	void display();
	int win_id;
	VideoWindow *next;
	static VideoWindow *first;
};

#endif

#endif

