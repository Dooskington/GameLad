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
    bool LoadROM(const char* bootROMPath, const char* cartridgePath);
    int Step();
    void TriggerInterrupt(byte interrupt);
    byte* GetCurrentFrame();
    void SetInput(byte input, byte buttons);
    void SetVSyncCallback(void(*pCallback)());

private:
    static byte GetHighByte(ushort dest);
    static byte GetLowByte(ushort dest);

    byte* GetByteRegister(byte val);
    ushort* GetUShortRegister(byte val, bool useAF);

    void SetHighByte(ushort* dest, byte val);
    void SetLowByte(ushort* dest, byte val);

    void SetFlag(byte flag);
    void ClearFlag(byte flag);
    bool IsFlagSet(byte flag);

    void PushByteToSP(byte val);
    void PushUShortToSP(ushort val);
    ushort PopUShort();
    byte PopByte();
    byte ReadBytePC();
    ushort ReadUShortPC();

    byte AddByte(byte b1, byte b2);
    ushort AddUShort(ushort u1, ushort u2);
    void ADC(byte val);
    void SBC(byte val);

    void HandleInterrupts();

    // TODO: Organize the following...
    // Z80 Instruction Set
    unsigned long NOP(const byte& opCode);             // 0x00

    unsigned long LDrn(const byte& opCode);
    unsigned long LDrR(const byte& opCode);
    unsigned long LDrrnn(const byte& opCode);
    unsigned long INCr(const byte& opCode);
    unsigned long INCrr(const byte& opCode);
    unsigned long DECrr(const byte& opCode);
    unsigned long ORr(const byte& opCode);
    unsigned long XORr(const byte& opCode);
    unsigned long PUSHrr(const byte& opCode);
    unsigned long POPrr(const byte& opCode);
    unsigned long DECr(const byte& opCode);
    unsigned long SUBr(const byte& opCode);
    unsigned long SBCAr(const byte& opCode);
    unsigned long CALLccnn(const byte& opCode);
    unsigned long LDr_HL_(const byte& opCode);
    unsigned long LD_HL_r(const byte& opCode);
    unsigned long RETcc(const byte& opCode);
    unsigned long ADDHLss(const byte& opCode);
    unsigned long JPccnn(const byte& opCode);
    unsigned long ADDAr(const byte& opCode);
    unsigned long ADCAr(const byte& opCode);
    unsigned long JRcce(const byte& opCode);
    unsigned long ANDr(const byte& opCode);
    unsigned long CPr(const byte& opCode);
    unsigned long RSTn(const byte& opCode);

    unsigned long LD_BC_A(const byte& opCode);         // 0x02
    unsigned long RLCA(const byte& opCode);            // 0x07
    unsigned long LD_nn_SP(const byte& opCode);        // 0x08
    unsigned long LDA_BC_(const byte& opCode);         // 0x0A
    unsigned long RRCA(const byte& opCode);            // 0x0F
    unsigned long STOP(const byte& opCode);            // 0x10
    unsigned long LD_DE_A(const byte& opCode);         // 0x12
    unsigned long RLA(const byte& opCode);             // 0x17
    unsigned long JRe(const byte& opCode);             // 0x18
    unsigned long LDA_DE_(const byte& opCode);         // 0x1A
    unsigned long RRA(const byte& opCode);             // 0x1F
    unsigned long LDI_HL_A(const byte& opCode);        // 0x22
    unsigned long DAA(const byte& opCode);             // 0x27
    unsigned long LDIA_HL_(const byte& opCode);        // 0x2A
    unsigned long CPL(const byte& opCode);             // 0x2F
    unsigned long LDD_HL_A(const byte& opCode);        // 0x32
    unsigned long INC_HL_(const byte& opCode);         // 0x34
    unsigned long DEC_HL_(const byte& opCode);         // 0x35
    unsigned long LD_HL_n(const byte& opCode);         // 0x36
    unsigned long SCF(const byte& opCode);             // 0x37
    unsigned long LDDA_HL_(const byte& opCode);        // 0x3A
    unsigned long CCF(const byte& opCode);             // 0x3F
    unsigned long HALT(const byte& opCode);            // 0x76
    unsigned long ADDA_HL_(const byte& opCode);        // 0x86
    unsigned long ADCA_HL_(const byte& opCode);        // 0x8E
    unsigned long SUB_HL_(const byte& opCode);         // 0x96
    unsigned long SBCA_HL_(const byte& opCode);        // 0x9E
    unsigned long AND_HL_(const byte& opCode);         // 0xA6
    unsigned long XOR_HL_(const byte& opCode);         // 0xAE
    unsigned long OR_HL_(const byte& opCode);          // 0xB6
    unsigned long CP_HL_(const byte& opCode);          // 0xBE
    unsigned long JPnn(const byte& opCode);            // 0xC3
    unsigned long ADDAn(const byte& opCode);           // 0xC6
    unsigned long RET(const byte& opCode);             // 0xC9
    unsigned long CALLnn(const byte& opCode);          // 0xCD
    unsigned long ADCAn(const byte& opCode);           // 0xCE
    unsigned long SUBn(const byte& opCode);            // 0xD6
    unsigned long RETI(const byte& opCode);            // 0xD9
    unsigned long SBCAn(const byte& opCode);           // 0xDE
    unsigned long LD_0xFF00n_A(const byte& opCode);    // 0xE0
    unsigned long LD_0xFF00C_A(const byte& opCode);    // 0xE2
    unsigned long ANDn(const byte& opCode);            // 0xE6
    unsigned long ADDSPdd(const byte& opCode);         // 0xE8
    unsigned long JP_HL_(const byte& opCode);          // 0xE9
    unsigned long LD_nn_A(const byte& opCode);         // 0xEA
    unsigned long XORn(const byte& opCode);            // 0xEE
    unsigned long LDA_0xFF00n_(const byte& opCode);    // 0xF0
    unsigned long LDA_0xFF00C_(const byte& opCode);    // 0xF2
    unsigned long DI(const byte& opCode);              // 0xF3
    unsigned long ORn(const byte& opCode);             // 0xF6
    unsigned long LDHLSPe(const byte& opCode);         // 0xF8
    unsigned long LDSPHL(const byte& opCode);          // 0xF9
    unsigned long LDA_nn_(const byte& opCode);         // 0xFA
    unsigned long EI(const byte& opCode);              // 0xFB
    unsigned long CPn(const byte& opCode);             // 0xFE

    // Z80 Instruction Set - CB
    unsigned long RLCr(const byte& opCode);
    unsigned long RLC_HL_(const byte& opCode);
    unsigned long RRCr(const byte& opCode);
    unsigned long RRC_HL_(const byte& opCode);
    unsigned long RLr(const byte& opCode);
    unsigned long RL_HL_(const byte& opCode);
    unsigned long RRr(const byte& opCode);
    unsigned long RR_HL_(const byte& opCode);
    unsigned long SLAr(const byte& opCode);
    unsigned long SLA_HL_(const byte& opCode);
    unsigned long SRLr(const byte& opCode);
    unsigned long SRL_HL_(const byte& opCode);
    unsigned long SRAr(const byte& opCode);
    unsigned long SRA_HL_(const byte& opCode);
    unsigned long BITbr(const byte& opCode);
    unsigned long BITb_HL_(const byte& opCode);
    unsigned long RESbr(const byte& opCode);
    unsigned long RESb_HL_(const byte& opCode);
    unsigned long SETbr(const byte& opCode);
    unsigned long SETb_HL_(const byte& opCode);
    unsigned long SWAPr(const byte& opCode);
    unsigned long SWAP_HL_(const byte& opCode);

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

    // Interrupts
    byte m_IME; // Interrupt master enable

    // OpCode Function Map
    typedef unsigned long(CPU::*opCodeFunction)(const byte& opCode);
    opCodeFunction m_operationMap[0xFF + 1];
    opCodeFunction m_operationMapCB[0xFF + 1];
};
