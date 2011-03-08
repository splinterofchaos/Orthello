 
// Local includes.
#include "Actor.h"
#include "Platform.h"

#include "Draw.h"
#include "Texture.h"

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

float zRotDeg = 0;

#include "Screen.h"

void keyboard_events()
{
}

typedef std::vector< Platform > Platforms;
Platforms platforms;

struct Player
{
    static const int JUMP_COOLDOWN = 500;

    static Texture img;

    Vector<float,3> s;
    Platform *plat, *prevPlat;

    int jumpCoolDown;

    Player()
    {
        plat = 0;
        jumpCoolDown = JUMP_COOLDOWN;
    }

    void move( int dt )
    {
        if( ! plat )
            return;

        jumpCoolDown = std::max( jumpCoolDown-dt, 0 );

        if( jumpCoolDown <= 0 ) {
            Vector<float,2> input( 0, 0 );

            if( Keyboard::key_down('w') )
                input.y() += -1;
            if( Keyboard::key_down('s') )
                input.y() +=  1;
            if( Keyboard::key_down('a') )
                input.x() += -1;
            if( Keyboard::key_down('d') )
                input.x() +=  1;

            if( magnitude(input) > 0.01f ) 
            {
                // Input is rotated to match perspective.
                Vector<float,2> direction;

                float rotate = -zRotDeg * 3.14f/180.f;
                direction.x( std::cos(rotate)*input.x() - std::sin(rotate)*input.y() );
                direction.y( std::sin(rotate)*input.x() + std::cos(rotate)*input.y() );

                // Find platform most in the direction the player is facing.
                Platform* nextPlat = 0;
                float minAngle = 366;
                for( size_t i=0; i < plat->adjacents.size(); i++ ) 
                {
                    Vector<float,2> d = plat->adjacents[i]->s - plat->s;

                    float angle = angle_between( direction, d );
                    if( angle < minAngle ) {
                        minAngle = angle;
                        nextPlat = plat->adjacents[i];
                    }
                }

                if( nextPlat && minAngle < 3.14 / 4 ) {
                    prevPlat = plat;
                    plat     = nextPlat;
                    jumpCoolDown = JUMP_COOLDOWN;
                }
            }
        }

        float t = JUMP_COOLDOWN - jumpCoolDown;
        float dz = plat->height() - prevPlat->height();

        Vector<float,2> d2 = plat->s - prevPlat->s;
        Vector<float,3> s0( prevPlat->s.x(), prevPlat->s.y(), prevPlat->height() );
        Vector<float,3> d3( d2.x(), d2.y(), dz ); 

        s = s0 + d3 * ( t / JUMP_COOLDOWN );

        float tmp = std::sin(3.14*t/JUMP_COOLDOWN);
        s.z() += 75 * std::sqrt(tmp);
    }

    void draw()
    {
        // The image for the player is only the left side so the strategy is:
        // Draw the left, then rotate the perspective and draw it again.

        glPushMatrix();

        glTranslatef( s.x(), s.y(), s.z() );

        glRotatef( -zRotDeg, 0, 0, 1 ); // Face the camera.
        glRotatef(       90, 1, 0, 0 ); // Stand up so the XY-plane is vertical.

        // Since X and Y are verticle now, ignore Z.
        // Remember, this is only for the left side.
        Vector<float,2> tmpVerts[] = {
            vector( -10.f,  0.f ),
            vector(   0.f,  0.f ),
            vector(   0.f, 50.f ),
            vector( -10.f, 50.f ),
        };

        Vector<int,2> tmpCoord[] = {
            vector( 0, 1 ),
            vector( 1, 1 ),
            vector( 1, 0 ),
            vector( 0, 0 )
        };

        draw::Verts<   Vector<float,2> > verts( tmpVerts, 4 );
        draw::TexCoords< Vector<int,2> > coords( tmpCoord, img.handle(), 4 );

        glColor3f( 1, 1, 1 );

        draw::draw( verts, coords );
        glRotatef( 180, 0, 1, 0 ); // Flip to the other side.
        draw::draw( verts, coords );

        glPopMatrix();
    }
};
Texture Player::img;

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
                zRotDeg += 0.1;
            if( Keyboard::key_down('e') )
                zRotDeg -= 0.1;

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

                            const float SCALE = 0.10;

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
                    player.plat = &platforms[ random(0, platforms.size()) ];
            } // If growing.
            else
            {
                player.move( DT );
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

            // Rotate the scene for the next run.
            glLoadIdentity();
            glRotatef(   45, 1, 0, 0 );
            glRotatef( zRotDeg, 0, 0, 1 );
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


