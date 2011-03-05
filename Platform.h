
#include "Vector.h"
#include "Collision.h"

#include <vector>

class Platform : public Square
{
  public:
    typedef Vector<float,2> Vec;

    std::vector< Platform* > adjacents;

    int growthLeft;
    float r, g, b;

    Platform( const Vec& pos );

    void draw();
    void add_adjacent( Platform* );
    float height();
};
