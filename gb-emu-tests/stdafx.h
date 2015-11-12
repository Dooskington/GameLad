// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#if WINDOWS2
    #include "targetver.h"

    // Headers for CppUnitTest
    #include "CppUnitTest.h"

    using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#else
    #define TEST_CLASS(className) class className
    #define TEST_METHOD(methodName) void methodName()

    class Assert
    {
    public:
        static void AreEqual(int expected, int actual) { }
        static void IsTrue(bool condition) { }
        static void IsFalse(bool condition) { Assert::IsTrue(!condition); }
    };
#endif

#include <cstring>
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
#define SETBIT(val, bit) (val | (1 << bit))
#define CLEARBIT(val, bit) (val & ~(1 << bit))
