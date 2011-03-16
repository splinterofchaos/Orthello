
#pragma once

#include "Actor.h"
#include "Texture.h"
#include "Platform.h"

struct Player
{
    static Texture img;

    Vector<float,3> s;
    Platform *plat, *prevPlat;

    float jumpCoolDown;
    float maxJumpCoolDown;

    Player();

    void move( float dt );

    void draw();

    float jump_completion() const;
};

