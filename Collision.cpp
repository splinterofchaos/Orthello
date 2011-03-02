
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
    float minXs1 = s1.s.x() - s1.scale/2;
    float maxXs1 = minXs1 + s1.scale;
    float minYs1 = s1.s.y() - s1.scale/2;
    float maxYs1 = minYs1 + s1.scale;

    float minXs2 = s2.s.x() - s2.scale/2;
    float maxXs2 = minXs2 + s2.scale;
    float minYs2 = s2.s.y() - s2.scale/2;
    float maxYs2 = minYs2 + s2.scale;

    return intersecting_range( minXs1, maxXs1, minXs2, maxXs2 ) &&
           intersecting_range( minYs1, maxYs1, minYs2, maxYs2 );
}
