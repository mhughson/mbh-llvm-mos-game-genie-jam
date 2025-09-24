#include "main.h"

// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <fixed_point.h>
#include <initializer_list>
#include <neslib.h>

// Add-ons to the neslib, bringing metatile support and more
#include <nesdoug.h>
#include <stdlib.h>
#include <zaplib.h>

// Include our own player update function for the movable sprite.
#include "text_render.hpp"
#include "metasprites.h"

// Include a basic nametable thats RLE compressed for the demo
const unsigned char nametable[] = {
    #embed "../default-nametable-rle.nam"
};

const unsigned char screen_title[] = {
    #embed "../screen_title.nrle"
};

const unsigned char screen_gameplay[] = {
    #embed "../screen_gameplay.nrle"
};

// On the Game Genie, only color 0 and 3 of each palette will be used

const unsigned char palette_metaspr_a[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x0c,0x21,0x32,0x0f,0x05,0x16,0x27,0x0f,0x0b,0x1a,0x29 };

// Tracking Zapper State
static uint8_t zapper_pressed = 0;
static uint8_t zapper_ready = 0; //wait till it's 0

// Tracking gamepad state (held and pressed)
static uint8_t pad = 0;
static uint8_t pad_pressed = 0;

// We defined this data in ca65 as an example of how to reference labels defined in asm in C
extern const uint8_t example_ca65_data[];
// This is a zeropage variable defined in ca65
extern uint8_t __zeropage var_defined_in_ca65;

// An array of all the active entities. If a new entities wants to spawn
// it needs to find an empty slot in here first.
Entity ActiveEntities[NUM_ENTITIES];

// Player object.
Entity p1;

Game_States cur_state = Game_States::STATE_TITLE;

// Frame tick counter since power-on (incremented once per main loop iteration)
static uint16_t ticks16 = 0; // 32-bit to avoid quick wrap; NES time constraints minimal

void goto_state(Game_States new_state)
{
    cur_state = new_state;

    switch (cur_state) 
    {
        case Game_States::STATE_TITLE:
        {
            // Upload a basic palette we can use later.
            pal_bg(palette_metaspr_a);
            pal_spr(palette_metaspr_a);

            // Set the scroll position on the screen to 0, 0
            scroll(0, 0);

            vram_adr(NAMETABLE_A);
            vram_unrle(screen_title);            
            break;
        }

        case Game_States::STATE_GAMEPLAY:
        {
            // Reseed RNG every time we enter gameplay, using frame ticks as timing entropy.
            srand((unsigned)ticks16);
            ppu_off();
            vram_adr(NAMETABLE_A);
            vram_unrle(screen_gameplay);
            ppu_on_all();
            break;
        }

        case Game_States::STATE_GAMEOVER:
        {
            break;
        }
    }
}

void update_state_title()
{
    if (pad_pressed & (PAD_A | PAD_START)) 
    {
        goto_state(STATE_GAMEPLAY);
        return;
    }
}

constexpr fs8_8 p1_VELOCITY_PERFRAME = 1.15_s8_8;
constexpr fs8_8 p1_VELOCITY_SPEED_LIMIT = 2.0_s8_8;
constexpr fs8_8 p1_BRAKING_FORCE = 0.35_s8_8;

