#pragma once

#include <cstring>
#include <string>
#include <memory>
#include <cstdlib>

#if WINDOWS
    #include <SDL.h>
#else
    #include "SDL2/SDL.h"
#endif

#include "Logger.hpp"

typedef unsigned char byte;
