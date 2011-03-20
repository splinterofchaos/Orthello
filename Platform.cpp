
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

    r = random( 0.2f, 0.4f );
    g = random( 0.7f, 1.0f );
    b = random( 0.2f, 0.3f );
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
        Vec3( -scale,  scale,    z ),
        Vec3( -scale,  scale, -100 ),
        Vec3( -scale, -scale, -100 ),

        Vec3( -scale, -scale,    z ),
        Vec3(  scale, -scale,    z ),
        Vec3(  scale, -scale, -100 ),
        Vec3( -scale, -scale, -100 ),

        Vec3(  scale, -scale,    z ),
        Vec3(  scale,  scale,    z ),
        Vec3(  scale,  scale, -100 ),
        Vec3(  scale, -scale, -100 ),

        Vec3( -scale,  scale,    z ),
        Vec3(  scale,  scale,    z ),
        Vec3(  scale,  scale, -100 ),
        Vec3( -scale,  scale, -100 ),
    };

    Vec3 topNorms[] = {
        Vec3( -1, -1, 0 ),
        Vec3(  1, -1, 0 ),
        Vec3(  1,  1, 0 ),
        Vec3( -1,  1, 0 )
    };

    Vec3 wallNorms[] = {
        Vec3( -1, -1,  2 ),
        Vec3( -1,  1,  2 ),
        Vec3( -1,  1, -2 ),
        Vec3( -1, -1, -2 ),

        Vec3( -1, -1,  2 ),
        Vec3(  1, -1,  2 ),
        Vec3(  1, -1, -2 ),
        Vec3( -1, -1, -2 ),

        Vec3(  1, -1,  2 ),
        Vec3(  1,  1,  2 ),
        Vec3(  1,  1, -2 ),
        Vec3(  1, -1, -2 ),
        
        Vec3( -1,  1,  2 ),
        Vec3(  1,  1,  2 ),
        Vec3(  1,  1, -2 ),
        Vec3( -1,  1, -2 ),
    };

    glEnableClientState( GL_NORMAL_ARRAY );

    glColor3f( r + lightAdd, g + lightAdd, b + lightAdd );
    glNormalPointer( GL_FLOAT, 0, topNorms );
    draw::draw( top );

    float intensity = ( r + g + b ) / 3;
    intensity *= intensity;

    glColor3f( 0.8*intensity, 0.4*intensity, 0.1*intensity );
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
    return scale * scale / 30;
}
