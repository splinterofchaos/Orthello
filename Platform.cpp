
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

    // TOP MAP
    // 0---14--5
    // |   |   |
    // |   |   |
    // 38-279C-6D
    // |   |   |
    // |   |   |
    // B--7AF--E

    draw::Verts< Vec3 > top {
        Vec3( -scale, -scale, z ), // 0
        Vec3(      0, -scale, z ), // 1
        Vec3(      0,      0, z ), // 2
        Vec3( -scale,      0, z ), // 3
        Vec3(      0, -scale, z ), // 4
        Vec3(  scale, -scale, z ), // 5
        Vec3(  scale,      0, z ), // 6
        Vec3(      0,      0, z ), // 7
        Vec3( -scale,      0, z ), // 8
        Vec3(      0,      0, z ), // 9
        Vec3(      0,  scale, z ), // A
        Vec3( -scale,  scale, z ), // B
        Vec3(      0,      0, z ), // C
        Vec3(  scale,      0, z ), // D
        Vec3(  scale,  scale, z ), // E
        Vec3(      0,  scale, z ), // F
    };

    Vec3 topNorms[] = {
        Vec3( -1, -1, 0 ), // 0
        Vec3(  0, -1, 0.5 ), // 1
        Vec3(  0,  0, 1 ), // 2
        Vec3( -1,  0, 0 ), // 3
        Vec3(  0, -1, 0.5 ), // 4
        Vec3(  1, -1, 0 ), // 5
        Vec3(  1,  0, 0.5 ), // 6
        Vec3(  0,  0, 1 ), // 7
        Vec3( -1,  1, 0 ), // 8
        Vec3(  0,  0, 1 ), // 9
        Vec3(  0,  1, 0.5 ), // A
        Vec3( -1,  1, 0 ), // B
        Vec3(  0,  0, 1 ), // C
        Vec3(  1,  0, 0.5 ), // D
        Vec3(  1,  1, 0 ), // E
        Vec3(  0,  1, 0.5 ), // F
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
