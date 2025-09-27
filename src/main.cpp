#include "main.hpp"

// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <cstdio>
#include <fixed_point.h>
#include <initializer_list>
#include <neslib.h>
#include <famitone2.h>

// Add-ons to the neslib, bringing metatile support and more
#include <nesdoug.h>
#include <stdlib.h>
#include <zaplib.h>

// Include our own player update function for the movable sprite.
#include "metatile.hpp"
#include "text_render.hpp"
#include "metasprites.h"

// define this somewhere in main.c this will write a string one byte a time to $401b
extern "C" void __putchar(char c) { POKE(0x401b, c); }

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

const unsigned char screen_gameover[] = {
    #embed "../screen_gameover.nrle"
};

// On the Game Genie, only color 0 and 3 of each palette will be used

const unsigned char palette_metaspr_a[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x0c,0x21,0x32,0x0f,0x05,0x16,0x27,0x0f,0x0b,0x1a,0x29 };

struct SpawnArea
{
    uint8_t start_x;
    uint8_t start_y;
};

SpawnArea spawn_points[] = 
{
    {
        16, 16,
    },
    {
        128, 16,
    },
    {
        16, 120,
    },
    {
        128, 120,
    }
};

SpawnArea spawn_area_collections[2][2][3] =
{
    {
        {
            // Top left
            spawn_points[1],
            spawn_points[2],
            spawn_points[3],
        },
        {
            // Bottom Left
            spawn_points[0],
            spawn_points[1],
            spawn_points[3],
        },
    },
    {
        {
            // Top Right
            spawn_points[0],
            spawn_points[2],
            spawn_points[3],
        },
        {
            // Bottom Right
            spawn_points[0],
            spawn_points[1],
            spawn_points[2],
        }
    }
};

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

static uint16_t score = 0;
static uint16_t hiscore = 0;

static uint8_t ammo_count = 3;

#define ENEMY_SPAWN_TIME 120
static uint8_t enemy_spawn_timer = 0;

#define AMMO_SPAWN_TIME (60 * 10)
static uint16_t ammo_spawn_timer = 0;

static uint16_t ticks_in_state = 0;

void update_player();

void try_spawn_ammo_pickup()
{
    // First, count how many active ammo pickups there are and store a valid index
    // for an unused slot.
    unsigned char active_count = 0;
    unsigned char unused_index = NUM_ENTITIES; // Invalid index.

    for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
    {
        if (ActiveEntities[i].cur_state == Entity_States::ACTIVE)
        {
            if (ActiveEntities[i].type == ENTITY_TYPE_AMMO)
            {
                ++active_count;
            }
        }
        else
        {
            unused_index = i;
        }
    }

    // If we have an unused slot, and we have less than 2 active ammo pickups,
    if (unused_index < NUM_ENTITIES && active_count == 0 && ammo_count < MAX_AMMO)
    {
        // Spawn a new ammo pickup.
        // Find a spawn area that is not the one the player is currently in.
        // The screen is divided into 4 quadrants, and we pick one of the other
        // 3 quadrants randomly.

        // Mark the entity as active and of type ammo.
    
        ActiveEntities[unused_index].cur_state = Entity_States::ACTIVE;
        ActiveEntities[unused_index].type = ENTITY_TYPE_AMMO;

        // which of the 4 regions is the player in?
        uint8_t x_region = (p1.x.as_i() / 128);
        uint8_t y_region = (p1.y.as_i() / 120);

        // pick a region from the area that exludes the one the player is
        // in.
        uint8_t area_choice = (uint8_t)(rand()) % 3;

        SpawnArea spawn_area = spawn_area_collections[x_region][y_region][area_choice];

        ActiveEntities[unused_index].x = spawn_area.start_x + ((uint8_t)rand() % (128 - 16));
        ActiveEntities[unused_index].y = spawn_area.start_y + ((uint8_t)rand() % (120 - 16));                

        ActiveEntities[unused_index].vel_x = 0;
        ActiveEntities[unused_index].vel_y = 0;
        ActiveEntities[unused_index].anim_counter = 0;
        ActiveEntities[unused_index].anim_frame = 0;
    }
}

