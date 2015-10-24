#pragma once

#include "PCH.hpp"
#include "MMU.hpp"

class CPU
{
public:
    CPU();
    ~CPU();

    bool Initialize();
    void StepFrame();

private:
    byte GetHighByte(ushort dest);
    byte GetLowByte(ushort dest);

    void SetHighByte(ushort* dest, byte val);
    void SetLowByte(ushort* dest, byte val);

    void SetFlag(byte bit);
    void ClearFlag(byte bit);
    bool IsFlagSet(byte bit);

    void HALT();

    // OpCode Functions
    void NOP();             // 0x00
    void LDHLnn();          // 0x21
    void LDSPnn();          // 0x31
    void LDD_HL_A();        // 0x32
    void XORA();            // 0xAF

private:
    // MMU (Memory Map Unit)
    std::unique_ptr<MMU> m_MMU;

    // Clock cycles
    unsigned int m_cycles; // The current number of cycles
    bool m_isHalted;

    // Registers
    ushort m_AF; // Accumulator & flags
    ushort m_BC; // General purpose
    ushort m_DE; // General purpose
    ushort m_HL; // General purpose
    ushort m_SP; // Stack pointer
    ushort m_PC; // Program counter

    // OpCode Function Map
    typedef void(CPU::*opCodeFunction)();
    opCodeFunction m_operationMap[0xFF];
};
