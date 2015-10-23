#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <cstdarg>
#include <memory>

#if WINDOWS
    #include <SDL.h>
   // #include <SDL_mixer.h>
#else
    #include "SDL2/SDL.h"
   // #include "SDL2_mixer/SDL_mixer.h"
#endif

#include "Logger.hpp"

typedef char byte;

#define ARRAYSIZE(a) (sizeof a / sizeof a[0])
