
#pragma once

#include "Actor.h"
#include "Texture.h"
#include "Platform.h"

#include <memory> // For weak_ptr.

struct Player
{
    static std::weak_ptr< Player > weakPlayer;
    static Texture img;

    Vector<float,3> s;
    Platform *plat, *prevPlat;

    float jumpCoolDown;
    float maxJumpCoolDown;

    Player( Platform* p );

    Platform* choose_next_plat();
    
    void move( float dt );

    void draw();

    float jump_completion() const;
};
