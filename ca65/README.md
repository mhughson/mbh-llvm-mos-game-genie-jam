# FOR ADVANCED USERS

If you have a lot of knowledge about the 6502 platform, and already have a fondness for coding in ca65,
then this might be the place for you.
LLVM-MOS allows linking ca65 object files with the rest of the application, allowing you to use both the
power of the llvm-mos C/C++ compiler, with the power of the ca65 assembler.

## CA65 source folder

This folder is configured in `CMakeLists.txt` to compile any `.s` or `.asm` files inside with the ca65 compiler.
If you'd like to use this for custom ASM instead of writing llvm ASM, check out `example.s` for more details.

Also see <https://llvm-mos.org/wiki/Cc65_integration> for more details about this integration in general.
