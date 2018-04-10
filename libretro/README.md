Libretro core for Hack CPU
==========================

Nand2Tetris is a really great course, but the emulator it ships with isn't _that_ great -- How good is a game for the Hack CPU if It doesn't run on my console.

[libretro](https://www.libretro.com/) comes to rescue -- A unified API for writing emulators and that is supported by all kinds of frontends: From Android Phones to XBox Consoles.

This project implements a simple and small libretro core to run Hack binaries (.hack files), and works nicely :)


Limitations
===========

CPU Speed
---------
The CPU speed is not fixed for the Hack CPU -- I have settled at 2MHz, with screen refreshes at 60Hz, which seems to work nicely with the Pong game.
Booting the game takes a few seconds, but everything works smoothly thereafter.

But this should be configurable somehow. A few ideas:
- Use cheat codes to specify clock frequency
- Annotate input files with things like `# cpu_speed = 2000000`
- Add a new register to set clock speed
- Add a new register to delay a fixed amount of time / wait until next vsync



Input
-----

I'm currently mapping libretro's virtual Joypad to the Hack Keyboard (Using a pretty arbitrary mapping).

Libretro also supports full Keyboards, and I should switch to use that instead (And leave the Joypad as a fallback for systems without a keyboard).



Future Improvements
===================

New Peripherals
---------------

The Hack computer has been defined with a lot of unmapped memory -- We could use it to create a few extra peripherals:
- Built-in support for Mouse and Joypads
- Sound (Everyone loves buzzy music)
- Better screen (We could trade higher resolution if we don't map it entirely in the RAM)
- System calls:
    - stdin/stdout/stderr Pipes (So that I can use the Hack CPU in a terminal)
    - Filesystem access
    - Sockets
    - OpenGL Graphics (why not, even [Bash got OpenGL support recently](https://opensource.com/article/18/4/opengl-bindings-bash))
    - etc

The new peripherals could either be "always there", in a usually ignored address, or configurable via source file annotations, like `# buzzer @addr`


Built-in Compiler
-----------------

A future version should be able to parse files in either Binary, Hack Assembly, VM assembly, Jack source. Possibly all of it mixed up.
