#pragma once

#include "MMU.hpp"
#include "Cartridge.hpp"
#include "GPU.hpp"
#include "APU.hpp"
#include "Joypad.hpp"
#include "Serial.hpp"
#include "Timer.hpp"

/*
The Flag Register (lower 8bit of AF register)
Bit  Name  Set Clr  Expl.
7    zf    Z   NZ   Zero Flag
6    n     -   -    Add/Sub-Flag (BCD)
5    h     -   -    Half Carry Flag (BCD)
4    cy    C   NC   Carry Flag
3-0  -     -   -    Not used (always zero)
*/
#define ZeroFlag        7
#define AddFlag         6
#define HalfCarryFlag   5
#define CarryFlag       4

class CPU : public ICPU
{
    friend class CPUTests;

public:
    CPU();
    ~CPU();

private:
    bool Initialize(IMMU* pMMU);

public:
    bool Initialize();
    bool LoadROM(std::string path);
    void StepFrame();
    void TriggerInterrupt(byte interrupt);

private:
    void Step();

    byte GetHighByte(ushort dest);
    byte GetLowByte(ushort dest);

    void SetHighByte(ushort* dest, byte val);
    void SetLowByte(ushort* dest, byte val);

    void SetFlag(byte flag);
    void ClearFlag(byte flag);
    bool IsFlagSet(byte flag);

    void HALT();

    // OpCode Functions
    void NOP();             // 0x00
    void INCC();            // 0x0C
    void LDCe();            // 0x0E
    void LDDEnn();          // 0x11
    void LDA_DE_();         // 0x1A
    void JRNZe();           // 0x20
    void LDHLnn();          // 0x21
    void LDSPnn();          // 0x31
    void LDD_HL_A();        // 0x32
    void LDAe();            // 0x3E
    void LD_HL_A();         // 0x77
    void XORA();            // 0xAF
    void LD_0xFF00C_A();    // 0xE0
    void LD_0xFF00n_A();    // 0xE2

    // OpCode 0xCB functions
    void BIT7h();           // 0x7C

private:
    // MMU (Memory Map Unit)
    std::unique_ptr<IMMU> m_MMU;

    // Cartridge
    std::unique_ptr<Cartridge> m_cartridge;

    // GPU
    std::unique_ptr<GPU> m_GPU;

    // APU
    std::unique_ptr<APU> m_APU;

    // Joypad
    std::unique_ptr<Joypad> m_joypad;

    // Serial
    std::unique_ptr<Serial> m_serial;

    // Timer
    std::unique_ptr<Timer> m_timer;

    // Clock cycles
    unsigned long m_cycles; // The current number of cycles
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
    opCodeFunction m_operationMap[0xFF + 1];
    opCodeFunction m_operationMapCB[0xFF + 1];
};
