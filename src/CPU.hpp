#pragma once

#include "PCH.hpp"
#include "MMU.hpp"

class CPU
{
public:
    CPU();
    ~CPU();

    void StepFrame();

private:
    // OpCode Functions
    void NOP();

private:
    // MMU (Memory Map Unit)
    std::unique_ptr<MMU> m_MMU;

    // Clock cycles
    unsigned int m_cycles; // The current number of cycles

    // Registers
    unsigned short m_AF; // Accumulator & flags
    unsigned short m_BC; // General purpose
    unsigned short m_DE; // General purpose
    unsigned short m_HL; // General purpose
    unsigned short m_SP; // Stack pointer
    unsigned short m_PC; // Program counter

    // OpCode Function Map
    typedef void(CPU::*opCodeFunction)();
    opCodeFunction m_operationMap[0xFF];
};
