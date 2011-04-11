
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
        { -scale, -scale, z*SLOPE_H },         // 1
        {  scale, -scale, z*SLOPE_H },         // 2
        {  scale*SLOPE_W, -scale*SLOPE_W, z }, // 4
        { -scale*SLOPE_W, -scale*SLOPE_W, z }, // 3 

        { -scale,  scale, z*SLOPE_H },         // 7
        { -scale, -scale, z*SLOPE_H },         // 1
        { -scale*SLOPE_W, -scale*SLOPE_W, z }, // 3 
        { -scale*SLOPE_W,  scale*SLOPE_W, z }, // 5

        {  scale, -scale, z*SLOPE_H },         // 2
        {  scale,  scale, z*SLOPE_H },         // 8
        {  scale*SLOPE_W,  scale*SLOPE_W, z }, // 6
        {  scale*SLOPE_W, -scale*SLOPE_W, z }, // 4

        {  scale,  scale, z*SLOPE_H },         // 8
        { -scale,  scale, z*SLOPE_H },         // 7
        { -scale*SLOPE_W,  scale*SLOPE_W, z }, // 5
        {  scale*SLOPE_W,  scale*SLOPE_W, z }, // 6

        { -scale*SLOPE_W, -scale*SLOPE_W, z }, // 3 
        {  scale*SLOPE_W, -scale*SLOPE_W, z }, // 4
        {  scale*SLOPE_W,  scale*SLOPE_W, z }, // 6
        { -scale*SLOPE_W,  scale*SLOPE_W, z }, // 5
    };

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

    draw::Verts< Vec3 > side {
        { -scale, -scale, z*SLOPE_H },
        { -scale,  scale, z*SLOPE_H },
        { -scale,  scale,    -100.f },
        { -scale, -scale,    -100.f },

        { -scale, -scale, z*SLOPE_H },
        {  scale, -scale, z*SLOPE_H },
        {  scale, -scale,    -100.f },
        { -scale, -scale,    -100.f },

        {  scale, -scale, z*SLOPE_H },
        {  scale,  scale, z*SLOPE_H },
        {  scale,  scale,    -100.f },
        {  scale, -scale,    -100.f },

        { -scale,  scale, z*SLOPE_H },
        {  scale,  scale, z*SLOPE_H },
        {  scale,  scale,    -100.f },
        { -scale,  scale,    -100.f },
    };

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