void try_spawn_enemy()
{
    // First, count how many active enemies there are and store a valid index
    // for an unused slot.
    unsigned char active_count = 0;
    unsigned char unused_index = NUM_ENTITIES; // Invalid index.

    for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
    {
        if (ActiveEntities[i].cur_state == Entity_States::ACTIVE)
        {
            if (ActiveEntities[i].type == ENTITY_TYPE_ENEMY)
            {
                ++active_count;
            }
        }
        else
        {
            unused_index = i;
        }
    }

    // If we have an unused slot, and we have less than 4 active enemies,
    if (unused_index < NUM_ENTITIES && active_count < 4)
    {
        ActiveEntities[unused_index].cur_state = Entity_States::ACTIVE;
        ActiveEntities[unused_index].type = ENTITY_TYPE_ENEMY;

        // which of the 4 regions is the player in?
        uint8_t x_region = (p1.x.as_i() / 128);
        uint8_t y_region = (p1.y.as_i() / 120);

        // pick a region from the area that exludes the one the player is
        // in.
        uint8_t area_choice = (uint8_t)(rand()) % 3;

        SpawnArea spawn_area = spawn_area_collections[x_region][y_region][area_choice];

        ActiveEntities[unused_index].x = spawn_area.start_x + ((uint8_t)rand() % (128 - 16));
        ActiveEntities[unused_index].y = spawn_area.start_y + ((uint8_t)rand() % (120 - 16));                

        ActiveEntities[unused_index].vel_x = 0;
        ActiveEntities[unused_index].vel_y = 0;
        ActiveEntities[unused_index].anim_counter = 0;
        ActiveEntities[unused_index].anim_frame = 0;
    }
}

void goto_state(Game_States new_state)
{
    ticks_in_state = 0;

    cur_state = new_state;

    switch (cur_state) 
    {
        case Game_States::STATE_TITLE:
        {
            ppu_off();
            oam_clear();
            // Upload a basic palette we can use later.
            pal_bg(palette_metaspr_a);
            pal_spr(palette_metaspr_a);

            // Set the scroll position on the screen to 0, 0
            scroll(0, 0);

            vram_adr(NAMETABLE_A);
            vram_unrle(screen_title);   
            ppu_on_all();         

            // Metatile_2_2 test_tile;
            // test_tile.top = 0x1f;
            // test_tile.bot = 0x1f;

            // draw_metatile_2_2(Nametable::A, 13, 20, &test_tile); // Flashing "Press Start"
            break;
        }

        case Game_States::STATE_TUTORIAL:
        {
            ppu_off();
            vram_adr(NAMETABLE_A);
            vram_unrle(screen_gameplay);        

                                                                //0000000000000000
            render_string(Nametable::A, 2, 4,  " MOVE with DPAD"_l);
            render_string(Nametable::A, 2, 12, " SHOOT with the"_l);
            render_string(Nametable::A, 2, 16, "        ZAPPER"_l);

            render_string(Nametable::A, 18, 26, "AMMO"_l);

            // allow vram to flush
            ppu_wait_nmi();

            p1.cur_state = Entity_States::ACTIVE;
            p1.x = 128 - 8;
            p1.y = 180 - 16;
            p1.vel_x = 0;
            p1.vel_y = 0;
            p1.anim_counter = 0;
            p1.anim_frame = 0;          
            
            for (uint8_t i = 0; i < ammo_count; ++i)
            {
                vram_adr(NTADR_A(29 - i, 27));
                vram_put(0x05); // bullet icon
            }            
            
            ppu_on_all();         

            break;            
        }

        case Game_States::STATE_GAMEPLAY:
        {
            // Reseed RNG every time we enter gameplay, using frame ticks as timing entropy.
            srand((unsigned)ticks16);
            ppu_off();
            vram_adr(NAMETABLE_A);
            vram_unrle(screen_gameplay);

            score = 0;
            render_string(Nametable::A, 2, 2, "000"_l);

            if (hiscore != 0)
            {
                Letter score_digits[4] = { 
                    (Letter)4,
                    (Letter)((hiscore / 100) % 10), 
                    (Letter)((hiscore / 10) % 10), 
                    (Letter)(hiscore % 10) 
                };

                render_string(Nametable::A, 24, 2, score_digits);                
            }
            else
            {
                render_string(Nametable::A, 24, 2, "000"_l);
            }

            // Clear out all entities
            for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
            {
                ActiveEntities[i].cur_state = Entity_States::UNUSED;
            }

            // Reset player position and state
            p1.cur_state = Entity_States::ACTIVE;
            p1.x = 128 - 8;
            p1.y = 120 - 16;
            p1.vel_x = 0;
            p1.vel_y = 0;
            p1.anim_counter = 0;
            p1.anim_frame = 0;

            ammo_count = 3;

            enemy_spawn_timer = ENEMY_SPAWN_TIME;
            ammo_spawn_timer = AMMO_SPAWN_TIME;

            for (uint8_t i = 0; i < ammo_count; ++i)
            {
                vram_adr(NTADR_A(29 - i, 27));
                vram_put(0x05); // bullet icon
            }

            ppu_on_all();
            break;
        }

        case Game_States::STATE_GAMEOVER:
        {
            ppu_off();
            vram_adr(NAMETABLE_A);
            vram_unrle(screen_gameover);

            if (score > hiscore)
            {
                // NEW HIGH SCORE
                hiscore = score;
            }

            ppu_on_all();
            break;
        }
    }
}

