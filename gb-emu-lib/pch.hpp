#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <cstdarg>
#include <memory>

#include <map>

#if WINDOWS
    #include <SDL.h>
#else
    #include "SDL2/SDL.h"
#endif

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;

#include "Logger.hpp"
#include "IMemoryUnit.hpp"
#include "ICPU.hpp"
#include "IMMU.hpp"

#define ARRAYSIZE(a) (sizeof a / sizeof a[0])
#define ISBITSET(val, bit) (((val >> bit) & 0x01) == 0x01)
