
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

void Platform::draw_square_plat()
{
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

    float z = height();

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
}

void Platform::draw_circlular_plat()
{
    typedef Vector<float,3> Vec3;

    const float SLOPE_H = 0.7;
    const float SLOPE_W = 0.75;

    float z = height();

    Vector<float,3> topVerts[ CIRCLE_SIZE ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        topVerts[i] = vector(
            unitCircle[i].x(), unitCircle[i].y(), 0 
        ) * scale * SLOPE_W;

        topVerts[i].z( z );
    }

    Vector<float,3> slopeVerts[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        slopeVerts[i*2+1].x() = slopeVerts[i*2].x() = unitCircle[i].x();
        slopeVerts[i*2+1].y() = slopeVerts[i*2].y() = unitCircle[i].y();

        slopeVerts[i*2+1].x()   *= scale;
        slopeVerts[i*2+1].y()   *= scale;
        slopeVerts[i*2].x() *= scale * SLOPE_W;
        slopeVerts[i*2].y() *= scale * SLOPE_W;

        slopeVerts[i*2].z() = z;
        slopeVerts[i*2+1].z( z * SLOPE_H );
    }
    slopeVerts[ CIRCLE_SIZE*2   ] = slopeVerts[0];
    slopeVerts[ CIRCLE_SIZE*2+1 ] = slopeVerts[1];

    Vector<float,3> slopeNorms[ CIRCLE_SIZE * 2 + 2 ];
    float theta = std::atan( SLOPE_H/(scale-SLOPE_W) );
    float xyPart = std::cos( theta );
    float zPart  = std::sin( theta );
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        slopeNorms[i*2] = slopeNorms[i*2+1] = vector (
            unitCircle[i].x()*xyPart, unitCircle[i].y()*xyPart, zPart
        );
    }
    slopeNorms[CIRCLE_SIZE*2] = slopeNorms[CIRCLE_SIZE*2+1] = slopeNorms[0];

    Vector<float,3> sideVerts[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        sideVerts[i*2] = sideVerts[i*2+1] = slopeVerts[i*2+1];
        sideVerts[i*2+1].z() = -100;
    }
    sideVerts[CIRCLE_SIZE*2] = sideVerts[0];
    sideVerts[CIRCLE_SIZE*2+1] = sideVerts[1];

    Vector<float,3> sideNorms[ CIRCLE_SIZE * 2 + 2 ];
    for( size_t i=0; i < CIRCLE_SIZE; i++ )
    {
        sideNorms[i*2].x() = sideNorms[i*2+1].x() = unitCircle[i].x();
        sideNorms[i*2].y() = sideNorms[i*2+1].y() = unitCircle[i].y();
        sideNorms[i*2].z() = sideNorms[i*2+1].z() = 0;
    }
    sideNorms[CIRCLE_SIZE*2]   = sideNorms[0];
    sideNorms[CIRCLE_SIZE*2+1] = sideNorms[1];

    draw::Verts< Vec3 > top(   topVerts, CIRCLE_SIZE );
    draw::Verts< Vec3 > slope( slopeVerts, CIRCLE_SIZE*2+2 );
    draw::Verts< Vec3 > side(  sideVerts, CIRCLE_SIZE*2+2 );

    glEnableClientState( GL_NORMAL_ARRAY );

    glColor3f( r + lightAdd, g + lightAdd, b + lightAdd );
    glNormalPointer( GL_FLOAT, 0, topNorms );
    draw::draw( top, GL_POLYGON );

    glNormalPointer( GL_FLOAT, 0, slopeNorms );
    draw::draw( slope, GL_QUAD_STRIP );

    glColor3f( 0.6, 0.5, 0 );
    glNormalPointer( GL_FLOAT, 0, sideNorms );
    draw::draw( side, GL_TRIANGLE_STRIP );

    glDisableClientState( GL_NORMAL_ARRAY );
}

void Platform::draw()
{
    glPushMatrix();
    glTranslatef( s.x(), s.y(), 0 );

    switch( N_SIDES )
    {
      case CIRCLE: draw_circlular_plat(); break;
      case SQUARE: draw_square_plat();    break;
      default: break;
    }

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

bool Platform::collide( Platform& p1, Platform& p2, float fuzziness )
{
    bool intersecting = false;

    Vector<float,2> diff = p2.s - p1.s;
    float dist = magnitude( diff );

    // Comb(ined) scale.
    float combScale = p1.scale + p2.scale;
    combScale *= fuzziness;

    switch( N_SIDES ) 
    {
      case CIRCLE: intersecting = dist < combScale; break;

      case SQUARE: 
        intersecting = std::abs(diff.x()) < combScale
                    && std::abs(diff.y()) < combScale;
        break;

      default: break;
    }

    return intersecting;
}
