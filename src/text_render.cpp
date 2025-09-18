

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

extern "C" void render_string(Nametable nmt, uint8_t x, uint8_t y, const Letter str[]) {
    while (*str != Letter::NUL) {
        auto letter = *str;
        str++;
        if (!SKIP_DRAWING_SPACE || letter != Letter::SPACE) {
            const auto t = all_letters[letter].get();
            draw_metatile_2_3(nmt, x, y, &t);
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
