#pragma once

#include <string>
#include <iostream>

#include <stdarg.h>
#include <memory>

#if WINDOWS
    #include <SDL.h>
    #include <SDL_mixer.h>

    #define vsnprintf _vsnprintf
#else
    #include "SDL2/SDL.h"
    #include "SDL2_mixer/SDL_mixer.h"

    // TODO: Doosk to add the correct #define macro here to map to OSX's printf if necessary
    //#define vsnprintf _vsnprintf???
#endif

#include "Logger.hpp"
