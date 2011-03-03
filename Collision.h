
#include "Vector.h"

#pragma once

struct Square
{
    Vector<float,2> s;
    float scale;
};

bool is_between( float x, float min, float max );
bool intersecting_range( float min1, float max1, float min2, float max2 );

bool square_square_collision( const Square& s1, const Square& s2 );
bool point_square_collision( const Vector<float,2>& p, const Square& s );
