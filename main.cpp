 
// Local includes.
#include "Actor.h"
#include "Player.h"
#include "Platform.h"

#include "World.h"

#include "Draw.h"
#include "Texture.h"
#include "Screen.h"

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

void keyboard_events()
{
}

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
    if( make_sdl_gl_window( Screen::width, Screen::height ) != GL_NO_ERROR )
        return 2;

    int w = 1000; // The width of one side of the area to spawn plats.
    for( int i=0; i < w/2; i++ )
    {
        Vector<float,2> pos;
        pos.x( random(-w/2, w/2) );
        pos.y( random(-w/2, w/2) );
        platforms.push_back( Platform( pos ) );
    }

    Player player;

    Player::img.load( "art/Wizzard.bmp" );

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
                Screen::width = w; Screen::height = h;
                break;
                // The window will be resized later.
            }
        }

        keyboard_events();

        // These lines use local vars thus couldn't be in keyboard_events.
        if( Keyboard::key_state('p') )
            paused = ! paused;
        if( Keyboard::key_state( Keyboard::ESQ ) )
            quit = true;

        float DT = IDEAL_FRAME_TIME * ( 1.f / 4.f );

        // For each time-step:
        static int time = 0;
        for( time += frameTimer.time_ms(); !paused && time >= DT; time -= DT ) 
        {
            if( Keyboard::key_down('q') )
                World::zRotDeg += 0.1;
            if( Keyboard::key_down('e') )
                World::zRotDeg -= 0.1;

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
                        if( Platform::collide(platforms[i], platforms[j], 1.1) )
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

                            if( Platform::collide(platforms[i], platforms[j], 0.6) )
                                platforms.erase( platforms.begin() + i );
                        }
                    } // For platforms[j].
                } // For platforms[i].

                if( ! growing )
                    player.plat = &platforms[ random(0, platforms.size()) ];
            } // If growing.
            else
            {
                player.move( DT );

                if( player.plat ) {
                    const float SCALE_FACTOR = 0.02;
                    Screen::scale = player.prevPlat->scale + (player.plat->scale-player.prevPlat->scale)*player.jump_completion();
                    Screen::scale *= SCALE_FACTOR;
                    resize_window( Screen::width, Screen::height, Screen::scale );
                }
            }
        } // For each timestep.

        for( size_t i=0; i < platforms.size(); i++ )
            platforms[i].draw();

        player.draw();

        static Timer realTimer;
        realTimer.update();
        static int lastUpdate = realTimer.time_ms();
        if( lastUpdate + IDEAL_FRAME_TIME/2 <= realTimer.time_ms() ) {
            update_screen();

            glLoadIdentity();

            GLfloat camPos[] = { 0, 0, 0, 1 };
            glLightfv( GL_LIGHT1, GL_POSITION, camPos );

            // Rotate the scene for the next run.
            glRotatef(             45, 1, 0, 0 );
            glRotatef( World::zRotDeg, 0, 0, 1 );

            float z;
            if( player.plat && player.prevPlat  )
                z = player.prevPlat->s.z() + (player.plat->s.z()-player.prevPlat->s.z())*player.jump_completion();
            else
                z = player.s.z();

            // Center the camera on the player.
            glTranslatef( -player.s.x(), -player.s.y(), -z );
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


