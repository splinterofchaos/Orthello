
#include "Platform.h"

#include "Random.h"
#include "Draw.h"

#include <GL/gl.h>

Platform::Platform( const Vec& pos )
{
    s = pos;
    scale = 0;

    lightAdd = 0;

    growthLeft = 50;

    r = random( 0.0f, 0.4f );
    g = random( 0.4f, 1.0f );
    b = random( 0.0f, 0.2f );
}

void Platform::draw()
{
    float z = height();

    glPushMatrix();
    glTranslatef( s.x(), s.y(), 0 );

    typedef Vector<float,3> Vec3;

    // TOP MAP
    // 0-1-2
    // | | |
    // 3-4-5
    // | | |
    // 6-7-8

    draw::Verts< Vec3 > top {
        Vec3(      0,      0, z     ), // 4
        Vec3( -scale,      0, z*0.8 ), // 3
        Vec3( -scale, -scale, z*0.8 ), // 0
        Vec3(      0, -scale, z*0.8 ), // 1

        Vec3(      0,      0, z     ), // 4
        Vec3(      0, -scale, z*0.8 ), // 1
        Vec3(  scale, -scale, z*0.8 ), // 2
        Vec3(  scale,      0, z*0.8 ), // 5

        Vec3(      0,      0, z     ), // 4
        Vec3(      0,  scale, z*0.8 ), // 7
        Vec3( -scale,  scale, z*0.8 ), // 6
        Vec3( -scale,      0, z*0.8 ), // 3

        Vec3(      0,      0, z     ), // 4
        Vec3(  scale,      0, z*0.8 ), // 5
        Vec3(  scale,  scale, z*0.8 ), // 8
        Vec3(      0,  scale, z*0.8 ), // 7
    };

    Vec3 topNorms[] = {
        Vec3(  0,  0,   1 ), // 4
        Vec3( -1,  0, 0.5 ), // 3
        Vec3( -1, -1, 0.5 ), // 0
        Vec3(  0, -1, 0.5 ), // 1
                                 
        Vec3(  0,  0,   1 ), // 4
        Vec3(  0, -1, 0.5 ), // 1
        Vec3(  1, -1, 0.5 ), // 2
        Vec3(  1,  0, 0.5 ), // 5
                                 
        Vec3(  0,  0,   1 ), // 4
        Vec3(  0,  1, 0.5 ), // 7
        Vec3( -1,  1, 0.5 ), // 6
        Vec3( -1,  0, 0.5 ), // 3
                                 
        Vec3(  0,  0,   1 ), // 4
        Vec3(  1,  0, 0.5 ), // 5
        Vec3(  1,  1, 0.5 ), // 8
        Vec3(  0,  1, 0.5 ), // 7
    };

    draw::Verts< Vec3 > side {
        Vec3( -scale, -scale, z*0.8 ),
        Vec3( -scale,  scale, z*0.8 ),
        Vec3( -scale,  scale,  -100 ),
        Vec3( -scale, -scale,  -100 ),

        Vec3( -scale, -scale, z*0.8 ),
        Vec3(  scale, -scale, z*0.8 ),
        Vec3(  scale, -scale,  -100 ),
        Vec3( -scale, -scale,  -100 ),

        Vec3(  scale, -scale, z*0.8 ),
        Vec3(  scale,  scale, z*0.8 ),
        Vec3(  scale,  scale,  -100 ),
        Vec3(  scale, -scale,  -100 ),

        Vec3( -scale,  scale, z*0.8 ),
        Vec3(  scale,  scale, z*0.8 ),
        Vec3(  scale,  scale,  -100 ),
        Vec3( -scale,  scale,  -100 ),
    };

    Vec3 wallNorms[] = {
        Vec3( -1, 0, 0 ),
        Vec3( -1, 0, 0 ),
        Vec3( -1, 0, 0 ),
        Vec3( -1, 0, 0 ),

        Vec3( 0, -1, 0 ),
        Vec3( 0, -1, 0 ),
        Vec3( 0, -1, 0 ),
        Vec3( 0, -1, 0 ),

        Vec3(  1, 0, 0 ),
        Vec3(  1, 0, 0 ),
        Vec3(  1, 0, 0 ),
        Vec3(  1, 0, 0 ),
        
        Vec3( 0,  1, 0 ),
        Vec3( 0,  1, 0 ),
        Vec3( 0,  1, 0 ),
        Vec3( 0,  1, 0 ),
    };

    glEnableClientState( GL_NORMAL_ARRAY );

    glColor3f( r + lightAdd, g + lightAdd, b + lightAdd );
    glNormalPointer( GL_FLOAT, 0, topNorms );

    draw::draw( top );

    float intensity = ( r + g + b ) / 3;
    intensity *= 2 * intensity;

    glColor3f( 0.6, 0.5, 0 );
    glNormalPointer( GL_FLOAT, 0, wallNorms );

    draw::draw( side );

    glDisableClientState( GL_NORMAL_ARRAY );

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
    return scale * scale / 20;
}
