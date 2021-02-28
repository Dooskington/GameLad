# GameLad

This is a gameboy emulator that I made with [tyren](https://github.com/TyrenDe) in my early college days.

![GameLad](https://i.imgur.com/QDiatSD.jpg)

It supports Tetris and some other games with a few minor issues. The emulator passes all CPU instruction tests, as well as the instruction timing tests.

Learn more [here](https://github.com/Dooskington/GameLad/wiki).

Technology     | Purpose
---------------|----------
**C++14**      | Core
**SDL2**       | Windowing, Rendering, Sound, & Input
**VS2015+**    | Windows Compilation

# Pull Requests
If you want to submit a pull request for the audio system, or other small fixes, be my guest. Just stay consistent with the style of the rest of the code. The best place to start is probably [here](https://github.com/Dooskington/GameLad/blob/master/gb-emu-lib/APU.cpp).

# What is all this code doing?
The best place to start is [here](https://github.com/Dooskington/GameLad/wiki).

Then, follow the code of the main emulator loop that starts [here](https://github.com/Dooskington/GameLad/blob/master/gb-emu/Main.cpp#L220).

# Building
* Clone and bootstrap per instructions at https://github.com/microsoft/vcpkg
* For example:
* `mkdir microsoft`
* `cd microsoft`
* `git clone https://github.com/Microsoft/vcpkg.git`
* `cd vcpkg`
* *Linux:* `sudo apt-get install build-essential`
* *macOS:* `xcode-select --install`
* *Linux/macOS:* `./bootstrap-vcpkg.sh`
* *Windows:* `bootstrap-vcpkg.bat`

NOTE: The bootstrap may fail, if it does, follow any instructions listed and try again.

## Linux/macOS
* You may need to run: `chmod 700 compile.sh`
* `./compile.sh <path_to_cloned_vcpkg>`
*     For example: ./compile.sh ~/git/microsoft/vcpkg

## Windows
* Open VS Developer Command Prompt
* Run: `compile.bat <path_to_cloned_vcpkg>`
*     For example: compile.bat E:\Git\microsoft\vcpkg
