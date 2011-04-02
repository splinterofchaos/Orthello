 
#include "Player.h"

#include "Keyboard.h"
#include "Draw.h"

#include "World.h"

Texture Player::img;
std::weak_ptr< Player > Player::weakPlayer;

Player::Player( Platform* p )
{
    prevPlat = plat = p;
    maxJumpCoolDown = 800;
    jumpCoolDown = maxJumpCoolDown;
}

void lighten_plat( Platform* plat, float brighten )
{
    if( brighten > 0.01 && plat->lightAdd < brighten ) {
        plat->lightAdd = brighten;
        brighten -= 0.5;

        for( size_t i=0; i < plat->adjacents.size(); i++ )
            lighten_plat( plat->adjacents[i], brighten );
    }
}

void darken_plat( Platform* plat )
{
    if( plat->lightAdd > 0.01 ) {
        plat->lightAdd = 0;

        for( size_t i=0; i < plat->adjacents.size(); i++ )
            darken_plat( plat->adjacents[i] );
    }
}

void Player::move( float dt )
{
    if( ! plat ) {
        s.z( 1000 );
        return;
    }

    if( ! prevPlat )
        prevPlat = plat;

    jumpCoolDown = std::max( jumpCoolDown-dt, 0.f );

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

            float rotate = -World::zRotDeg * 3.14f/180.f;
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
                darken_plat( plat );

                prevPlat = plat;
                plat     = nextPlat;

                maxJumpCoolDown = 600 * plat->height() / prevPlat->height();
                if( maxJumpCoolDown < 100 )
                    maxJumpCoolDown = 100;

                jumpCoolDown = maxJumpCoolDown;
            }
        }
    }

    float deltaZ = plat->height() - prevPlat->height();
    float dz     = plat->height() / prevPlat->height();

    Vector<float,2> d2 = plat->s - prevPlat->s;
    Vector<float,3> s0( prevPlat->s.x(), prevPlat->s.y(), prevPlat->height() );
    Vector<float,3> d3( d2.x(), d2.y(), deltaZ ); 

    s = s0 + d3 * jump_completion();

    float tmp = std::sin( 3.14 * jump_completion() );
    s.z() += 75 * dz * std::sqrt(tmp);

    lighten_plat( plat, 0.9 );
}


void Player::draw()
{
    // The image for the player is only the left side so the strategy is:
    // Draw the left, then rotate the perspective and draw it again.

    glPushMatrix();

    glTranslatef( s.x(), s.y(), s.z() );

    glRotatef( -World::zRotDeg, 0, 0, 1 ); // Face the camera.
    glRotatef( 90, 1, 0, 0 ); // Stand up so the XY-plane is vertical.

    // Since X and Y are verticle now, ignore Z.
    // Remember, this is only for the left side.

    draw::Verts<   Vector<float,2> > verts {
        vector( -10.f,  0.f ),
        vector(   0.f,  0.f ),
        vector(   0.f, 50.f ),
        vector( -10.f, 50.f ),

        vector(  10.f,  0.f ),
        vector(   0.f,  0.f ),
        vector(   0.f, 50.f ),
        vector(  10.f, 50.f ),
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


float Player::jump_completion() const
{
    return (maxJumpCoolDown - jumpCoolDown) / maxJumpCoolDown;
}
