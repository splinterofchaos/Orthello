
#pragma once

#include "Vector.h"
#include "Collision.h"

#include <vector>

class Platform : public Square
{
    static const unsigned int CIRCLE_SIZE = 20;
    static Vector<float,2> unitCircle[ CIRCLE_SIZE ];

    static bool firstInit;

  public:
    typedef Vector<float,2> Vec;

    std::vector< Platform* > adjacents;

    int growthLeft;
    float r, g, b;
    float lightAdd;

    Platform( const Vec& pos );

    void draw();
    void add_adjacent( Platform* );
    float height();
};
