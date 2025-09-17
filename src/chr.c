
// Include the 8kb GG chr data and force the linker to not discard it.
__attribute__((section(".chr_rom"))) __attribute__((retain)) const unsigned char chr[] = {
    #embed "../GG-8K.chr"
};
