#pragma once



#include <fixed_point.h>

using namespace fixedpoint_literals;



enum class PlayerState : uint8_t {
    Grounded,
    Rising,
    Falling,
};

// Defines a basic object which can move on the screen.
// Floating point numbers on the NES are extremely slow and take up too much space since theres
// no hardware support for them, but we can get something similar by using fixed point numbers instead.
// A fixed point number stores a "integer" part and a "fractional" part, where the integer part represents
// everything to the left of the decimal place, and the fractional part is the right of the decimal place.
struct Player {
    // The data type `fu8_8` can be read as `fixed point unsigned number with 8 bits integer and 8 bits fractional`
    // This lets us smooth out the movement a bit as 1px is often too slow, and 2px is often too fast!
    fu8_8 x;
    fu8_8 y;
    // For a basic platformer, it feels really "cheap" if theres no acceleration. We can solve this by
    // tracking the object velocity and increasing the X position by the velocity each frame instead.
    fs8_8 vel_x;
    fs8_8 vel_y;
    // Store what state the player is in (grounded, rising, falling, etc)
    PlayerState state;
    // Store how many frames its been since we started a jump
    uint8_t jump_timer;
    // As soon as the player releases the jump button we'll start applying gravity (or whenever we reach max jump)
    bool released_jump;
    // Keep track of the current animation frame for the sprite
    uint8_t frame;
    // and how long we've been in the current animation
    uint8_t animation_count;
};


// Single global player instance.
// NOTE: This is declared 'extern' here and defined once in player.cpp to avoid
// creating a separate copy in every translation unit that includes this header.
extern Player player;

#ifdef __cplusplus
extern "C" {
#endif

void update_player_position();

#ifdef __cplusplus
}
#endif