void update_player()
{
    // Get the latest input from the controller without polling them again
    auto input = pad_state(0);
    bool move_input_pressed = false;

    // Use the input to adjust the p1 velocity
    if (input & PAD_LEFT)
    {
        move_input_pressed = true;

        p1.facing_left = true;

        if (p1.vel_x > -p1_VELOCITY_SPEED_LIMIT) 
        {
            p1.vel_x = p1.vel_x - p1_VELOCITY_PERFRAME;        
        }
    }
    if (input & PAD_RIGHT)
    {
        p1.facing_left = false;
        move_input_pressed = true;
        if (p1.vel_x < p1_VELOCITY_SPEED_LIMIT) 
        {
            p1.vel_x = p1.vel_x + p1_VELOCITY_PERFRAME;        
        }
    }
    if (input & PAD_UP)
    {
        move_input_pressed = true;
        if (p1.vel_y > -p1_VELOCITY_SPEED_LIMIT) 
        {
            p1.vel_y = p1.vel_y - p1_VELOCITY_PERFRAME;        
        }
    }
    if (input & PAD_DOWN)
    {
        move_input_pressed = true;
        if (p1.vel_y < p1_VELOCITY_SPEED_LIMIT) 
        {
            p1.vel_y = p1.vel_y + p1_VELOCITY_PERFRAME;        
        }
    }

    if (!move_input_pressed)
    {
        
        // Holding neither, so apply a braking force to stop the p1.
        if (p1.vel_x > 0) {
            auto braking_force = p1.vel_x - p1_BRAKING_FORCE;
            p1.vel_x = MMAX(braking_force, 0);
        } 
        else if (p1.vel_x < 0) {
            auto braking_force = p1.vel_x + p1_BRAKING_FORCE;
            p1.vel_x = MMIN(braking_force, 0);
        }
        
        if (p1.vel_y > 0) {
            auto braking_force = p1.vel_y - p1_BRAKING_FORCE;
            p1.vel_y = MMAX(braking_force, 0);
        } else if (p1.vel_y < 0) {
            auto braking_force = p1.vel_y + p1_BRAKING_FORCE;
            p1.vel_y = MMIN(braking_force, 0);
        }
    }

    // Finally apply the velocity for the p1 that we calculated
    p1.x = p1.x + p1.vel_x;
    p1.y = p1.y + p1.vel_y;

    ++p1.anim_counter;

    if (p1.anim_counter > 5)
    {
        p1.anim_counter = 0;
        ++p1.anim_frame;

        if (p1.anim_frame > 1)
        {
            p1.anim_frame = 0;
        }
    }

    uint8_t facing_offset = 0;
    if (p1.facing_left)
    {
        facing_offset = 3;
    }

    if (move_input_pressed)
    {
        oam_meta_spr(p1.x.as_i(), p1.y.as_i(), metaspr_list[3 + facing_offset + p1.anim_frame]);
    }
    else
    {
        oam_meta_spr(p1.x.as_i(), p1.y.as_i(), metaspr_list[5 + facing_offset]);
    }

}

void update_state_gameplay()
{
    update_player();

    // DEBUG: Spawn enemies to shoot.
    if (pad_pressed & PAD_B)
    {
        for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
        {
            if (ActiveEntities[i].cur_state == Entity_States::UNUSED)
            {
                ActiveEntities[i].cur_state = Entity_States::ACTIVE;
                ActiveEntities[i].x = 128;
                ActiveEntities[i].y = (uint8_t)rand() % (240 - 16);

                ActiveEntities[i].vel_x = 0;
                ActiveEntities[i].vel_y = 0;
                ActiveEntities[i].anim_counter = 0;

                break;
            }
        }
    }

    // Update all the entities and draw them to the screen.
    for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
    {
        if (ActiveEntities[i].cur_state != Entity_States::UNUSED)
        {
            if (p1.x.as_i() < ActiveEntities[i].x.as_i())
            {
                ActiveEntities[i].vel_x -= 0.01_s8_8;
            }
            else if (p1.x.as_i() > ActiveEntities[i].x.as_i())
            {
                ActiveEntities[i].vel_x += 0.01_s8_8;
            }

            if (p1.y.as_i() < ActiveEntities[i].y.as_i())
            {
                ActiveEntities[i].vel_y -= 0.01_s8_8;
            }
            else if (p1.y.as_i() > ActiveEntities[i].y.as_i())
            {
                ActiveEntities[i].vel_y += 0.01_s8_8;
            }                

            ActiveEntities[i].x += ActiveEntities[i].vel_x;
            ActiveEntities[i].y += ActiveEntities[i].vel_y;

            ++ActiveEntities[i].anim_counter;

            if (ActiveEntities[i].anim_counter > 5)
            {
                ActiveEntities[i].anim_counter = 0;
                ++ActiveEntities[i].anim_frame;

                if (ActiveEntities[i].anim_frame > 1)
                {
                    ActiveEntities[i].anim_frame = 0;
                }
            }

            oam_meta_spr(ActiveEntities[i].x.as_i(), ActiveEntities[i].y.as_i(), metaspr_list[1 + ActiveEntities[i].anim_frame]);
        }
    }

    // Was the Zapper pressed this frame, but NOT pressed last frame.
    if (zapper_pressed && zapper_ready)
    {   
        // TODO: Needed?
        ppu_wait_nmi();

        bool hit_detected = false;

        for (uint8_t i = 0; i < NUM_ENTITIES; ++i)
        {
            if (ActiveEntities[i].cur_state != Entity_States::UNUSED)
            {
                oam_clear();
                oam_meta_spr(ActiveEntities[i].x.as_i(), ActiveEntities[i].y.as_i(), metaspr_box_16_16_data);

                // NOTE: Must be here before zap_read, or else the zapper
                //       will see the previous frames data.
                ppu_wait_nmi();

                hit_detected = zap_read(1);

                if (hit_detected)
                {
                    ActiveEntities[i].cur_state = Entity_States::UNUSED;
                    break;
                }
            }
        }
    }        
}

