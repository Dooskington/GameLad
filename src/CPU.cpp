#include "PCH.hpp"
#include "CPU.hpp"
#include "Logger.hpp"

// The number of CPU cycles per frame
const unsigned int CyclesPerFrame = 70244;

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
    Logger::Log("CPU created.");

    // Create the MMU
    m_MMU = std::make_unique<MMU>();

    // Create the Cartridge
    m_cartridge = std::make_unique<Cartridge>();

    m_MMU->RegisterMemoryUnit(0x0000, 0x7FFF, m_cartridge.get());

    // Initialize the operationMap
    m_operationMap[0x00] = &CPU::NOP;
    m_operationMap[0x0C] = &CPU::INCC;
    m_operationMap[0x0E] = &CPU::LDCe;
    m_operationMap[0x11] = &CPU::LDDEnn;
    m_operationMap[0x1A] = &CPU::LDA_DE_;
    m_operationMap[0x20] = &CPU::JRNZe;
    m_operationMap[0x21] = &CPU::LDHLnn;
    m_operationMap[0x31] = &CPU::LDSPnn;
    m_operationMap[0x32] = &CPU::LDD_HL_A;
    m_operationMap[0x3E] = &CPU::LDAe;
    m_operationMap[0x77] = &CPU::LD_HL_A;
    m_operationMap[0xAF] = &CPU::XORA;
    m_operationMap[0xE0] = &CPU::LD_0xFF00n_A;
    m_operationMap[0xE2] = &CPU::LD_0xFF00C_A;

    // Initialize the operationMapCB
    m_operationMapCB[0x7C] = &CPU::BIT7h;
}

CPU::~CPU()
{
    m_cartridge.reset();
    m_MMU.reset();

    Logger::Log("CPU destroyed.");
}

bool CPU::Initialize()
{
    return m_MMU->Initialize();
}

bool CPU::LoadROM(std::string path)
{
    return m_cartridge->LoadROM(path);
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
        opCodeFunction instruction; // Execute the correct function for each OpCode

        if (opCode == 0xCB)
        {
            m_PC += 1;
            opCode = m_MMU->ReadByte(m_PC);
            instruction = m_operationMapCB[opCode];
        }
        else
        {
            instruction = m_operationMap[opCode];
        }

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
    *dest = (val << 8) | low;
}

void CPU::SetLowByte(ushort* dest, byte val)
{
    byte high = GetHighByte(*dest);
    *dest = (high << 8) | val;
}

void CPU::SetFlag(byte flag)
{
    // This shifts the bit to the left to where the flag is
    // Then ORs it with the Flag register.
    // Finally it filters out the lower 4 bits, as they aren't used on the Gameboy
    SetLowByte(&m_AF, (GetLowByte(m_AF) | (1 << flag)) & 0xF0);
}

void CPU::ClearFlag(byte flag)
{
    // This shifts the bit to the left to where the flag is
    // Then it inverts all of the bits
    // Then ANDs it with the Flag register.
    // Finally it filters out the lower 4 bits, as they aren't used on the Gameboy
    SetLowByte(&m_AF, (GetLowByte(m_AF) & ~(1 << flag)) & 0xF0);
}

bool CPU::IsFlagSet(byte flag)
{
    return IsBitSet(GetLowByte(m_AF), flag);
}

bool CPU::IsBitSet(byte val, byte bit)
{
    // This shifts the byte val to the right to get the bit we are interested
    // in to the "1" position. Then we AND it with 1 and compare to 1.
    // If that bit was set, it'll return true, otherwise false.
    return ((val >> bit) & 0x01) == 0x01;
}

void CPU::HALT()
{
    m_isHalted = true;
    Logger::Log("!!!! HALTED !!!!");
}

/*
    CPU INSTRUCTION MAP
*/

// 0x00 (NOP)
void CPU::NOP()
{
    m_PC += 1;
    m_cycles += 4;

    // No flags affected
}

// 0x0C (INC C)
void CPU::INCC()
{
    m_PC += 1;

    byte C = GetLowByte(m_BC);
    bool isBit3Before = IsBitSet(C, 3);
    C += 1;
    bool isBit3After = IsBitSet(C, 3);
    SetLowByte(&m_BC, C);
    m_cycles += 4;

    // Flags affected: z0h- (znhc)
    // Affects Z, clears N, affects H
    if (GetHighByte(m_AF) == 0)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(AddFlag);

    if (isBit3Before && !isBit3After)
    {
        SetFlag(HalfCarryFlag);
    }
    else
    {
        ClearFlag(HalfCarryFlag);
    }
}

