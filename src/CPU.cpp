#include "PCH.hpp"
#include "CPU.hpp"
#include "Logger.hpp"

// The number of CPU cycles per frame
const unsigned int CyclesPerFrame = 70244;

CPU::CPU() :
    m_cycles(0),
    m_isHalted(false),
    m_AF(0x0000),
    m_BC(0x0000),
    m_DE(0x0000),
    m_HL(0x0000),
    m_SP(0x0000),
    m_PC(0x0000)
{
    Logger::Log("CPU Created.");

    // Create the MMU
    m_MMU = std::make_unique<MMU>();

    // Initialize the operationMap
    m_operationMap[0x00] = &CPU::NOP;
    m_operationMap[0x31] = &CPU::LDSPnn;
}

CPU::~CPU()
{
    m_MMU.reset();

    Logger::Log("CPU Destroyed.");
}

bool CPU::Initialize()
{
    return m_MMU->Initialize();
}

void CPU::StepFrame()
{
    if (m_isHalted)
    {
        return;
    }

    while (m_cycles < CyclesPerFrame)
    {
        // Read through the memory, starting at m_PC
        byte opCode = m_MMU->ReadByte(m_PC);
        // Execute the correct function for each OpCode
        opCodeFunction instruction;
        instruction = m_operationMap[opCode];

        if (instruction != nullptr)
        {
            (this->*instruction)();
        }
        else
        {
            Logger::LogError("OpCode 0x%02X could not be interpreted.", opCode);
            HALT();
            return;
        }
    }

    // Reset the cycles. If we went over our max cycles, the next frame will start a
    // few cycles ahead.
    m_cycles -= CyclesPerFrame;
}

void CPU::HALT()
{
    m_isHalted = true;
    Logger::Log("!!!! HALTED !!!!");
}

/*
    CPU INSTRUCTIONS
*/

void CPU::NOP()
{
    m_PC += 1;
    m_cycles += 4;
}

void CPU::LDSPnn()
{
    m_PC += 1; // Look at the first byte of nn
    unsigned short nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_SP = nn;
    m_PC += 2; // Move onto the next instruction
    m_cycles += 8;
}