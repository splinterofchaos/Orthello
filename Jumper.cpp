
#include "Jumper.h"
#include "Keyboard.h"
#include "World.h"

// Helpers.
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

Jumper::Jumper( Platform* p )
{
    prevPlat = plat = p;
    maxJumpCoolDown = 800;
    jumpCoolDown = maxJumpCoolDown;
    deleteMe = false;
}

float Jumper::jump_completion() const
{
    return (maxJumpCoolDown - jumpCoolDown) / maxJumpCoolDown;
}

void Jumper::on_collide( std::shared_ptr<Jumper> other )
{
    deleteMe = true;
}

void Jumper::move( float dt )
{
    if( ! plat ) {
        s.z( 1000 );
        return;
    }

    if( ! prevPlat )
        prevPlat = plat;

    jumpCoolDown = std::max( jumpCoolDown-dt, 0.f );

    if( jumpCoolDown <= 0 ) {
        auto next = choose_next_plat();

        if( next ) {
            darken_plat( plat );

            prevPlat = plat;
            plat     = next;

            maxJumpCoolDown = 600 * plat->height() / prevPlat->height();
            if( maxJumpCoolDown < 100 )
                maxJumpCoolDown = 100;

            jumpCoolDown = maxJumpCoolDown;
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
