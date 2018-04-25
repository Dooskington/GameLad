# GameLad

The [GameLad](http://declanhopkins.com/gamelad/) is an almost finished gameboy emulator that was made in conjunction with [tyren](https://github.com/TyrenDe).

![GameLad](https://i.imgur.com/QDiatSD.jpg)

It supports Tetris and some other games with a few minor issues. The emulator passes all CPU instruction tests, as well as the instruction timing tests, which means our emulation of the GameBoy's Z80 chip is pretty solid. There isn't audio support :(

Learn more [here](http://declanhopkins.com/projects/gamelad.html).

Technology     | Purpose
---------------|----------
**C++14**      | Core
**SDL2**       | Windowing, Rendering, & Input
**VS2015+**    | Windows Compilation

# Building
* Clone and bootstrap per instructions at https://github.com/microsoft/vcpkg
* For example:
* `mkdir microsoft`
* `cd microsoft`
* `git clone https://github.com/Microsoft/vcpkg.git`
* `cd vcpkg`
* *Linux:* `./bootstrap-vcpkg.sh`
* *Windows:* `bootstrap-vcpkg.bat`

## Linux
* `./compile.sh <path_to_cloned_vcpkg>`
*     For example: ./compile.sh ~/git/microsoft/vcpkg

## Windows
* Open VS Developer Command Prompt
* Run: `compile.bat <path_to_cloned_vcpkg>`
*     For example: compile.bat E:\Git\microsoft\vcpkg