
#pragma once

#include "Vector.h"
#include "Collision.h"

#include <vector>

class Platform 
{
    static const unsigned int CIRCLE_SIZE = 60;
    static Vector<float,2> unitCircle[ CIRCLE_SIZE ];
    static Vector<float,3> topNorms[ CIRCLE_SIZE ];

    enum Shape 
    {
        CIRCLE = 1,
        SQUARE = 4,
    };

    static const unsigned int N_SIDES = Shape::CIRCLE;

    static bool firstInit;

  public:
    typedef Vector<float,2> Vec;

    std::vector< Platform* > adjacents;

    Vec s;

    int growthLeft;
    float scale;
    float r, g, b;
    float lightAdd;

    Platform( const Vec& pos );

    void draw();
    void add_adjacent( Platform* );
    float height();

    void draw_square_plat();
    void draw_circlular_plat();
    static bool collide( Platform& p1, Platform& p2, float fuzziness=1 );
};
