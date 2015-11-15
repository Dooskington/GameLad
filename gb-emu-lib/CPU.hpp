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
    bool Initialize(IMMU* pMMU, bool isFromTest);

public:
    bool Initialize();
    bool LoadROM(const char* path);
    void StepFrame();
    void TriggerInterrupt(byte interrupt);
    byte* GetCurrentFrame();
    void SetInput(byte input, byte buttons);

private:
    void Step();

    static byte GetHighByte(ushort dest);
    static byte GetLowByte(ushort dest);

    byte* GetByteRegister(byte val);
    ushort* GetUShortRegister(byte val, bool useAF);

    void SetHighByte(ushort* dest, byte val);
    void SetLowByte(ushort* dest, byte val);

    void SetFlag(byte flag);
    void ClearFlag(byte flag);
    bool IsFlagSet(byte flag);
    bool LookupAndCheckFlag(byte val);

    void PushByteToSP(byte val);
    void PushUShortToSP(ushort val);
    ushort PopUShort();
    byte PopByte();
    byte ReadBytePC();
    ushort ReadUShortPC();

    void HALT();

    // TODO: Organize the following...
    // Z80 Instruction Set
    void NOP(const byte& opCode);             // 0x00
    void LDrn(const byte& opCode);
    void LDrR(const byte& opCode);
    void LDrrnn(const byte& opCode);
    void INCr(const byte& opCode);
    void INCrr(const byte& opCode);
    void XORr(const byte& opCode);
    void PUSHrr(const byte& opCode);
    void POPrr(const byte& opCode);
    void DECr(const byte& opCode);

    void RLA(const byte& opCode);             // 0x17
    void LDA_DE_(const byte& opCode);         // 0x1A
    void JRNZe(const byte& opCode);           // 0x20
    void LDI_HL_A(const byte& opCode);        // 0x22
    void LDD_HL_A(const byte& opCode);        // 0x32
    void LD_HL_A(const byte& opCode);         // 0x77
    void RET(const byte& opCode);             // 0xC9
    void CALLnn(const byte& opCode);          // 0xCD
    void LD_0xFF00n_A(const byte& opCode);    // 0xE0
    void LD_0xFF00C_A(const byte& opCode);    // 0xE2

    // Z80 Instruction Set - CB
    void RLr(const byte& opCode);
    void RL_HL_(const byte& opCode);
    void BITbr(const byte& opCode);
    void BITb_HL_(const byte& opCode);
    void RESbr(const byte& opCode);
    void RESb_HL_(const byte& opCode);
    void SETbr(const byte& opCode);
    void SETb_HL_(const byte& opCode);
    void SWAPr(const byte& opCode);
    void SWAP_HL_(const byte& opCode);

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
    byte* m_ByteRegisterMap[0x07 + 1];
    ushort* m_UShortRegisterMap[0x03 + 1];

    // OpCode Function Map
    typedef void(CPU::*opCodeFunction)(const byte& opCode);
    opCodeFunction m_operationMap[0xFF + 1];
    opCodeFunction m_operationMapCB[0xFF + 1];
};
