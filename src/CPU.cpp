#include "PCH.hpp"
#include "CPU.hpp"
#include "Logger.hpp"

CPU::CPU() :
    m_cycles(0),
    m_maxCycles(70244),
    m_AF(0x0000),
    m_BC(0x0000),
    m_DE(0x0000),
    m_HL(0x0000),
    m_SP(0x0000),
    m_PC(0x0000)
{
    Logger::Log("CPU Created.");

    // Initialize memory to 0x0000
    for (int i = 0; i < 0xFFFF; i++)
    {
        m_memory[i] = 0x0000;
    }
    
    // Initialize the operationMap
    m_operationMap[0x00] = &CPU::NOP;
}

CPU::~CPU()
{
    Logger::Log("CPU Destroyed.");
}

void CPU::StepFrame()
{
    while (m_cycles <= m_maxCycles)
    {
        // Read through the memory, starting at m_PC
        // Execute the correct function for each OpCode
        opCodeFunction func = m_operationMap[m_memory[m_PC]];
        if (func != nullptr)
        {
            (this->*func)();
        }
        else
        {
            Logger::LogError("OpCode 0x%02X could not be interpreted.", m_memory[m_PC]);
            // HALT
            return;
        }
    }

    // Reset the cycles. If we went over our max cycles, the next frame will start a
    // few cycles ahead.
    m_cycles -= m_maxCycles;
}

void CPU::NOP()
{
    m_cycles += 4;
    m_PC += 1;
}