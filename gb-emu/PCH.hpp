#pragma once

#include <cstring>
#include <memory>

#if WINDOWS
    #include <SDL.h>
#else
    #include "SDL2/SDL.h"
#endif

#include "Logger.hpp"

typedef unsigned char byte;
