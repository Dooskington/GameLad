# GameLad

This is a gameboy emulator that I made with [tyren](https://github.com/TyrenDe) in my early college days.

![GameLad](https://i.imgur.com/QDiatSD.jpg)

It supports Tetris and some other games with a few minor issues. The emulator passes all CPU instruction tests, as well as the instruction timing tests.

## Controls

Directions are `W`/`A`/`S`/`D`, A/B are `K`/`L`, and Start/Select are `N`/`M`.

SDL-compatible game controllers are enabled by default. The D-pad and left
stick control movement. The east face button is Game Boy A, the south face
button is Game Boy B, Start/Menu is Start, and Back/View/Share is Select.
SDL normalizes these positions across Xbox, PlayStation, Nintendo, and other
known controller layouts.

To disable gamepad initialization, pass `--disable-gamepad` before or after the
existing positional arguments:

```bat
build\bin\gb-emu.exe 3 "E:\ROMs\Tetris.gb" --disable-gamepad
```

Learn more [here](https://github.com/Dooskington/GameLad/wiki).

Technology     | Purpose
---------------|----------
**C++14**      | Core
**SDL2**       | Windowing, Rendering, & Input
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
* *Linux:* `./bootstrap-vcpkg.sh`
* *Windows:* `bootstrap-vcpkg.bat`

NOTE: The bootstrap may fail, if it does, follow any instructions listed and try again.

## Linux
* Install a C++ compiler, GNU Make, and CMake 3.16 or newer.
* You may need to run: `chmod 700 compile.sh`
* Run `./compile.sh` for a Debug build or `./compile.sh --release` for Release.

The Linux build produces `build-linux/bin/gamelad_libretro.so`,
`build-linux/bin/gamelad_libretro.info`, and both test executables. It does not
require SDL or vcpkg. To build the optional desktop frontend, install the SDL2
development package and configure CMake with
`-DGAMELAD_BUILD_SDL_FRONTEND=ON`.

## Windows
* Open VS Developer Command Prompt
* Run: `compile.bat <path_to_cloned_vcpkg> [--release]`
*     For example: compile.bat E:\Git\microsoft\vcpkg

The Windows build produces the desktop emulator at `build\bin\gb-emu.exe`
and the libretro files at `build\bin\gamelad_libretro.dll` and
`build\bin\gamelad_libretro.info`. It builds Debug by default; pass `--release`
after the vcpkg path for an optimized Release build.

## RetroArch core

`gamelad_libretro.dll` on Windows and `gamelad_libretro.so` on Linux are
standalone libretro cores for Game Boy and Game Boy Color ROMs. On Windows,
build from a Visual Studio Developer Command Prompt whose architecture matches
the RetroArch installation. For example, use an x64 prompt for 64-bit
RetroArch:

```bat
compile.bat E:\Git\microsoft\vcpkg --release
```

To install the resulting core manually:

1. In RetroArch, open **Settings > Directory** and note the **Cores** and
   **Core Info** directories.
2. Close RetroArch, copy `build\bin\gamelad_libretro.dll` into the **Cores**
   directory, and copy `build\bin\gamelad_libretro.info` into the **Core Info**
   directory. Keep the matching `gamelad_libretro` filenames unchanged.
3. Start RetroArch and choose
   **Main Menu > Load Core > Nintendo - Game Boy / Color (GameLad)**.
4. Choose **Load Content** and select a `.gb` or `.gbc` ROM.

The core uses the standard RetroPad D-pad, A, B, Start, and Select mappings.
RetroPad X and Y provide Turbo A and Turbo B. RetroArch controller remapping,
overlays, shaders, scaling, screenshots, recording, rewind, run-ahead, and
serialization-based netplay can operate through the normal frontend features.

Implemented core integrations include:

* Deterministic save states, including CPU, PPU, APU, timers, DMA, MBC, SRAM,
  RTC, and mid-frame state.
* RetroArch-managed battery RAM and MBC3 RTC persistence.
* Game Genie (`XXX-XXX` / `XXX-XXX-XXX`) and GameShark (`XXXXXXXX`) cheats.
* Memory descriptors and RetroAchievements support.
* Core options for hardware model, boot ROM use, opposing directions, and
  audio-buffer-driven frameskip.
* VBlank-latched video output, so frontends never receive a framebuffer while
  the PPU is progressively overwriting it for the next hardware frame.
* MBC5 cartridge rumble forwarded to the frontend controller.
* In-memory and archive-extracted ROM loading, plus frontend VFS access.
* Two-player link-cable communication through RetroArch's netpacket interface.

Boot ROMs are optional and disabled by default. To use them, place a 256-byte
`gb_bios.bin` and/or a 2304-byte `gbc_bios.bin` in RetroArch's **System**
directory, then enable **Use Official Boot ROM** in **Core Options**. GameLad
selects the matching file for the active hardware model. Close and reload
content after changing the hardware-model or boot-ROM options.

Game Boy Camera, MBC7 tilt sensors, and Super Game Boy features remain
unsupported because those cartridge/system devices are not implemented by the
emulator. Hardware rendering and disk control do not apply to Game Boy
cartridges. Link-cable networking is covered by the core integration tests but
has not yet been exercised against a live RetroArch multiplayer session. Save
states are deterministic within a build and platform, but are not advertised as
portable between architectures or different core versions.