// 0x0E (LD C, e)
void CPU::LDCe()
{
    m_PC += 1; // Look at e
    byte e = m_MMU->ReadByte(m_PC); // Read e
    SetLowByte(&m_BC, e); // Set C to e
    m_PC += 1; // Move onto the next instruction
    m_cycles += 8;

    // No flags affected
}

// 0x11 (LD DE, nn)
void CPU::LDDEnn()
{
    m_PC += 1; // Look at the first byte of nn
    ushort nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_DE = nn;
    m_PC += 2; // Move onto the next instruction
    m_cycles += 8;

    // No flags affected
}

// 0x1A (LD A, (DE))
void CPU::LDA_DE_()
{
    // loads the value stored at the address pointed to by DE 
    // (currently 0x0104) and stores in the A register
    m_PC += 1;
    byte val = m_MMU->ReadByte(m_DE);
    SetHighByte(&m_AF, val);
    m_cycles += 8;

    // No flags affected
}

// 0x20 0xFB (JR NZ, e)
void CPU::JRNZe()
{
    if (IsFlagSet(ZeroFlag))
    {
        m_PC += 2;
        m_cycles += 12;
    }
    else
    {
        m_PC += 1;
        sbyte arg = static_cast<sbyte>(m_MMU->ReadByte(m_PC));
        m_PC += 1;
        m_PC += arg;
    }

    // No flags affected
}

// 0x21 (LD HL, nn)
void CPU::LDHLnn()
{
    m_PC += 1; // Look at the first byte of nn
    ushort nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_HL = nn;
    m_PC += 2; // Move onto the next instruction
    m_cycles += 8;

    // No flags affected
}

// 0x31 (LD SP, nn)
void CPU::LDSPnn()
{
    m_PC += 1; // Look at the first byte of nn
    ushort nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_SP = nn;
    m_PC += 2; // Move onto the next instruction
    m_cycles += 8;

    // No flags affected
}

// 0x32 (LDD (HL), A)
void CPU::LDD_HL_A()
{
    m_PC += 1;

    if (!m_MMU->WriteByte(m_HL, GetHighByte(m_AF))) // Load A into the address pointed at by HL.
    {
        HALT();
        return;
    }

    m_HL--;
    m_cycles += 8;

    // No flags affected
}

// 0x3E (LD A, e)
void CPU::LDAe()
{
    m_PC += 1; // Look at e
    byte e = m_MMU->ReadByte(m_PC); // Read e
    SetHighByte(&m_AF, e); // Set A to e
    m_PC += 1; // Move onto the next instruction
    m_cycles += 8;

    // No flags affected
}

// 0x77 (LD (HL), A)
// Identical to 0x32, except does not decrement
void CPU::LD_HL_A()
{
    m_PC += 1;

    if (!m_MMU->WriteByte(m_HL, GetHighByte(m_AF))) // Load A into the address pointed at by HL.
    {
        HALT();
    }

    m_cycles += 8;

    // No flags affected
}

// 0xAF (XOR A)
void CPU::XORA()
{
    m_PC += 1;
    SetHighByte(&m_AF, GetHighByte(m_AF) ^ GetHighByte(m_AF));
    m_cycles += 4;

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(AddFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);
}

// 0xE0 (LD(0xFF00 + n), A)
void CPU::LD_0xFF00n_A()
{
    m_PC += 1; // Look at n
    byte n = m_MMU->ReadByte(m_PC); // Read n

    if (!m_MMU->WriteByte(0xFF00 + n, GetHighByte(m_AF))) // Load A into 0xFF00 + n
    {
        HALT();
    }

    m_PC += 1;
    m_cycles += 8;

    // No flags affected
}

// 0xE2 (LD(0xFF00 + C), A)
void CPU::LD_0xFF00C_A()
{
    m_PC += 1;

    if (!m_MMU->WriteByte(0xFF00 + GetLowByte(m_BC), GetHighByte(m_AF))) // Load A into 0xFF00 + C
    {
        HALT();
    }

    m_cycles += 8;

    // No flags affected
}

/*
    CPU 0xCB INSTRUCTION MAP
*/

// 0x7C (BIT 7, h)
void CPU::BIT7h()
{
    m_PC++;
    m_cycles += 8;

    // Test bit 7 in H
    if (!IsBitSet(GetHighByte(m_HL), 7))
    {
        // Z is set if specified bit is 0
        SetFlag(ZeroFlag);
    }
    else
    {
        // Reset otherwise
        ClearFlag(ZeroFlag);
    }

    SetFlag(HalfCarryFlag); // H is set
    ClearFlag(AddFlag); // N is reset
}
