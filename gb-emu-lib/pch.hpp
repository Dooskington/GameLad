#pragma once

#include <cstring>
#include <iostream>
#include <fstream>

#include <cstdarg>
#include <memory>

#include <map>
#include <cmath>

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;

#include "Logger.hpp"
#include "IMemoryUnit.hpp"
#include "ICPU.hpp"
#include "IMMU.hpp"

#if WINDOWS
    #include <SDL.h>
#else
    #include "SDL2/SDL.h"
#endif

#define ARRAYSIZE(a) (sizeof a / sizeof a[0])
#define ISBITSET(val, bit) (((val >> bit) & 0x01) == 0x01)
#define SETBIT(val, bit) (val | (1 << bit))
#define CLEARBIT(val, bit) (val & ~(1 << bit))
