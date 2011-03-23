
#include "Screen.h"

#include <SDL/SDL.h>

int   Screen::height = 600;
int   Screen::width  = 600;
float Screen::scale  = 1;

bool resize_window( float w, float h, float scale )
{
#if defined( __linux__ )
    // Seems to be necessary to do this on Linux, but bad on Windows.
    if( !SDL_SetVideoMode(w, h, 32, SDL_OPENGL|SDL_RESIZABLE) )
        return false;
#endif

    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    w *= scale;
    h *= scale;

    glOrtho( -w/2, w/2, h/2, -h/2, -10000, 10000 );
    glMatrixMode( GL_MODELVIEW );

    glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.1 / (scale * scale) );
    glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01 / (scale * scale) );
    glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.00001 / (scale * scale) );

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

    // Lighting.
    GLfloat specularMat[] = { 0.05, 0.05, 0.05, 1 };
    GLfloat emissionMat[] = { -0.3, -0.3, -0.3, 1 };
    GLfloat one[] = { 1, 1, 1, 1 };
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specularMat );
    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  emissionMat );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,  one );

    glEnable( GL_COLOR_MATERIAL ) ;
    
    GLfloat diffuse[]  = { 0.7, 0.7,  0.7,   1 };
    GLfloat ambient[]  = {   1,   1,    1,   1 };
    GLfloat pos[]      = {   0,   0,  100,   1 };
    GLfloat spot[]     = {   1,   1,   -1,   1 };

    glLightfv( GL_LIGHT1, GL_AMBIENT,  ambient  );
    glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuse  );
    glLightfv( GL_LIGHT1, GL_POSITION, pos      );
    glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, spot );

    glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.00001 );
    glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0001 );

    glEnable( GL_LIGHT1 );
    glEnable( GL_LIGHTING );

    glShadeModel( GL_SMOOTH );

    glEnable( GL_NORMALIZE );

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
