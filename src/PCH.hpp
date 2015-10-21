// TODO Make this an actual precompiled header
#pragma once

#include <iostream>

#if WINDOWS
    #include <SDL.h>
    #include <SDL_mixer.h>
#else
    #include "SDL2/SDL.h"
    #include "SDL2_mixer/SDL_mixer.h"
#endif
