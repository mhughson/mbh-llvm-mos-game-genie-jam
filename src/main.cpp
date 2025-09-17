
// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <neslib.h>

// Add-ons to the neslib, bringing metatile support and more
#include <nesdoug.h>

// Include our own player update function for the movable sprite.
#include "player.hpp"

// Include a basic nametable thats RLE compressed for the demo
const unsigned char nametable[] = {
    #embed "../default-nametable-rle.nam"
};

// On the Game Genie, only color 0 and 3 of each palette will be used
static const uint8_t default_palette[] = {
// BG Palette
    0x0f, 0x0f, 0x0f, 0x30,
    0x0f, 0x0f, 0x0f, 0x22,
    0x0f, 0x0f, 0x0f, 0x15,
    0x0f, 0x0f, 0x0f, 0x0f,
// Sprite Palette
    0x0f, 0x0f, 0x0f, 0x30,
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f,
};
// Start the scroll position at 0
static uint8_t scroll_y = 0;
static int8_t direction = 1;

void update_scroll_position() {
    // Update the scroll each frame to bounce the screen
    auto frame = get_frame_count();

    // every 32 frames, switch which direction to move the words
    direction = frame & 0x1f ? direction : -direction;
    scroll_y += direction;
    // Be careful when scrolling in the Y direction!
    // Setting the y scroll between 240-255 will try to render the attribute table.
    // So make sure if you are setting the scroll that you skip over this.
    // The nesdoug libary has a add_scroll_y/sub_scroll_y helper method for skipping those.
    // But i'm not using them here.
    if (scroll_y >= 240) {
        // if we are going up into 240-255, then skip to 0
        // else we are going down into 255, so skip to 239
        scroll_y = direction > 0 ? 0 : 239;
    }
    set_scroll_y(scroll_y);
}

// We defined this data in ca65 as an example of how to reference labels defined in asm in C
extern const uint8_t example_ca65_data[];
// This is a zeropage variable defined in ca65
extern uint8_t __zeropage var_defined_in_ca65;

int main() {
    // Start off by disabling the PPU rendering, allowing us to upload data safely to the nametable (background)
    ppu_off();

    // Clear all sprites off screen. RAM state is random on boot so there is a bunch of garbled sprites on screen
    // by default.
    oam_clear();

    // Set to use 8x8 sprite mode. I doubt 8x16 sprite mode will help much with the game genie CHR :)
    oam_size(0);

    // Upload a basic palette we can use later.
    pal_all(default_palette);

    // Set the scroll position on the screen to 0, 0
    scroll(0, 0);

    // And then draw into the top left nametable.
    vram_adr(0x2000);

    // Write a RLE compressed nametable to the screen. You can generate NESLIB compatible RLE
    // compressed nametables with a tool like NEXXT
    vram_unrle(nametable);
    
    // And then clear out the other nametable as well.
    vram_adr(0x2400);
    vram_fill(0x00, 0x400);
    
    // Turn on the screen, showing both the background and sprites
    ppu_on_all();


    // Now time to start the main game loop
    while (true) {
        // At the start of the frame, poll the controller to get the latest state.
        pad_poll(0);
        // Once a frame, clear the sprites out so that we don't have leftover sprites.
        oam_clear();

        // Update the scroll position for the screen. Typically if you are following a character
        // then this would happen *after* the character moves.
        // NOTE: llvm-mos is very aggressive at inlining, so don't stress the small details too much.
        update_scroll_position();

        update_player_position();
        
        // All done! Wait for the next frame before looping again
        ppu_wait_frame();
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
