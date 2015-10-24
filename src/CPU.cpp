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
    m_operationMap[0x21] = &CPU::LDHLnn;
    m_operationMap[0x31] = &CPU::LDSPnn;
    m_operationMap[0x32] = &CPU::LDD_HL_A;
    m_operationMap[0xAF] = &CPU::XORA;
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

byte CPU::GetHighByte(ushort dest)
{
    return ((dest >> 8) & 0xFF);
}

byte CPU::GetLowByte(ushort dest)
{
    return (dest & 0xFF);
}

void CPU::SetHighByte(ushort* dest, byte val)
{
    byte low = GetLowByte(*dest);
    *dest = (val << 8) + low;
}

void CPU::SetLowByte(ushort* dest, byte val)
{
    byte high = GetHighByte(*dest);
    *dest = (high << 8) + val;
}

void CPU::HALT()
{
    m_isHalted = true;
    Logger::Log("!!!! HALTED !!!!");
}

/*
    CPU INSTRUCTIONS
*/

// 0x00 (NOP)
void CPU::NOP()
{
    m_PC += 1;
    m_cycles += 4;
}

// 0x21 (LD HL, nn)
void CPU::LDHLnn()
{
    m_PC += 1; // Look at the first byte of nn
    ushort nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_HL = nn;
    m_PC += 2; // Move onto the next instruction
    m_cycles += 8;
}

// 0x31 (LD SP, nn)
void CPU::LDSPnn()
{
    m_PC += 1; // Look at the first byte of nn
    ushort nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_SP = nn;
    m_PC += 2; // Move onto the next instruction
    m_cycles += 8;
}

// 0x32 (LDD (HL), A)
void CPU::LDD_HL_A()
{
    m_PC += 1; // Look at A
    byte A = m_MMU->ReadByte(m_PC); // Read A
    m_MMU->SetMemory(m_HL, A); // Load A into the address pointed at by HL.
    m_HL--;
    m_PC += 1; // Move onto the next instruction
    m_cycles += 8;
}

// 0xAF (XOR A)
void CPU::XORA()
{
    m_PC += 1;
    SetHighByte(&m_AF, GetHighByte(m_AF) ^ GetHighByte(m_AF));
    m_cycles += 4;
}