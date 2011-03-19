
#include "Platform.h"

#include "Random.h"
#include "Draw.h"

#include <GL/gl.h>

Platform::Platform( const Vec& pos )
{
    s = pos;
    scale = 0;

    growthLeft = 50;

    r = random( 0.2f, 0.4f );
    g = random( 0.7f, 1.0f );
    b = random( 0.4f, 0.5f );
}

void Platform::draw()
{
    float z = height();

    glPushMatrix();
    glTranslatef( s.x(), s.y(), 0 );

    typedef Vector<float,3> Vec3;

    draw::Verts< Vec3 > top {
        Vec3( -scale, -scale, z ),
        Vec3(  scale, -scale, z ),
        Vec3(  scale,  scale, z ),
        Vec3( -scale,  scale, z )
    };

    draw::Verts< Vec3 > side {
        Vec3( -scale, -scale,    z ),
        Vec3( -scale, -scale, -100 ),
        Vec3( -scale,  scale, -100 ),
        Vec3( -scale,  scale,    z )
    };

    glColor3f( r, g, b );
    draw::draw( top );

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

void Platform::add_adjacent( Platform* p )
{
    if( std::find(adjacents.begin(), adjacents.end(), p) == 
        adjacents.end() )
        adjacents.push_back( p );
}

float Platform::height()
{
    return scale * scale / 30;
}
