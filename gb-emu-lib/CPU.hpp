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
#define SubtractFlag    6
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
    void SetVSyncCallback(void(*pCallback)());

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

    byte AddByte(byte b1, byte b2);
    ushort AddUShort(ushort u1, ushort u2);

    void HandleInterrupts();

    // TODO: Organize the following...
    // Z80 Instruction Set
    void NOP(const byte& opCode);             // 0x00

    void LDrn(const byte& opCode);
    void LDrR(const byte& opCode);
    void LDrrnn(const byte& opCode);
    void INCr(const byte& opCode);
    void INCrr(const byte& opCode);
    void DECrr(const byte& opCode);
    void ORr(const byte& opCode);
    void XORr(const byte& opCode);
    void PUSHrr(const byte& opCode);
    void POPrr(const byte& opCode);
    void DECr(const byte& opCode);
    void SUBr(const byte& opCode);
    void CALLccnn(const byte& opCode);
    void LDr_HL_(const byte& opCode);
    void LD_HL_r(const byte& opCode);
    void RETcc(const byte& opCode);
    void ADDHLss(const byte& opCode);
    void JPccnn(const byte& opCode);
    void ADDAr(const byte& opCode);
    void ADCAr(const byte& opCode);
    void JRcce(const byte& opCode);
    void ANDr(const byte& opCode);
    void CPr(const byte& opCode);
    void RSTn(const byte& opCode);

    void LD_BC_A(const byte& opCode);         // 0x02
    void RLCA(const byte& opCode);            // 0x07
    void LD_nn_SP(const byte& opCode);        // 0x08
    void LDA_BC_(const byte& opCode);         // 0x0A
    void RRCA(const byte& opCode);            // 0x0F
    void STOP(const byte& opCode);            // 0x10
    void LD_DE_A(const byte& opCode);         // 0x12
    void RLA(const byte& opCode);             // 0x17
    void JRe(const byte& opCode);             // 0x18
    void LDA_DE_(const byte& opCode);         // 0x1A
    void RRA(const byte& opCode);             // 0x1F
    void LDI_HL_A(const byte& opCode);        // 0x22
    void DAA(const byte& opCode);             // 0x27
    void LDIA_HL_(const byte& opCode);        // 0x2A
    void CPL(const byte& opCode);             // 0x2F
    void LDD_HL_A(const byte& opCode);        // 0x32
    void INC_HL_(const byte& opCode);         // 0x34
    void DEC_HL_(const byte& opCode);         // 0x35
    void LD_HL_n(const byte& opCode);         // 0x36
    void SCF(const byte& opCode);             // 0x37
    void LDDA_HL_(const byte& opCode);        // 0x3A
    void CCF(const byte& opCode);             // 0x3F
    void HALT(const byte& opCode);            // 0x76
    void ADDA_HL_(const byte& opCode);        // 0x86
    void SUB_HL_(const byte& opCode);         // 0x96
    void AND_HL_(const byte& opCode);         // 0xA6
    void XOR_HL_(const byte& opCode);         // 0xAE
    void OR_HL_(const byte& opCode);          // 0xB6
    void CP_HL_(const byte& opCode);          // 0xBE
    void JPnn(const byte& opCode);            // 0xC3
    void ADDAn(const byte& opCode);           // 0xC6
    void RET(const byte& opCode);             // 0xC9
    void CALLnn(const byte& opCode);          // 0xCD
    void ADCAn(const byte& opCode);           // 0xCE
    void SUBn(const byte& opCode);            // 0xD6
    void RETI(const byte& opCode);            // 0xD9
    void SBCAn(const byte& opCode);           // 0xDE
    void LD_0xFF00n_A(const byte& opCode);    // 0xE0
    void LD_0xFF00C_A(const byte& opCode);    // 0xE2
    void ANDn(const byte& opCode);            // 0xE6
    void ADDSPdd(const byte& opCode);         // 0xE8
    void JP_HL_(const byte& opCode);          // 0xE9
    void LD_nn_A(const byte& opCode);         // 0xEA
    void XORn(const byte& opCode);            // 0xEE
    void LDA_0xFF00n_(const byte& opCode);    // 0xF0
    void LDA_0xFF00C_(const byte& opCode);    // 0xF2
    void DI(const byte& opCode);              // 0xF3
    void ORn(const byte& opCode);             // 0xF6
    void LDHLSPe(const byte& opCode);         // 0xF8
    void LDSPHL(const byte& opCode);          // 0xF9
    void LDA_nn_(const byte& opCode);         // 0xFA
    void EI(const byte& opCode);              // 0xFB
    void CPn(const byte& opCode);             // 0xFE

    // Z80 Instruction Set - CB
    void RLCr(const byte& opCode);
    void RLC_HL_(const byte& opCode);
    void RRCr(const byte& opCode);
    void RRC_HL_(const byte& opCode);
    void RLr(const byte& opCode);
    void RL_HL_(const byte& opCode);
    void RRr(const byte& opCode);
    void RR_HL_(const byte& opCode);
    void SLAr(const byte& opCode);
    void SLA_HL_(const byte& opCode);
    void SRLr(const byte& opCode);
    void SRL_HL_(const byte& opCode);
    void SRAr(const byte& opCode);
    void SRA_HL_(const byte& opCode);
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
    byte m_IFWhenHalted;

    // Registers
    ushort m_AF; // Accumulator & flags
    ushort m_BC; // General purpose
    ushort m_DE; // General purpose
    ushort m_HL; // General purpose
    ushort m_SP; // Stack pointer
    ushort m_PC; // Program counter
    byte* m_ByteRegisterMap[0x07 + 1];
    ushort* m_UShortRegisterMap[0x03 + 1];

    // Interrupts
    byte m_IME; // Interrupt master enable
    // TODO: Implement interrupts

    // OpCode Function Map
    typedef void(CPU::*opCodeFunction)(const byte& opCode);
    opCodeFunction m_operationMap[0xFF + 1];
    opCodeFunction m_operationMapCB[0xFF + 1];
};
