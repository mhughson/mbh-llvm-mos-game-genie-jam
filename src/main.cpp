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
#include "player.hpp"
#include "text_render.hpp"
#include "metasprites.h"

// Include a basic nametable thats RLE compressed for the demo
const unsigned char nametable[] = {
    #embed "../default-nametable-rle.nam"
};

const unsigned char screen_title[] = {
    #embed "../screen_title.nrle"
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
    
    // Turn on the screen, showing both the background and sprites
    ppu_on_all();

    // Now time to start the main game loop
    while (true) {
        
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

        update_player_position();

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

                    //ActiveEntities[i].vel_x = 0.35_s8_8;

                    break;
                }
            }
        }

        // Update all the entities and draw them to the screen.
        for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
        {
            if (ActiveEntities[i].cur_state != Entity_States::UNUSED)
            {
                if (player.x.as_i() < ActiveEntities[i].x.as_i())
                {
                    ActiveEntities[i].vel_x -= 0.01_s8_8;
                }
                else if (player.x.as_i() > ActiveEntities[i].x.as_i())
                {
                    ActiveEntities[i].vel_x += 0.01_s8_8;
                }

                if (player.y.as_i() < ActiveEntities[i].y.as_i())
                {
                    ActiveEntities[i].vel_y -= 0.01_s8_8;
                }
                else if (player.y.as_i() > ActiveEntities[i].y.as_i())
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
        
        // All done! Wait for the next frame before looping again
        ppu_wait_nmi();
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
