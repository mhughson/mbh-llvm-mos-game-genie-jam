

#include <cstdint>
#include <nesdoug.h>
#include <neslib.h>

// We want to keep the bytes for each of the 
#include <soa.h>

#include "metatile.hpp"
#include "text_render.hpp"


/**
    Font definitions
*/

__attribute__((section(".prg_rom_fixed")))
static const constexpr soa::Array<Metatile_2_3, Letter::COUNT> all_letters = {
    #include "font.inc"
};

[[maybe_unused]]
static const Letter* test_string = "01001"_l;

// Include the VRAM buffer and the VRAM_INDEX so we can write directly into the buffer ourselves.
extern volatile uint8_t VRAM_BUF[128];
extern volatile __zeropage uint8_t VRAM_INDEX;
extern volatile __zeropage uint8_t NAME_UPD_ENABLE;

extern "C" void draw_letter(uint8_t x, uint8_t y, Letter letter) {
    auto idx = VRAM_INDEX;
    auto tile = all_letters[letter];
    int ppuaddr_left = NTADR_A(x, y);
    int ppuaddr_right = NTADR_A(x+1, y);
    VRAM_BUF[idx+ 0] = MSB(ppuaddr_left) | NT_UPD_VERT;
    VRAM_BUF[idx+ 1] = LSB(ppuaddr_left);
    VRAM_BUF[idx+ 6] = MSB(ppuaddr_right) | NT_UPD_VERT;
    VRAM_BUF[idx+ 7] = LSB(ppuaddr_right);
    VRAM_BUF[idx+ 2] = 3;
    VRAM_BUF[idx+ 8] = 3;
    VRAM_BUF[idx+ 3] = LEFT_TILE(tile->top_top);
    VRAM_BUF[idx+ 9] = RIGHT_TILE(tile->top_top);
    VRAM_BUF[idx+ 4] = LEFT_TILE(tile->top_bot);
    VRAM_BUF[idx+10] = RIGHT_TILE(tile->top_bot);
    VRAM_BUF[idx+ 5] = LEFT_TILE(tile->bot_top);
    VRAM_BUF[idx+11] = RIGHT_TILE(tile->bot_top);
    VRAM_BUF[idx+12] = 0xff; // terminator bit
    VRAM_INDEX += 12;
}

extern "C" void render_string(uint8_t x, uint8_t y, const Letter str[]) {
    while (*str != Letter::NUL) {
        auto letter = *str;
        str++;
        if (!SKIP_DRAWING_SPACE || letter != Letter::SPACE) {
            draw_letter(x, y, letter);
        }
        if (HALF_SIZE_SPACE && letter == Letter::SPACE)
            x += 1;
        else
            x += 2;
        if (x >= 31) {
            x = 0;
            y += 3;
        }
        if (VRAM_INDEX > (128 - 14)) {
            flush_vram_update2();
        }
    }
    NAME_UPD_ENABLE = true;
}
