
#include "Platform.h"

#include "Random.h"
#include "Draw.h"

#include <GL/gl.h>

Vector<float,2> Platform::unitCircle[ Platform::CIRCLE_SIZE ];
Vector<float,3> Platform::topNorms[ Platform::CIRCLE_SIZE ];
bool Platform::firstInit = true;

Platform::Platform( const Vec& pos )
{
    s = pos;
    scale = 0;

    lightAdd = 0;

    growthLeft = 50;

    r = random( 0.0f, 0.4f );
    g = random( 0.4f, 1.0f );
    b = random( 0.0f, 0.2f );

    if( firstInit )
    {
        for( size_t i=0; i < CIRCLE_SIZE; i++ )
        {
            float theta = float(i)/CIRCLE_SIZE * (2*3.14);
            unitCircle[i] = vector( std::cos(theta), std::sin(theta) );

            topNorms[i] = vector( 0, 0, 1 );
        }

        firstInit = false;
    }
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

    const float SLOPE_H = 0.7;
    const float SLOPE_W = 0.75;

    Vector<float,3> circleT[ CIRCLE_SIZE ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        circleT[i] = vector(
            unitCircle[i].x(), unitCircle[i].y(), 0 
        ) * scale * SLOPE_W;

        circleT[i].z( z );
    }

    Vector<float,3> circle[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        circle[i*2+1].x() = circle[i*2].x() = unitCircle[i].x();
        circle[i*2+1].y() = circle[i*2].y() = unitCircle[i].y();

        circle[i*2+1].x()   *= scale;
        circle[i*2+1].y()   *= scale;
        circle[i*2].x() *= scale * SLOPE_W;
        circle[i*2].y() *= scale * SLOPE_W;

        circle[i*2].z() = z;
        circle[i*2+1].z( z * SLOPE_H );
    }
    circle[ CIRCLE_SIZE*2   ] = circle[0];
    circle[ CIRCLE_SIZE*2+1 ] = circle[1];

    Vector<float,3> sideV[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        sideV[i*2] = sideV[i*2+1] = circle[i*2+1];
        sideV[i*2+1].z() = -100;
    }
    sideV[CIRCLE_SIZE*2] = sideV[0];
    sideV[CIRCLE_SIZE*2+1] = sideV[1];

    Vector<float,3> wallN[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        wallN[i*2].x() = wallN[i*2+1].x() = unitCircle[i].x();
        wallN[i*2].y() = wallN[i*2+1].y() = unitCircle[i].y();
        wallN[i*2].z() = wallN[i*2+1].z() = 0;
    }
    wallN[CIRCLE_SIZE*2]   = wallN[0];
    wallN[CIRCLE_SIZE*2+1] = wallN[1];

    draw::Verts< Vec3 > veryTop( circleT, CIRCLE_SIZE );
    draw::Verts< Vec3 > top( circle, CIRCLE_SIZE*2+2 );
    draw::Verts< Vec3 > side( sideV, CIRCLE_SIZE*2+2 );

    glNormalPointer( GL_FLOAT, 0, circle );
    glColor3f( r + lightAdd, g + lightAdd, b + lightAdd );

    glNormalPointer( GL_FLOAT, 0, topNorms );
    draw::draw( veryTop, GL_POLYGON );

    glEnableClientState( GL_NORMAL_ARRAY );

    draw::draw( top, GL_QUAD_STRIP );

    float intensity = ( r + g + b ) / 3;
    intensity *= 2 * intensity;

    glColor3f( 0.6, 0.5, 0 );
    glNormalPointer( GL_FLOAT, 0, wallN );

    draw::draw( side, GL_TRIANGLE_STRIP );

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