void update_state_title()
{
    if (pad_pressed & (PAD_A | PAD_START) || (zapper_pressed && zapper_ready)) 
    {
        goto_state(STATE_TUTORIAL);
        return;
    }
}

void update_state_tutorial()
{
    
    update_player();

    if (pad_pressed & (PAD_A | PAD_START) || (zapper_pressed && zapper_ready)) 
    {
        goto_state(STATE_GAMEPLAY);
        return;
    }
}

constexpr fs8_8 p1_VELOCITY_PERFRAME = 0.2_s8_8;
constexpr fs8_8 p1_VELOCITY_SPEED_LIMIT = 1.2_s8_8;
constexpr fs8_8 p1_BRAKING_FORCE = 0.1_s8_8;

constexpr uint8_t WALL_OFFSET = 8;
constexpr uint8_t WALL_OFFSET_RIGHT = 256 - WALL_OFFSET - 16 - 3;
constexpr uint8_t WALL_OFFSET_BOTTOM = 240 - 32 - 8 - 3;

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

//    if (!move_input_pressed)
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

    if (input & PAD_LEFT)
    {
        // did we hit a wall?
        if (p1.x.as_i() <= WALL_OFFSET)
        {
            p1.x = WALL_OFFSET;
            p1.vel_x = 0;
        }
    }
    if (input & PAD_RIGHT)
    {
        // did we hit a wall?
        if (p1.x.as_i() >= WALL_OFFSET_RIGHT)
        {
            p1.x = WALL_OFFSET_RIGHT;
            p1.vel_x = 0;
        }
    }
    if (input & PAD_UP)
    {
        if (cur_state == STATE_TUTORIAL)
        {
            // did we hit a wall?
            if (p1.y.as_i() <= (140 + WALL_OFFSET))
            {
                p1.y = (140 + WALL_OFFSET);
                p1.vel_y = 0;
            }
        }
        else
        {
            // did we hit a wall?
            if (p1.y.as_i() <= WALL_OFFSET)
            {
                p1.y = WALL_OFFSET;
                p1.vel_y = 0;
            }
        }
    }
    if (input & PAD_DOWN)
    {
        // did we hit a wall?
        if (p1.y.as_i() >= WALL_OFFSET_BOTTOM)
        {
            p1.y = WALL_OFFSET_BOTTOM;
            p1.vel_y = 0;
        }
    }

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

