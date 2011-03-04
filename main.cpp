 
// Local includes.
#include "Actor.h"

#include "Draw.h"

#include "Keyboard.h"
#include "Timer.h"

#include "Random.h"
#include "Collision.h"

// 3rd party includes.
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

// std::includes.
#include <algorithm>  // For for_each.
#include <functional> // For mem_fun_ref   in main.
#include <cstdio>     // For file renaming in update_high_score.
#include <iomanip>    // For setw          in arcade_mode.

#include <sstream> // For int -> string conversions.

Timer gameTimer;

bool showFrameTime = false;

float zRot = 0;

bool resize_window( float w, float h )
{
    if( !SDL_SetVideoMode(w, h, 32, SDL_OPENGL|SDL_RESIZABLE) )
        return false;

    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -w/2, w/2, h/2, -h/2, -1000, 1000 );
    glMatrixMode( GL_MODELVIEW );

    return true;
}

GLenum init_gl( int w, int h )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    resize_window( w, h );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );

    return glGetError();
}

#ifdef __WIN32
// Borrowed and modified from http://www.devmaster.net/forums/showthread.php?t=443
void set_vsync( int interval = 1 )
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
    if( ! resize_window(w,h) )
        return false;
    init_gl( w, h );

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

void keyboard_events()
{
}

class Platform : public Square
{
    float r, g, b;

  public:
    typedef Vector<float,2> Vec;

    int growthLeft;

    Platform( const Vec& pos )
    {
        s = pos;
        scale = 0;

        growthLeft = 50;

        r = random( 0.3f, 0.5f );
        g = random( 0.7f, 1.0f );
        b = random( 0.4f, 0.6f );
    }

    void draw()
    {
        float z = scale;

        glPushMatrix();
        glTranslatef( s.x(), s.y(), z );
        
        typedef Vector<float,3> Vec3;

        Vec3 square[] = {
            Vec3( -scale, -scale, z ),
            Vec3(  scale, -scale, z ),
            Vec3(  scale,  scale, z ),
            Vec3( -scale,  scale, z )
        };

        Vec3 wall[] = {
            Vec3( -scale, -scale,   z  ),
            Vec3( -scale, -scale, -100 ),
            Vec3( -scale,  scale, -100 ),
            Vec3( -scale,  scale,   z  )
        };

        draw::Verts< Vec3 > verts( square, 4 );
        draw::Verts< Vec3 > side( wall, 4 );

        glColor3f( r, g, b );
        draw::draw( verts );

        float intensity = ( r + g + b ) / 3;
        intensity *= intensity;

        glColor3f( 0.8*intensity, 0.4*intensity, 0.1*intensity );
        draw::draw( side );

        glRotatef( 90, 0, 0, 1 );
        glColor3f( 0.85*intensity, 0.5*intensity, 0.15*intensity );
        draw::draw( side );

        glColor3f( 0.9*intensity, 0.6*intensity, 0.15*intensity );
        glRotatef( 90, 0, 0, 1 );
        draw::draw( side );

        glColor3f( 0.95*intensity, 0.7*intensity, 0.2*intensity );
        glRotatef( 90, 0, 0, 1 );
        draw::draw( side );

        glPopMatrix();
    }

};

typedef std::vector< Platform > Platforms;
Platforms platforms;

int main( int, char** )
{
    const int IDEAL_FRAME_TIME = Timer::SECOND / 60;
    const int MAX_FRAME_TIME = 3 * IDEAL_FRAME_TIME;

    bool quit = false;
    bool paused = false;

    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
        return 1;
    make_sdl_gl_window( 600, 600 );

    for( int i=0; i < 300; i++ )
    {
        Vector<float,2> pos;
        pos.x( random(-400, 400) );
        pos.y( random(-400, 400) );
        platforms.push_back( Platform( pos ) );
    }

    Timer frameTimer;
    while( quit == false )
    {
        Keyboard::update();

        static SDL_Event event;
		while( SDL_PollEvent(&event) )
		{
            switch( event.type ) 
            {
              case SDL_QUIT: quit = true; break;

              case SDL_KEYDOWN:
                Keyboard::add_key_status( event.key.keysym.sym, Keyboard::PRESSED ); break;

              case SDL_VIDEORESIZE:
                float w=event.resize.w, h=event.resize.h;
                resize_window( w, h );
                break;
            }
        }

        keyboard_events();

        // These lines use local vars thus couldn't be in keyboard_events.
        if( Keyboard::key_state('p') )
            paused = ! paused;
        if( Keyboard::key_state( Keyboard::ESQ ) )
            quit = true;

        float DT = IDEAL_FRAME_TIME * ( 1.f / 4.f );
        //
        // For each time-step:
        static int time = 0;
        for( time += frameTimer.time_ms(); !paused && time >= DT; time -= DT ) 
        {
            if( Keyboard::key_down('q') )
                zRot += 0.1;
            if( Keyboard::key_down('e') )
                zRot -= 0.1;

            for( size_t i=0; i < platforms.size(); i++ )
                if( platforms[i].growthLeft )
                    platforms[i].scale += random( 0.7f, 1.4f );

            for( size_t i=0; i < platforms.size(); i++ )
            {
                for( size_t j=i+1; j < platforms.size(); j++ )
                {
                    if( square_square_collision(platforms[i], platforms[j]) )
                    {
                        if( platforms[i].growthLeft )
                            platforms[i].growthLeft -= 1;
                        if( platforms[j].growthLeft )
                            platforms[j].growthLeft -= 1;

                        const float SCALE = 0.15;

                        // Remove square significantly within another.
                        Square scaledDownI = platforms[i], scaledDownJ = platforms[j];
                        scaledDownI.scale *= SCALE; scaledDownJ.scale *= SCALE;
                        if( square_square_collision(scaledDownI, platforms[j]) )
                            platforms.erase( platforms.begin() + i );
                        else if( square_square_collision(scaledDownJ, platforms[i]) )
                            platforms.erase( platforms.begin() + j );
                    }
                }
            }
        }

        for( size_t i=0; i < platforms.size(); i++ )
            platforms[i].draw();

        static Timer realTimer;
        realTimer.update();
        static int lastUpdate = realTimer.time_ms();
        if( lastUpdate + IDEAL_FRAME_TIME/2 <= realTimer.time_ms() ) {
            update_screen();

            // Rotate the scene for the next run.
            glLoadIdentity();
            glRotatef( 45, 1, 0, 0 );
            glRotatef( 45 + zRot, 0, 0, 1 );
        }
        
        if( paused )
            frameTimer.zero();

        frameTimer.reset();
        frameTimer.clamp_ms( MAX_FRAME_TIME );

        frameTimer.clamp_ms( frameTimer.time_ms() );

        gameTimer.update();
    }

    glFinish();
    SDL_Quit();

    return 0;
}