void update_state_gameover()
{

}

// ENTRY POINT FOR THE PROGRAM
int main() 
{
  
    // Tell NMI to update graphics using the VRAM_BUFFER provided by nesdoug library
    set_vram_buffer();
    
    // Start off by disabling the PPU rendering, allowing us to upload data safely to the nametable (background)
    ppu_off();

    // Clear all sprites off screen. RAM state is random on boot so there is a bunch of garbled sprites on screen
    // by default.
    oam_clear();

    // Set to use 8x8 sprite mode. I doubt 8x16 sprite mode will help much with the game genie CHR :)
    oam_size(0);

    // Upload a basic palette we can use later.
    pal_bg(palette_metaspr_a);
    pal_spr(palette_metaspr_a);

    // Set the scroll position on the screen to 0, 0
    scroll(0, 0);

    vram_adr(NAMETABLE_A);
    vram_unrle(screen_title);
    
    // And then clear out the other nametable as well.
    vram_adr(NAMETABLE_B);

    // Write a RLE compressed nametable to the screen. You can generate NESLIB compatible RLE
    // compressed nametables with a tool like NEXXT
    vram_unrle(nametable);


    goto_state(Game_States::STATE_TITLE);

    
    // Turn on the screen, showing both the background and sprites
    ppu_on_all();

    // Now time to start the main game loop
    while (true) 
    {
        // Count frames elapsed since boot (before reading input so timing differences matter for seeding)
        ++ticks16;
        
        // Get the input state.
        // NOTE: This will return the "trigger/pressed" state, but pad_state
        //       will still return the "held" state.
        pad_pressed = pad_trigger(0);
        pad = pad_state(0);

        // Once a frame, clear the sprites out so that we don't have leftover sprites.
        oam_clear();

        // XOR with the last frame to make sure this is a NEW press. In other words,
        // if pad2_zapper was 1 last frame (pressed), zapper_ready will be 0 (not ready).
		zapper_ready = zapper_pressed^1;

		// is trigger pulled?
		zapper_pressed = zap_shoot(1);

        switch (cur_state) 
        {
            case STATE_TITLE:
            {
                update_state_title();
                break;
            }

            case STATE_GAMEPLAY:
            {
                update_state_gameplay();
                break;
            }
        
            case STATE_GAMEOVER:
            {
                update_state_gameover();
                break;
            }
        }
        
        // All done! Wait for the next frame before looping again
        ppu_wait_nmi();
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
