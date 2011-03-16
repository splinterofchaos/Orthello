
#include "Screen.h"

#include <SDL/SDL.h>

int   Screen::height = 600;
int   Screen::width  = 600;
float Screen::scale  = 1;

bool resize_window( float w, float h, float scale )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    w *= scale;
    h *= scale;

    glOrtho( -w/2, w/2, h/2, -h/2, -10000, 10000 );
    glMatrixMode( GL_MODELVIEW );

    return true;
}

GLenum init_gl( int w, int h )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    resize_window( w, h );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable( GL_DEPTH_TEST );

    glClearDepth(1000.0f);	

    glEnable( GL_NORMALIZE );

    // Lighting.
    glEnable( GL_LIGHTING ); 
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable( GL_COLOR_MATERIAL ) ;

    GLfloat ambiant[] = { 0.4, 0.4,  0.4, 1 };
    GLfloat diffuse[] = { 1, 1,  1, 1 };
    GLfloat pos[]     = { 0,     0,  500, 1 };
    GLfloat spot[]    = { 0, 0, -1, 1 };

    glLightfv( GL_LIGHT1, GL_AMBIENT, ambiant );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT1, GL_POSITION, pos );
    glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, spot );

    glShadeModel( GL_FLAT );

    glEnable( GL_LIGHT1 );

    return glGetError();
}

#ifdef __WIN32
// Borrowed and modified from http://www.devmaster.net/forums/showthread.php?t=443
#include <Windows.h>
void set_vsync( int interval )
{
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
    PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

    const char *extensions = (const char*)glGetString( GL_EXTENSIONS );

    if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 ) {
        return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
    } else {
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

        if( wglSwapIntervalEXT )
            wglSwapIntervalEXT( interval );
    }
}
#endif

bool make_sdl_gl_window( int w, int h )
{
    if( !SDL_SetVideoMode(w, h, 32, SDL_OPENGL|SDL_RESIZABLE) )
        return false;

    if( ! resize_window(w,h) )
        return false;
    if( ! init_gl( w, h ) )
        return false;

#ifdef __WIN32
    set_vsync( 0 );
#endif

    return true;
}

void update_screen()
{
    SDL_GL_SwapBuffers();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
