#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Bitpacks a 2x1 strip of tiles into a single byte
#define LEFT_TILE(x) (((x) >> 4) & 0xf)
#define RIGHT_TILE(x) ((x) & 0xf)

struct Metatile_2_2 {
    uint8_t top;
    uint8_t bot;
};

struct Metatile_2_3 {
    uint8_t top_top;
    uint8_t top_bot;
    uint8_t bot_top;
};

struct Metatile_4_4 {
    uint8_t topleft_top;
    uint8_t topleft_bot;
    uint8_t botleft_top;
    uint8_t botleft_bot;
    uint8_t topright_top;
    uint8_t topright_bot;
    uint8_t botright_top;
    uint8_t botright_bot;
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

consteval Metatile_4_4 parse_string_mt_4_4(const char* text, size_t len) {
    // loop through the letters of the provided string looking for any special characters.
    // Replace a space character with a since bit 0 and any other non-ignored character with a bit 1
    // I'm sure I could've done this in a better way but this works well enough for now.
    uint64_t bits = 0;
    int i = 0;
    // Require all tiles passed into mt to have at least 4px per row and 8 px per column
    char c = text[i];
    for (int row=0; row < 8; row++) {
        // skip anything else until the next pipe
        while (c != '|' && i < len) c = text[i++];
        // skip over the left |
        if (c == '|') c = text[i++];
        // then read 8 px (or until the next pipe or newline)
        for (int col=0; col < 8; col++) {
            // Any non-"blank" character will use color 3, and any "blank" or missing characters will use 
            uint8_t value = (c == ' ' || c == '|' || c == '\n') ? 0 : 1;
            bits <<= 1;
            bits |= value;
            if (c != '|' && c != '\n')
                c = text[i++];
        }
        // skip to the next line
        while (c != '\n' && i < len) c = text[i++];
    }
    // Screaming internally.
    constexpr uint8_t stride = 8;
    return {
        .topleft_top  = (uint8_t)((get_tile_for_bits((((bits >> (54 + stride)) & 0x3) << 2) | ((bits >> (54)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> (52 + stride)) & 0x3) << 2) | ((bits >> (52)) & 0x3))),
        .topleft_bot  = (uint8_t)((get_tile_for_bits((((bits >> (38 + stride)) & 0x3) << 2) | ((bits >> (38)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> (36 + stride)) & 0x3) << 2) | ((bits >> (36)) & 0x3))),

        .botleft_top  = (uint8_t)((get_tile_for_bits((((bits >> (22 + stride)) & 0x3) << 2) | ((bits >> (22)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> (20 + stride)) & 0x3) << 2) | ((bits >> (20)) & 0x3))),
        .botleft_bot  = (uint8_t)((get_tile_for_bits((((bits >> ( 6 + stride)) & 0x3) << 2) | ((bits >> ( 6)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> ( 4 + stride)) & 0x3) << 2) | ((bits >> ( 4)) & 0x3))),

        .topright_top = (uint8_t)((get_tile_for_bits((((bits >> (50 + stride)) & 0x3) << 2) | ((bits >> (50)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> (48 + stride)) & 0x3) << 2) | ((bits >> (48)) & 0x3))),
        .topright_bot = (uint8_t)((get_tile_for_bits((((bits >> (34 + stride)) & 0x3) << 2) | ((bits >> (34)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> (32 + stride)) & 0x3) << 2) | ((bits >> (32)) & 0x3))),

        .botright_top = (uint8_t)((get_tile_for_bits((((bits >> (18 + stride)) & 0x3) << 2) | ((bits >> (18)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> (16 + stride)) & 0x3) << 2) | ((bits >> (16)) & 0x3))),
        .botright_bot = (uint8_t)((get_tile_for_bits((((bits >> ( 2 + stride)) & 0x3) << 2) | ((bits >> ( 2)) & 0x3)) << 4)
                                 | get_tile_for_bits((((bits >> ( 0 + stride)) & 0x3) << 2) | ((bits >> ( 0)) & 0x3))),
    };
}
consteval Metatile_2_2 parse_string_mt_2_2(const char* text, size_t len) {
    Metatile_4_4 mt = parse_string_mt_4_4(text, len);
    return {
        .top = mt.topleft_top,
        .bot = mt.topleft_bot,
    };
}

consteval Metatile_2_3 parse_string_mt_2_3(const char* text, size_t len) {
    Metatile_4_4 mt = parse_string_mt_4_4(text, len);
    return {
        .top_top = mt.topleft_top,
        .top_bot = mt.topleft_bot,
        .bot_top = mt.botleft_top,
    };
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <soa.h>

#define SOA_STRUCT Metatile_2_2
#define SOA_MEMBERS MEMBER(top) MEMBER(bot)
#include <soa-struct.inc>

#define SOA_STRUCT Metatile_2_3
#define SOA_MEMBERS MEMBER(top_top) MEMBER(top_bot) MEMBER(bot_top)
#include <soa-struct.inc>

#define SOA_STRUCT Metatile_4_4
#define SOA_MEMBERS MEMBER(topleft_top) MEMBER(topleft_bot) MEMBER(botleft_top) MEMBER(botleft_bot) \
                    MEMBER(topright_top) MEMBER(topright_bot) MEMBER(botright_top) MEMBER(botright_bot)
#include <soa-struct.inc>

#include <cstdint>
#include <cstddef>

consteval Metatile_2_2 operator""_mt_2_2(const char* text, size_t len) {
    return parse_string_mt_2_2(text, len);
}
consteval Metatile_2_3 operator""_mt_2_3(const char* text, size_t len) {
    return parse_string_mt_2_3(text, len);
}
consteval Metatile_4_4 operator""_mt_4_4(const char* text, size_t len) {
    return parse_string_mt_4_4(text, len);
}

#endif