
#include "Platform.h"
#include "Vector.h"

#include <memory>

struct Jumper
{
    Vector<float,3> s;
    Platform *plat, *prevPlat;

    float jumpCoolDown;
    float maxJumpCoolDown;

    bool deleteMe;

    Jumper( Platform* p );

    virtual Platform* choose_next_plat() = 0;
    float jump_completion() const;

    virtual void on_collide( std::shared_ptr<Jumper> other );

    virtual void move( float dt );
    virtual void draw() = 0;
};

void lighten_plat( Platform* plat, float brighten );

void darken_plat( Platform* plat );

