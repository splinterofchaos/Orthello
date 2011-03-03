 
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

const int SCREEN_WIDTH  = 900;
const int SCREEN_HEIGHT = 700;

bool resize_window( float w_, float h_ )
{
    float w = w_, h = h_;
    float ratio = (float)SCREEN_HEIGHT / SCREEN_WIDTH;

    if( !SDL_SetVideoMode( w, h, 32, SDL_OPENGL|SDL_RESIZABLE ) )
        return false;

    if( w*ratio > h ) 
        // h is the limiting factor.
        w = h / ratio;
    else
        h = w * ratio;

    float wOff = ( w_ - w ) / 2;
    float hOff = ( h_ - h );

    glViewport( wOff, hOff, w, h );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( -SCREEN_WIDTH/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, -SCREEN_HEIGHT/2, -1000, 1000 );
    glMatrixMode(GL_MODELVIEW);

    return true;
}

GLenum init_gl( int w, int h )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    resize_window( w, h );

    glEnable( GL_BLEND );
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        glPushMatrix();
        glTranslatef( s.x(), s.y(), scale/1000 );
        
        Vec square[] = {
            Vec( -scale, -scale ),
            Vec(  scale, -scale ),
            Vec(  scale,  scale ),
            Vec( -scale,  scale )
        };

        draw::Verts< Vec > verts( square, 4 );

        glColor3f( r, g, b );
        draw::draw( verts );

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
    make_sdl_gl_window( SCREEN_WIDTH, SCREEN_HEIGHT );

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
            for( size_t i=0; i < platforms.size(); i++ )
                if( platforms[i].growthLeft )
                    platforms[i].scale += 1;

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

                        // Remove square significantly within another.
                        Square scaledDownI = platforms[i], scaledDownJ = platforms[j];
                        scaledDownI.scale *= 0.2; scaledDownJ.scale *= 0.2;
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
            glRotatef( 45, 0, 0, 1 );
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


