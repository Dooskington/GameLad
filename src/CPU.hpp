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

    void SetFlag(byte flag);
    void ClearFlag(byte flag);
    bool IsFlagSet(byte flag);
    bool IsBitSet(byte val, byte bit);

    void HALT();

    // OpCode Functions
    void NOP();
    void LDHLnn();
    void LDSPnn();
    void LDD_HL_A();
    void XORA();
    void JRNZe();
    void LDCe();
    void LDAe();
    void LD_0xFF00C_A();
    void INCC();
    
    // OpCode CD functions
    void BIT7h();

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
    opCodeFunction m_operationMapCB[0xFF];
};