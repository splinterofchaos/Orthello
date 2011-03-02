
#include "Vector.h"

#pragma once

struct Square
{
    Vector<float,2> s;
    float scale;
};

bool square_square_collision( const Square& s1, const Square& s2 );
