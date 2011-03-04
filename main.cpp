 
// Local includes.
#include "Actor.h"
#include "Platform.h"

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

#include "Screen.h"

void keyboard_events()
{
}

typedef std::vector< Platform > Platforms;
Platforms platforms;

struct Player
{
    Vector<float,2> s;

    void move( int dt )
    {
    }

    void draw()
    {
    }
};

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

    Player player;

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

            static bool growing = true;

            bool didGrow = false;
            if( growing ) 
            {
                // Grow all.
                for( size_t i=0; i < platforms.size(); i++ ) {

                    if( platforms[i].growthLeft ) { 
                        platforms[i].scale += random( 0.7f, 1.4f );
                        didGrow = true;
                    }
                }

                growing = didGrow;

                // Collide & link all.
                for( size_t i=0; i < platforms.size(); i++ )
                {
                    for( size_t j=i+1; j < platforms.size(); j++ )
                    {
                        if( square_square_collision(platforms[i], platforms[j]) )
                        {
                            // If we stopped growing,
                            if( !growing ) {
                                // We can build the map.
                                platforms[i].add_adjacent( &platforms[j] );
                                platforms[j].add_adjacent( &platforms[i] );
                            }

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
                    } // For platforms[j].
                } // For platforms[i].

                if( ! growing )
                    player.s = platforms[ random(0, platforms.size()) ].s;
            } // If growing.
        } // For each timestep.

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


