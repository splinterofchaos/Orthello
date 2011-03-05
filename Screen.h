
#include <GL/gl.h>

bool resize_window( float w, float h );

GLenum init_gl( int w, int h );

#ifdef __WIN32
void set_vsync( int interval = 1 );
#endif

bool make_sdl_gl_window( int w, int h );

void update_screen();
