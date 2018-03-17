# GameLad

The [GameLad](http://declanhopkins.com/gamelad/) is an almost finished gameboy emulator that was made in conjunction with [tyren](https://github.com/TyrenDe).

![GameLad](https://i.imgur.com/QDiatSD.jpg)

It supports Tetris and some other games with a few minor issues. The emulator passes all CPU instruction tests, as well as the instruction timing tests, which means our emulation of the GameBoy's Z80 chip is pretty solid. There isn't audio support :(

Learn more [here](http://declanhopkins.com/projects/gamelad.html).

Technology     | Purpose
---------------|----------
**C++14**      | Core
**SDL2**       | Windowing, Rendering, & Input
**VS2013**     | Windows Compilation
**make**       | Linux and OSX Compilation
