
#include "Collision.h"

bool is_between( float x, float min, float max )
{
    return x >= min || x <= max;
}

bool intersecting_range( float min1, float max1, float min2, float max2 )
{
    return is_between( min1, min2, max2 ) || is_between( max1, min2, max2 );
}

bool square_square_collision( const Square& s1, const Square& s2 )
{
    Vector<float,2> d = s1.s - s2.s;
    float scale = (s1.scale + s2.scale) / 2;

    return std::abs(d.x()) > scale && std::abs(d.y()) > scale;
}
