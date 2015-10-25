#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <cstdarg>
#include <memory>

#if WINDOWS
    #include <SDL.h>
#else
    #include "SDL2/SDL.h"
#endif

#include "Logger.hpp"

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;

#define ARRAYSIZE(a) (sizeof a / sizeof a[0])
