
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
    //
    // Top view   Side view
    // 1-----2      _____  -
    // |\   /|     /     \ | SLOPE_H
    // | 3-4 |     ------- -
    // | | | |    |-|
    // | 5-6 |    SLOPE_W
    // |/   \|
    // 7_____8

    const float SLOPE_H = 0.6;
    const float SLOPE_W = 0.5;

    draw::Verts< Vec3 > top {
        Vec3( -scale, -scale, z*SLOPE_H ),         // 1
        Vec3(  scale, -scale, z*SLOPE_H ),         // 2
        Vec3(  scale*SLOPE_W, -scale*SLOPE_W, z ), // 4
        Vec3( -scale*SLOPE_W, -scale*SLOPE_W, z ), // 3 

        Vec3( -scale,  scale, z*SLOPE_H ),         // 7
        Vec3( -scale, -scale, z*SLOPE_H ),         // 1
        Vec3( -scale*SLOPE_W, -scale*SLOPE_W, z ), // 3 
        Vec3( -scale*SLOPE_W,  scale*SLOPE_W, z ), // 5

        Vec3(  scale, -scale, z*SLOPE_H ),         // 2
        Vec3(  scale,  scale, z*SLOPE_H ),         // 8
        Vec3(  scale*SLOPE_W,  scale*SLOPE_W, z ), // 6
        Vec3(  scale*SLOPE_W, -scale*SLOPE_W, z ), // 4

        Vec3(  scale,  scale, z*SLOPE_H ),         // 8
        Vec3( -scale,  scale, z*SLOPE_H ),         // 7
        Vec3( -scale*SLOPE_W,  scale*SLOPE_W, z ), // 5
        Vec3(  scale*SLOPE_W,  scale*SLOPE_W, z ), // 6

        Vec3( -scale*SLOPE_W, -scale*SLOPE_W, z ), // 3 
        Vec3(  scale*SLOPE_W, -scale*SLOPE_W, z ), // 4
        Vec3(  scale*SLOPE_W,  scale*SLOPE_W, z ), // 6
        Vec3( -scale*SLOPE_W,  scale*SLOPE_W, z ), // 5
    };

    Vec3 topNorms[] = {
        Vec3( 0, -1, 0 ), // 1
        Vec3( 0, -1, 0 ), // 2
        Vec3( 0, -1, 0.5 ), // 4
        Vec3( 0, -1, 0.5 ), // 3 

        Vec3( -1, 0, 0 ), // 7
        Vec3( -1, 0, 0 ), // 1
        Vec3( -1, 0, 0.5 ), // 3 
        Vec3( -1, 0, 0.5 ), // 5

        Vec3( 1, 0, 0 ), // 2
        Vec3( 1, 0, 0 ), // 8
        Vec3( 1, 0, 0.5 ), // 6
        Vec3( 1, 0, 0.5 ), // 4

        Vec3( 0, 1, 0 ), // 8
        Vec3( 0, 1, 0 ), // 7
        Vec3( 0, 1, 0.5 ), // 5
        Vec3( 0, 1, 0.5 ), // 6

        Vec3( 0, 0, 1 ), // 3 
        Vec3( 0, 0, 1 ), // 4
        Vec3( 0, 0, 1 ), // 5
        Vec3( 0, 0, 1 ), // 6
    };

    draw::Verts< Vec3 > side {
        Vec3( -scale, -scale, z*SLOPE_H ),
        Vec3( -scale,  scale, z*SLOPE_H ),
        Vec3( -scale,  scale,      -100 ),
        Vec3( -scale, -scale,      -100 ),

        Vec3( -scale, -scale, z*SLOPE_H ),
        Vec3(  scale, -scale, z*SLOPE_H ),
        Vec3(  scale, -scale,      -100 ),
        Vec3( -scale, -scale,      -100 ),

        Vec3(  scale, -scale, z*SLOPE_H ),
        Vec3(  scale,  scale, z*SLOPE_H ),
        Vec3(  scale,  scale,      -100 ),
        Vec3(  scale, -scale,      -100 ),

        Vec3( -scale,  scale, z*SLOPE_H ),
        Vec3(  scale,  scale, z*SLOPE_H ),
        Vec3(  scale,  scale,      -100 ),
        Vec3( -scale,  scale,      -100 ),
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
