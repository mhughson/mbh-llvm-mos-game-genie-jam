# LLVM-MOS-SDK Template for Game Genie Game Jam 2025

## What's inside this example?

* A bootstraping CMake build step that will download the compiler toolchain if its not found
* VSCode integration with recommended extensions
* Clangd integration for inline suggestions and compiler feedback (including tooling like `Go To Definition` and more!)
* CA65 and CMake preconfigured to build CA65 source files (ca65 compiler binaries are included for windows users to reduce first time install pain)
* NESLIB / NESDOUG / FAMITONE C libraries included
* Example Project using the aformentioned libraries which demos the following items:
  * Basic initialization for a NES ROM (including setting up the INES header)
  * Includes the CHR for the Game Genie
  * Barebones CA65 asm integration example
  * Draws a simple background image to the screen (thanks Freem for the image!)
  * Mid-level platformer character controller to demonstrate input.

Note: I put this together really fast so it may have bugs in it. I really only had time to test Windows as well.

## How do I build the example?

If you have experience using CMake, then don't let me tell you how to live your life.

But if you don't, here's my recommendations.

* Download and install CMake <https://cmake.org/download/>
* Download VSCODE <https://code.visualstudio.com/Download> and open up the project folder
* Install the recommended extensions (which are the following list of extensions:)
  * `alchemic-raker.alchemy65` - used for ca65 source code highlighting
  * `llvm-vs-code-extensions.vscode-clangd` - generates inline error reporting and `Go To Definition` support for the editor
  * `ms-vscode.cmake-tools` - lets you configure and build from the editor
  * `pucelle.run-on-save` - configured to automatically build the nes file whenever you save with `Ctrl+S`
* Push `Ctrl+Shift+P` to open the VSCode command window and type `CMake: Configure` and choose the `default` preset
* Push `Ctrl+Shift+P` again to open the VSCode command window and this time type `CMake: Build` to build the `build/gg-llvm-mos-sample.nes` file
* From here on out, whenever you change a source file and save, it should automatically rebuild the application.

## Questions no one asked but I wanted to answer anyway

## What is the Game Genie Game Jam 2025?

The Game Genie is a NES Cartridge accessory that allowed players to input cheat codes into a menu
which would affect the game that was plugged into the Game Genie.
But we don't particularly care about that for this Game Jam.
Instead, we are pretending to make our own custom games that run *on* the Game Genie cart itself.

For more details see <https://forums.nesdev.org/viewtopic.php?p=304053#p304053>

## What is LLVM-MOS

LLVM-MOS is a new backend for the powerful LLVM compiler suite that enables it to target the 6502 CPU
used in the NES.
It includes a full C and C++ compiler, with parts of the C++ standard library available (mostly only
things that are compile-time only since the NES doesn't have a lot of space on it.)
It's quite powerful, but also quite new, so not as many people have experience with it.

## What is LLVM-MOS-SDK

LLVM-MOS-SDK is a prepackaged build of the LLVM-MOS tools with many helpful examples and targets to make
building for the NES much easier.
While not strictly required, without it, you'd end up needing to bring your own linker scripts and so much more
just to get started.
