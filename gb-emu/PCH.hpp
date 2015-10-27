#pragma once

#include <string>
#include <memory>

#if WINDOWS
    #include <SDL.h>
#else
    #include "SDL2/SDL.h"
#endif

#include "Logger.hpp"
