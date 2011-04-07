
#pragma once

#include "Jumper.h"
#include "Texture.h"
#include "Platform.h"

#include <memory> // For weak_ptr.

struct Player : public Jumper
{
    static std::weak_ptr< Player > weakPlayer;
    static Texture img;

    Player( Platform* p );

    Platform* choose_next_plat();
    
    void draw();

    float jump_completion() const;
};
