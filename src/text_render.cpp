

#include <cstdint>
#include <nesdoug.h>
#include <neslib.h>

// We want to keep the bytes for each of the 
#include <soa.h>

#include "text_render.hpp"


/**
    Font definitions
*/

/*
    A letter is a 4x8 pixel square packed into a 32bit int, which we then convert into a 
    For performance and space sakes, we split this into 4 individual bytes using the SOA struct to make it compile better.
*/
struct LetterMt {
    // uint8_t top_topleft;
    // uint8_t top_topright;
    uint8_t top_botleft;
    uint8_t top_botright;
    uint8_t bot_topleft;
    uint8_t bot_topright;
    uint8_t bot_botleft;
    uint8_t bot_botright;
};

consteval uint8_t get_tile_for_bits(uint8_t bits) {
    const uint8_t bits_to_tile[] = {
        // order for the bits is tl tr bl br
        0x0, // 0b0000
        0x2, // 0b0001
        0x1, // 0b0010
        0x3, // 0b0011
        0x8, // 0b0100
        0xa, // 0b0101
        0x9, // 0b0110
        0xb, // 0b0111
        0x4, // 0b1000
        0x6, // 0b1001
        0x5, // 0b1010
        0x7, // 0b1011
        0xc, // 0b1100
        0xe, // 0b1101
        0xd, // 0b1110
        0xf, // 0b1111
    };
    return bits_to_tile[bits];
}

consteval LetterMt operator""_mt(const char* text, size_t len) {
    // loop through the letters of the provided string looking for any special characters.
    // Replace a space character with a since bit 0 and any other non-ignored character with a bit 1
    uint32_t bits = 0;
    int i = 0;
    // Require all tiles passed into mt to have at least 4px per row and 8 px per column
    char c = text[i];
    while (i < len) {
        // then skip anything else until the next pipe
        while (c != '|' && i < len) c = text[i++];
        // skip over the left |
        c = text[i++];
        // then read 4 letters
        for (int j=0; j < 4; j++) {
            uint8_t value = (c == ' ') ? 0 : 1;
            bits <<= 1;
            bits |= value;
            c = text[i++];
        }
        // skip over the right |
        c = text[i++];
    }

    LetterMt letter {
        // .top_topleft   = get_tile_for_bits((((bits >> 30) & 0x3) << 2) | ((bits >> 26) & 0x3)),
        // .top_topright  = get_tile_for_bits((((bits >> 28) & 0x3) << 2) | ((bits >> 24) & 0x3)),
        .top_botleft   = get_tile_for_bits((((bits >> 22) & 0x3) << 2) | ((bits >> 18) & 0x3)),
        .top_botright  = get_tile_for_bits((((bits >> 20) & 0x3) << 2) | ((bits >> 16) & 0x3)),
        .bot_topleft   = get_tile_for_bits((((bits >> 14) & 0x3) << 2) | ((bits >> 10) & 0x3)),
        .bot_topright  = get_tile_for_bits((((bits >> 12) & 0x3) << 2) | ((bits >>  8) & 0x3)),
        .bot_botleft   = get_tile_for_bits((((bits >>  6) & 0x3) << 2) | ((bits >>  2) & 0x3)),
        .bot_botright  = get_tile_for_bits((((bits >>  4) & 0x3) << 2) | ((bits >>  0) & 0x3)),
    };
    return letter;
}

#define SOA_STRUCT LetterMt
#define SOA_MEMBERS MEMBER(top_botleft) MEMBER(top_botright) \
                    MEMBER(bot_topleft) MEMBER(bot_topright) MEMBER(bot_botleft) MEMBER(bot_botright)
#include <soa-struct.inc>

__attribute__((section(".prg_rom_fixed")))
static const constexpr soa::Array<LetterMt, Letter::COUNT> all_letters = {
    #include "font.inc"
};

[[maybe_unused]]
static const Letter* test_string = "01001"_l;

// Include the VRAM buffer and the VRAM_INDEX so we can write directly into the buffer ourselves.
extern volatile uint8_t VRAM_BUF[128];
extern volatile __zeropage uint8_t VRAM_INDEX;
extern volatile __zeropage uint8_t NAME_UPD_ENABLE;

extern "C" void draw_letter(uint8_t x, uint8_t y, Letter letter) {
    int ppuaddr_left = NTADR_A(x, y);
    int ppuaddr_right = NTADR_A(x+1, y);
    auto idx = VRAM_INDEX;
    auto tile = all_letters[letter];
    VRAM_BUF[idx+ 0] = MSB(ppuaddr_left) | NT_UPD_VERT;
    VRAM_BUF[idx+ 1] = LSB(ppuaddr_left);
    VRAM_BUF[idx+ 2] = 3;
    // VRAM_BUF[idx+ 3] = tile->top_topleft;
    VRAM_BUF[idx+ 3] = tile->top_botleft;
    VRAM_BUF[idx+ 4] = tile->bot_topleft;
    VRAM_BUF[idx+ 5] = tile->bot_botleft;
    VRAM_BUF[idx+ 6] = MSB(ppuaddr_right) | NT_UPD_VERT;
    VRAM_BUF[idx+ 7] = LSB(ppuaddr_right);
    VRAM_BUF[idx+ 8] = 3;
    // VRAM_BUF[idx+10] = tile->top_topright;
    VRAM_BUF[idx+9] = tile->top_botright;
    VRAM_BUF[idx+10] = tile->bot_topright;
    VRAM_BUF[idx+11] = tile->bot_botright;
    VRAM_BUF[idx+12] = 0xff; // terminator bit
    VRAM_INDEX += 12;
}

extern "C" void render_string(uint8_t x, uint8_t y, const Letter str[]) {
    while (*str != Letter::NUL) {
        auto letter = *str;
        draw_letter(x, y, letter);
        x += 2;
        if (x >= 32) {
            x = 0;
            y += 4;
        }
        str++;
        if (VRAM_INDEX > (128 - 14)) {
            flush_vram_update2();
        }
    }
    NAME_UPD_ENABLE = true;
}
