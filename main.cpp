 
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

typedef std::shared_ptr<Jumper> ActorPtr;
typedef std::vector< ActorPtr > Actors;
Actors actors;

struct Dog : public Jumper
{
    static Texture img;

    Dog( Platform* p )
        : Jumper( p )
    {
    }

    Platform* choose_next_plat()
    {
        std::shared_ptr<Player> pl = Player::weakPlayer.lock();

        if( !pl )
            return 0;
        
        Vector<float,3> targetDir3 = pl->s - s;
        Vector<float,2> targetDir( targetDir3.x(), targetDir3.y() );

        Platform* nextPlat = 0;
        float minAngle = 366;
        for( size_t i=0; i < plat->adjacents.size(); i++ ) 
        {
            Vector<float,2> d = plat->adjacents[i]->s - plat->s;

            float angle = angle_between( targetDir, d );
            if( angle < minAngle ) {
                minAngle = angle;
                nextPlat = plat->adjacents[i];
            }
        }

        return nextPlat;
    }

    void on_collide( std::shared_ptr<Jumper> other )
    {
        // Do nothing... for now.
    }

    void draw()
    {
        glPushMatrix();

        glTranslatef( s.x(), s.y(), s.z() );

        glRotatef( -World::zRotDeg, 0, 0, 1 ); // Face the camera.
        glRotatef( 90, 1, 0, 0 ); // Stand up so the XY-plane is vertical.

        // Since X and Y are verticle now, ignore Z.
        // Remember, this is only for the left side.

        draw::Verts<   Vector<float,2> > verts {
            vector( -10.f,  0.f ),
            vector(   0.f,  0.f ),
            vector(   0.f, 30.f ),
            vector( -10.f, 30.f ),

            vector(  10.f,  0.f ),
            vector(   0.f,  0.f ),
            vector(   0.f, 30.f ),
            vector(  10.f, 30.f ),
        };   

        draw::TexCoords< Vector<int,2> > coords {
            vector( 0, 1 ),
            vector( 1, 1 ),
            vector( 1, 0 ),
            vector( 0, 0 ),

            vector( 0, 1 ),
            vector( 1, 1 ),
            vector( 1, 0 ),
            vector( 0, 0 ),
        };

        coords.texture = img.handle();

        glColor3f( 1, 1, 1 );

        draw::draw( verts, coords );

        glPopMatrix();
    }
};
Texture Dog::img;

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

    //Player::weakPlayer = player;

    Player::img.load( "art/Wizzard.bmp" );
    Dog::img.load(    "art/Dog.bmp" );

    Timer frameTimer;
    while( quit == false )
    {
        std::shared_ptr<Player> player = Player::weakPlayer.lock();
        
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

                if( ! growing ) {
                    Platform* randPlat = &platforms[ random(0, platforms.size() ) ];

                    std::shared_ptr<Player> pl( new Player(randPlat) );
                    actors.push_back( pl );
                    Player::weakPlayer = pl;

                    randPlat = &platforms[ random(0, platforms.size() ) ];

                    actors.push_back( ActorPtr(new Dog(randPlat)) );
                }
            } // If growing.
            else
            {
                // Update all.
                for( size_t i=0; i < actors.size(); i++ )
                    actors[i]->move( DT );

                // Check and respond to collisions.
                for( size_t i=0; i < actors.size(); i++ )
                    for( size_t j=i+1; j < actors.size(); j++ )
                        if( actors[i]->plat == actors[j]->plat ) {
                            actors[i]->on_collide( actors[j] );
                            actors[j]->on_collide( actors[i] );
                        }

                if( player && player->plat ) {
                    const float SCALE_FACTOR = 0.02;
                    Screen::scale = player->prevPlat->scale + (player->plat->scale-player->prevPlat->scale)*player->jump_completion();
                    Screen::scale *= SCALE_FACTOR;
                    resize_window( Screen::width, Screen::height, Screen::scale );
                }
            }
        } // For each timestep.

        for( size_t i=0; i < actors.size(); i++ )
            if( actors[i]->deleteMe )
                actors.erase( actors.begin() + i );

        static Timer realTimer;
        realTimer.update();
        static int lastUpdate = 0;
        if( lastUpdate + IDEAL_FRAME_TIME/2 <= realTimer.time_ms() ) 
        {
            lastUpdate = realTimer.time_ms();

            for( size_t i=0; i < platforms.size(); i++ )
                platforms[i].draw();

            for( size_t i=0; i < actors.size(); i++ )
                actors[i]->draw();

            update_screen();
            glLoadIdentity();

            GLfloat camPos[] = { 0, 0, 0, 1 };
            glLightfv( GL_LIGHT1, GL_POSITION, camPos );

            // Rotate the scene for the next run.
            glRotatef(             45, 1, 0, 0 );
            glRotatef( World::zRotDeg, 0, 0, 1 );

            float y = 0;
            float x = 0;
            float z = 0;
            if( player ) {
                y = -player->s.y();
                x = -player->s.x();

                if( player->plat && player->prevPlat  )
                    z = player->prevPlat->s.z() + 
                        (player->plat->s.z()-player->prevPlat->s.z())*player->jump_completion();
                else
                    z = player->s.z();
            }

            // Center the camera on the player.
            glTranslatef( x, y, -z );
        } 
        else // Do not update.
        {
            SDL_Delay( 0 );
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