void update_enemy(Entity& Object)
{
    constexpr fs8_8 MAX_SPEED = 5.0_s8_8;
    constexpr fs8_8 ACCELERATION = 0.01_s8_8;

    if (p1.x.as_i() < Object.x.as_i())
    {
        if (Object.vel_x > -MAX_SPEED) 
        {
            Object.vel_x -= ACCELERATION;
        }   
    }
    else if (p1.x.as_i() > Object.x.as_i())
    {
        if (Object.vel_x < MAX_SPEED) 
        {
            Object.vel_x += ACCELERATION;
        }
    }

    if ((p1.y.as_i() + 16) < Object.y.as_i())
    {
        if (Object.vel_y > -MAX_SPEED) 
        {
            Object.vel_y -= ACCELERATION;
        }
    }
    else if ((p1.y.as_i() + 16) > Object.y.as_i())
    {
        if (Object.vel_y < MAX_SPEED) 
        {
            Object.vel_y += ACCELERATION;
        }
    }            

    constexpr uint8_t SCREEN_BORDER = 8;
    // If approaching border of the screen, reverse direction.
    // Also cut the velocity in half.
    if (Object.vel_x < 0 && Object.x.as_i() < SCREEN_BORDER)
    {
        Object.vel_x = MABS(Object.vel_x) / 2;
    }
    else if (Object.vel_x > 0 && Object.x.as_i() + 16 > (256 - SCREEN_BORDER))
    {
        Object.vel_x = -MABS(Object.vel_x) / 2;
    }
    // Also for the y axis which is 240 pixels high.
    if (Object.vel_y < 0 && Object.y.as_i() < SCREEN_BORDER)
    {
        Object.vel_y = MABS(Object.vel_y) / 2;
    }
    else if (Object.vel_y > 0 && Object.y.as_i() + 16 > (240 - SCREEN_BORDER))
    {
        Object.vel_y = -MABS(Object.vel_y) / 2;
    }

    Object.x += Object.vel_x;
    Object.y += Object.vel_y;

    // Is the center point of the entity colliding with the player?
    // Note: The player and entity have top left origins.
    //       The player is 32x16, and we want to collide with the
    //       bottom 16x16 area, so we offset the player's y position by 16.
    if (   (p1.x.as_i() + 8 >= Object.x.as_i())
        && (p1.x.as_i() <= Object.x.as_i() + 8)
        && (p1.y.as_i() + 16 + 8 >= Object.y.as_i())
        && (p1.y.as_i() + 16 <= Object.y.as_i() + 8))
    {
        // Collision detected, go to game over state.
        goto_state(STATE_GAMEOVER);
        return;
    }

    ++Object.anim_counter;

    if (Object.anim_counter > 5)
    {
        Object.anim_counter = 0;
        ++Object.anim_frame;

        if (Object.anim_frame > 1)
        {
            Object.anim_frame = 0;
        }
    }

    oam_meta_spr(
        Object.x.as_i(), 
        Object.y.as_i(), 
        metaspr_list[1 + Object.anim_frame]);
}

void update_ammo_pickup(Entity& Object)
{
    // Not really needed since these don't move, but just keep for now.
    Object.x += Object.vel_x;
    Object.y += Object.vel_y;

    #define PLAYER_WIDTH 16
    #define PLAYER_HEIGHT 32
    #define AMMO_WIDTH 8
    #define AMMO_HEIGHT 8

    // is the right edge of the player left of the left edge of the object?
    bool isLeft     = (p1.x.as_i() + PLAYER_WIDTH) < Object.x.as_i();
    bool isRight    = (p1.x.as_i()) > (Object.x.as_i() + AMMO_WIDTH);
    bool isAbove    = (p1.y.as_i() + PLAYER_HEIGHT) < Object.y.as_i();
    bool isBelow    = (p1.y.as_i()) > (Object.y.as_i() + AMMO_HEIGHT);

    bool isOverlap = !(isLeft || isRight || isAbove || isBelow);

    if (isOverlap)
    {
        // Give ammo and update this location on the ammo count display.
        if (ammo_count < MAX_AMMO) // this should always pass
        {
            // Update the screen first so that we draw an ammo on the currently
            // empty slot.
            one_vram_buffer(0x05, get_ppu_addr(0, (29 - ammo_count) * 8, (27 * 8)));
            ++ammo_count;

            Object.cur_state = Entity_States::UNUSED;
        }
    }

    ++Object.anim_counter;

    if (Object.anim_counter > 5)
    {
        Object.anim_counter = 0;
        ++Object.anim_frame;

        if (Object.anim_frame > 1)
        {
            Object.anim_frame = 0;
        }
    }

    oam_spr(Object.x.as_i(), Object.y.as_i(), 0x05, 0); // Simple single-sprite bullet icon
}

