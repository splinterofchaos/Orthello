
#include "Platform.h"

#include "Random.h"
#include "Draw.h"

#include <GL/gl.h>

Platform::Vec Platform::unitCircle[ Platform::CIRCLE_SIZE ];
bool  Platform::firstInit = true;

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

    const float SLOPE_H = 0.6;
    const float SLOPE_W = 0.5;

    Vector<float,3> circleT[ CIRCLE_SIZE ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        circleTop[i] = vector(
            unitCircle[i].x(), unitCircle[i].y(), z 
        );
    }

    Vector<float,3> circle[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        circle[i*2+1].x() circle[i*2] = unitCircle[i].x();
        circle[i*2+1].y() circle[i*2] = unitCircle[i].y();
        circle[i*2] = z;
        circle[i*2+1].z( z * SLOPE_H );
    }
    circle[ CIRCLE_SIZE   ] = circle[0];
    circle[ CIRCLE_SIZE+1 ] = circle[1];

    Vector<float,3> sideV[ CIRCLE_SIZE * 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        sideV[i*2] = sideV[i*2+1] = circle[i+1];
        sideV[i*2+1].z() = -100;
    }

    Vector<float,3> wallN[ CIRCLE_SIZE * 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        wallN[i*2].x() = wallN[i*2+1].x() = unitCircle[i].x();
        wallN[i*2].y() = wallN[i*2+1].y() = unitCircle[i].y();
        wallN[i*2].z() = wallN[i*2+1].z() = 0;
    }

    draw::Verts< Vec3 > veryTop( circleT, CIRCLE_SIZE );
    draw::Verts< Vec3 > top( circle, CIRCLE_SIZE+2 );

    Vec3 topNorms[] = {
        { 0.f, -1.f, 0.0f }, // 1
        { 0.f, -1.f, 0.0f }, // 2
        { 0.f, -1.f, 0.5f }, // 4
        { 0.f, -1.f, 0.5f }, // 3 

        { -1.f, 0.f, 0.0f }, // 7
        { -1.f, 0.f, 0.0f }, // 1
        { -1.f, 0.f, 0.5f }, // 3 
        { -1.f, 0.f, 0.5f }, // 5

        { 1.f, 0.f, 0.0f }, // 2
        { 1.f, 0.f, 0.0f }, // 8
        { 1.f, 0.f, 0.5f }, // 6
        { 1.f, 0.f, 0.5f }, // 4

        { 0.f, 1.f, 0.0f }, // 8
        { 0.f, 1.f, 0.0f }, // 7
        { 0.f, 1.f, 0.5f }, // 5
        { 0.f, 1.f, 0.5f }, // 6

        { 0.f, 0.f, 1.f }, // 3 
        { 0.f, 0.f, 1.f }, // 4
        { 0.f, 0.f, 1.f }, // 5
        { 0.f, 0.f, 1.f }, // 6
    };

    draw::Verts< Vec3 > side( sideV, CIRCLE_SIZE*2 );

    Vector<float,3> wallNorms[] = {
        { -1.f,  0.f,  0.f },
        { -1.f,  0.f,  0.f },
        { -1.f,  0.f,  0.f },
        { -1.f,  0.f,  0.f },

        {  0.f, -1.f,  0.f },
        {  0.f, -1.f,  0.f },
        {  0.f, -1.f,  0.f },
        {  0.f, -1.f,  0.f },

        {  1.f,  0.f,  0.f },
        {  1.f,  0.f,  0.f },
        {  1.f,  0.f,  0.f },
        {  1.f,  0.f,  0.f },
        
        {  0.f,  1.f,  0.f },
        {  0.f,  1.f,  0.f },
        {  0.f,  1.f,  0.f },
        {  0.f,  1.f,  0.f },
    };

    glEnableClientState( GL_NORMAL_ARRAY );

    glColor3f( r + lightAdd, g + lightAdd, b + lightAdd );

    glNormalPointer( GL_FLOAT, 0, circleT );
    draw::draw( veryTop, GL_POLYGON );

    glNormalPointer( GL_FLOAT, 0, circle );
    draw::draw( top, GL_TRIANGLE_STRIP );

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
