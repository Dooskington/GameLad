# GameLad

The [GameLad](http://declanhopkins.com/gamelad/) is a (relatively) fully featured gameboy emulator that I have worked on for a few months in conjunction with [TyrenDe](https://github.com/TyrenDe).

![GameLad](http://declanhopkins.com/static/images/previews/gamelad_preview.png)

It supports Tetris and some other games with a few minor issues. The emulator passes all CPU instruction tests, as well as the instruction timing tests, which means our emulation of the GameBoy's Z80 chip is pretty solid.

Development began in October 2015.

Learn more [here](http://declanhopkins.com/gamelad/).

Technology     | Purpose
---------------|----------
**C++14**      | Core
**SDL2**       | Windowing, Rendering, & Input
**VS2013**     | Windows Compilation
**make**       | Linux and OSX Compilation
