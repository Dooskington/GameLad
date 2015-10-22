#pragma once

#include "PCH.hpp"

class CPU
{
public:
    CPU();
    ~CPU();

    void StepFrame();
    void NOP();

    // Registers
    unsigned short AF; // Accumulator & flags
    unsigned short BC; // General purpose
    unsigned short DE; // General purpose
    unsigned short HL; // General purpose
    unsigned short SP; // Stack pointer
    unsigned short PC; // Program counter

    // Memory
    char memory[0xFFFF]; // 64k array that will act as memory

    // OpCode Functions
    typedef void(CPU::*opCodeFunction)();
    opCodeFunction operationMap[0xFF];
};