void update_state_gameplay()
{
    update_player();

    --enemy_spawn_timer;

    if (enemy_spawn_timer == 0)
    {
        enemy_spawn_timer = ENEMY_SPAWN_TIME;

        try_spawn_enemy();
    }

    --ammo_spawn_timer;

    if (ammo_spawn_timer == 0)
    {
        ammo_spawn_timer = AMMO_SPAWN_TIME;

        try_spawn_ammo_pickup();
    }


    // DEBUG: Spawn enemies to shoot.
    if (pad_pressed & PAD_B)
    {
        for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
        {
            if (ActiveEntities[i].cur_state == Entity_States::UNUSED)
            {
                ActiveEntities[i].cur_state = Entity_States::ACTIVE;
                ActiveEntities[i].type = ENTITY_TYPE_ENEMY;

                // uint8_t start_x = (p1.x.as_i() > 128) ? 16 : 128;
                // uint8_t start_y = (p1.y.as_i() > 120) ? 16 : 120;

                // ActiveEntities[i].x = start_x + ((uint8_t)rand() % (128 - 16));
                // ActiveEntities[i].y = start_y + ((uint8_t)rand() % (120 - 16));

                // which of the 4 regions is the player in?
                uint8_t x_region = (p1.x.as_i() / 128);
                uint8_t y_region = (p1.y.as_i() / 120);

                // use it like this
                //puts("x: ");
                // puts("x,y regions:");
                // putchar('0' + x_region);
                // putchar(',');
                // //puts(" y: ");
                // putchar('0' + y_region);           
                // puts("");

                // pick a region from the area that exludes the one the player is
                // in.
                uint8_t area_choice = (uint8_t)(rand()) % 3;

                // puts("area choice:");
                // putchar('0' + area_choice);          
                // puts("");                

                SpawnArea spawn_area = spawn_area_collections[x_region][y_region][area_choice];

                ActiveEntities[i].x = spawn_area.start_x + ((uint8_t)rand() % (128 - 16));
                ActiveEntities[i].y = spawn_area.start_y + ((uint8_t)rand() % (120 - 16));                

                ActiveEntities[i].vel_x = 0;
                ActiveEntities[i].vel_y = 0;
                ActiveEntities[i].anim_counter = 0;

                break;
            }
        }
    }

    // DEBUG: Spawn ammo
    if (pad_pressed & PAD_A)
    {
        try_spawn_ammo_pickup();
    }    

    if (pad_pressed & PAD_SELECT)
    {
        goto_state(STATE_GAMEOVER);
        return;
    }   

    // Update all the entities and draw them to the screen.
    for (unsigned char i = 0; i < NUM_ENTITIES; ++i)
    {
        if (ActiveEntities[i].cur_state != Entity_States::UNUSED)
        {
            switch (ActiveEntities[i].type) 
            {
                case ENTITY_TYPE_ENEMY:
                {
                    update_enemy(ActiveEntities[i]);
                    break;
                }

                case ENTITY_TYPE_AMMO:
                {
                    update_ammo_pickup(ActiveEntities[i]);
                    break;
                }

                default:
                    break;
            }
        }
    }

    // Was the Zapper pressed this frame, but NOT pressed last frame.
    if (zapper_pressed && zapper_ready && ammo_count > 0)
    {   
        // Decrease ammo count and update the display
        --ammo_count;
        // Clear the one ammo that was fired.
        one_vram_buffer(0x00, get_ppu_addr(0, (29 - ammo_count) * 8, (27 * 8)));

        // TODO: Needed?
        ppu_wait_nmi();

        bool hit_detected = false;

        for (uint8_t i = 0; i < NUM_ENTITIES; ++i)
        {
            if (ActiveEntities[i].cur_state != Entity_States::UNUSED 
                && ActiveEntities[i].type == ENTITY_TYPE_ENEMY)
            {
                oam_clear();
                oam_meta_spr(ActiveEntities[i].x.as_i(), ActiveEntities[i].y.as_i(), metaspr_box_16_16_data);

                // NOTE: Must be here before zap_read, or else the zapper
                //       will see the previous frames data.
                ppu_wait_nmi();

                hit_detected = zap_read(1);

                if (hit_detected)
                {
                    // increase score and draw it
                    ++score;
                    if (score > 999) score = 999;

                    // Create a temp letter array to hold the score digits
                    Letter score_digits[4] = { 
                        (Letter)4,
                        (Letter)((score / 100) % 10), 
                        (Letter)((score / 10) % 10), 
                        (Letter)(score % 10) 
                    };

                    render_string(Nametable::A, 2, 2, score_digits);

                    ActiveEntities[i].cur_state = Entity_States::UNUSED;
                    break;
                }
            }
        }
    }        
}

void update_state_gameover()
{
    if (pad_pressed & (PAD_A | PAD_START) || (zapper_pressed && zapper_ready)) 
    {
        if (ticks_in_state < 60) return;
        
        goto_state(STATE_TITLE);
        return;
    }
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
        ++ticks_in_state;
        
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

            case STATE_TUTORIAL:
            {
                update_state_tutorial();
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
