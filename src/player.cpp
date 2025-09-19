
// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <neslib.h>

// Helper class for compiler generated fixed point math, a simplified replacement for floating point numbers
#include <fixed_point.h>

using namespace fixedpoint_literals;

// Basic drawing to demonstrate metasprite support
const int8_t character_metasprite_frame0[]={
	- 4,-11,0x0f,0,
	  0,  3,0x06,0,
	- 2,- 4,0x05,0,
	  1,- 5,0x06,0,
	- 9,- 5,0x09,0,
	- 7,  3,0x09,0,
	(int8_t)0x80
};
const int8_t character_metasprite_frame1[]={
	- 4,-11,0x0f,0,
	  0,  3,0x06,0,
	- 2,- 4,0x05,0,
	  2,- 6,0x06,(int8_t)(0|OAM_FLIP_V),
	-10,- 6,0x09,(int8_t)(0|OAM_FLIP_V),
	- 7,  3,0x09,0,
	(int8_t)0x80
};

const int8_t* character_metasprites[] = {
    character_metasprite_frame0,
    character_metasprite_frame1,
};

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


// Define a player!
static Player player {
    .x = 120, .y = 200,
    .vel_x = 0, .vel_y = 0,
    .state = PlayerState::Grounded,
    .jump_timer = 0,
    .released_jump = false,
    .frame = 0, .animation_count = 0
};


// Custom MIN/MAX macros that do not double evaluate the inputs
#define MMAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MMIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })

// Constants for the velocity you can tweak to adjust the player physics
// (the `_s8_8` means convert this floating point number to signed fixed point 8.8 at compile time)
constexpr fs8_8 PLAYER_VELOCITY_PERFRAME = 1.15_s8_8;
constexpr fs8_8 PLAYER_VELOCITY_SPEED_LIMIT = 2.0_s8_8;
constexpr fs8_8 PLAYER_BRAKING_FORCE = 0.35_s8_8;

// constexpr fs8_8 PLAYER_JUMP_MOMENTUM = 3.5_s8_8;
// constexpr fs8_8 PLAYER_GRAVITY_PERFRAME = 0.40_s8_8;
// constexpr fs8_8 PLAYER_GRAVITY_SPEED_LIMIT = 4.0_s8_8;
// constexpr uint8_t PLAYER_MIN_AIR_TIME = 5; // frames
// constexpr uint8_t PLAYER_MAX_AIR_TIME = 12; // frames

extern "C" void update_player_position() {
    // Get the latest input from the controller without polling them again
    auto input = pad_state(0);
    bool move_input_pressed = false;

    // Use the input to adjust the player velocity
    if (input & PAD_LEFT)
    {
        move_input_pressed = true;

        if (player.vel_x > -PLAYER_VELOCITY_SPEED_LIMIT) 
        {
            player.vel_x = player.vel_x - PLAYER_VELOCITY_PERFRAME;        
        }
    }
    if (input & PAD_RIGHT)
    {
        move_input_pressed = true;
        if (player.vel_x < PLAYER_VELOCITY_SPEED_LIMIT) 
        {
            player.vel_x = player.vel_x + PLAYER_VELOCITY_PERFRAME;        
        }
    }
    if (input & PAD_UP)
    {
        move_input_pressed = true;
        if (player.vel_y > -PLAYER_VELOCITY_SPEED_LIMIT) 
        {
            player.vel_y = player.vel_y - PLAYER_VELOCITY_PERFRAME;        
        }
    }
    if (input & PAD_DOWN)
    {
        move_input_pressed = true;
        if (player.vel_y < PLAYER_VELOCITY_SPEED_LIMIT) 
        {
            player.vel_y = player.vel_y + PLAYER_VELOCITY_PERFRAME;        
        }
    }

    if (!move_input_pressed)
    {
        // Holding neither, so apply a braking force to stop the player.
        if (player.vel_x > 0) {
            auto braking_force = player.vel_x - PLAYER_BRAKING_FORCE;
            player.vel_x = MMAX(braking_force, 0);
        } 
        else if (player.vel_x < 0) {
            auto braking_force = player.vel_x + PLAYER_BRAKING_FORCE;
            player.vel_x = MMIN(braking_force, 0);
        }
        
        if (player.vel_y > 0) {
            auto braking_force = player.vel_y - PLAYER_BRAKING_FORCE;
            player.vel_y = MMAX(braking_force, 0);
        } else if (player.vel_y < 0) {
            auto braking_force = player.vel_y + PLAYER_BRAKING_FORCE;
            player.vel_y = MMIN(braking_force, 0);
        }
    }

    // Finally apply the velocity for the player that we calculated
    player.x = player.x + player.vel_x;
    player.y = player.y + player.vel_y;

    // If the player is moving either left or right, increase the frame count
    if (move_input_pressed) 
    {
        player.animation_count++;
        // and then every 15 frames switch which frame we are using.
        // the `& 1` is because we only have 2 frames of animation right now
        player.frame = player.animation_count & 0xf ? player.frame : (player.frame + 1) & 1;
    } 
    else 
    {
        // We stopped moving, so reset the frame and animation count!
        player.frame = 0;
        player.animation_count = 0;
    }

    // Last thing to do is draw the player metasprite.
    // Use the current frame as lookup into the list of metasprite pointers,
    // and pass it and our current position to `oam_meta_spr` to draw it to the screen.
    auto frame_ptr = character_metasprites[player.frame];
    oam_meta_spr(player.x.as_i(), player.y.as_i(), frame_ptr);
}
