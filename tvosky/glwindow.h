
#include "videotransport.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#else
#include <GL/freeglut.h>
#include <GL/gl.h>
#endif

class glWindow : public  VideoTransport {
	public:
		glWindow();
		~glWindow();

		bool Present( void *data, bufferstruct *b, long long ts );

		static void displayFunc( void );
		static void idleFunc( void );
		glWindow *next;
		static glWindow *first;
	private:
		GLuint tex_id;
		int win_id;

		int textureWidth, textureHeight;
};

