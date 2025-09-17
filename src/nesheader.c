
// This file probably doesn't need to be edited. This sets the header options
// for llvm-mos to fit within the game genie jam limitations.

#include <ines.h>

// Game Genie only has 16kb of ROM data
MAPPER_PRG_ROM_KB(16);
// Technically it only uses 256 bytes of CHR data, but we just rep
MAPPER_CHR_ROM_KB(8);
// For the game jam, we can choose what mirroring to use. Default to vertical mirroring but you can change this.
MAPPER_USE_VERTICAL_MIRRORING;
