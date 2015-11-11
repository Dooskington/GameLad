#include "PCH.hpp"
#include "CPU.hpp"

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
    Logger::Log("CPU created.");
}

CPU::~CPU()
{
    m_timer.reset();
    m_serial.reset();
    m_joypad.reset();
    m_APU.reset();
    m_GPU.reset();
    m_cartridge.reset();
    m_MMU.reset();

    Logger::Log("CPU destroyed.");
}

bool CPU::Initialize(IMMU* pMMU, bool isFromTest)
{
    // Initialize the operationMap
    m_operationMap[0x00] = &CPU::NOP;
    m_operationMap[0x06] = &CPU::LDBe;
    m_operationMap[0x0C] = &CPU::INCC;
    m_operationMap[0x0E] = &CPU::LDCe;
    m_operationMap[0x11] = &CPU::LDDEnn;
    m_operationMap[0x1A] = &CPU::LDA_DE_;
    m_operationMap[0x20] = &CPU::JRNZe;
    m_operationMap[0x21] = &CPU::LDHLnn;
    m_operationMap[0x31] = &CPU::LDSPnn;
    m_operationMap[0x32] = &CPU::LDD_HL_A;
    m_operationMap[0x3E] = &CPU::LDAe;
    m_operationMap[0x4F] = &CPU::LDCA;
    m_operationMap[0x77] = &CPU::LD_HL_A;
    m_operationMap[0xAF] = &CPU::XORA;
    m_operationMap[0xE0] = &CPU::LD_0xFF00n_A;
    m_operationMap[0xE2] = &CPU::LD_0xFF00C_A;
    m_operationMap[0xCD] = &CPU::CALLnn;

    // Initialize the operationMapCB
    m_operationMapCB[0x7C] = &CPU::BIT7h;

    // Create the MMU
    m_MMU = std::unique_ptr<IMMU>(pMMU);

    if (!isFromTest)
    {
        // Create the Cartridge
        m_cartridge = std::make_unique<Cartridge>();

        // Create the GPU
        m_GPU = std::unique_ptr<GPU>(new GPU(pMMU, this));

        // Create the APU
        m_APU = std::make_unique<APU>();

        // Create the Joypad
        m_joypad = std::make_unique<Joypad>(this);

        // Create the Serial
        m_serial = std::make_unique<Serial>();

        // Create the Timer
        m_timer = std::unique_ptr<Timer>(new Timer(this));

        m_MMU->RegisterMemoryUnit(0x0000, 0x7FFF, m_cartridge.get());
        m_MMU->RegisterMemoryUnit(0x8000, 0x9FFF, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xA000, 0xBFFF, m_cartridge.get());
        m_MMU->RegisterMemoryUnit(0xFE00, 0xFE9F, m_GPU.get());
        // 0xFEA0-0xFEFF - Unusable
        m_MMU->RegisterMemoryUnit(0xFF00, 0xFF00, m_joypad.get());
        m_MMU->RegisterMemoryUnit(0xFF01, 0xFF02, m_serial.get());
        m_MMU->RegisterMemoryUnit(0xFF04, 0xFF07, m_timer.get());
        m_MMU->RegisterMemoryUnit(0xFF10, 0xFF3F, m_APU.get());
        m_MMU->RegisterMemoryUnit(0xFF40, 0xFF4C, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xFF4E, 0xFF55, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xFF57, 0xFF6B, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xFF6D, 0xFF6F, m_GPU.get());
    }

    return true;
}

bool CPU::Initialize()
{
    return Initialize(new MMU(), false);
}

bool CPU::LoadROM(std::string path)
{
    return m_cartridge->LoadROM(path);
}

void CPU::StepFrame()
{
    while (m_cycles < CyclesPerFrame)
    {
        Step();
    }

    // Reset the cycles. If we went over our max cycles, the next frame will start a
    // few cycles ahead.
    m_cycles -= CyclesPerFrame;
}

void CPU::TriggerInterrupt(byte interrupt)
{
    // TODO: Process interrupts here!
}

byte* CPU::GetCurrentFrame()
{
    return m_GPU->GetCurrentFrame();
}

void CPU::SetInput(byte input, byte buttons)
{
    m_joypad->SetInput(input, buttons);
}

void CPU::Step()
{
    unsigned long preCycles = m_cycles;

    if (m_isHalted)
    {
        NOP();
    }
    else
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
        }
    }

    unsigned long elapsedCycles = m_cycles - preCycles;

    if (m_GPU != nullptr)
    {
        // Step GPU by # of elapsed cycles
        m_GPU->Step(elapsedCycles);
    }

    if (m_timer != nullptr)
    {
        // Step the timer by the # of elapsed cycles
        m_timer->Step(elapsedCycles);
    }

    if (m_APU != nullptr)
    {
        // Step the audio processing unit by the # of elapsed cycles
        m_APU->Step(elapsedCycles);
    }
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
    SetLowByte(&m_AF, SETBIT(GetLowByte(m_AF), flag) & 0xF0);
}

void CPU::ClearFlag(byte flag)
{
    // This shifts the bit to the left to where the flag is
    // Then it inverts all of the bits
    // Then ANDs it with the Flag register.
    // Finally it filters out the lower 4 bits, as they aren't used on the Gameboy
    SetLowByte(&m_AF, CLEARBIT(GetLowByte(m_AF), flag) & 0xF0);
}

bool CPU::IsFlagSet(byte flag)
{
    return ISBITSET(GetLowByte(m_AF), flag);
}

void CPU::PushByteToSP(byte val)
{
    m_SP--;
    m_MMU->WriteByte(m_SP, val);
}

void CPU::PushUShortToSP(ushort val)
{
    PushByteToSP(GetHighByte(val));
    PushByteToSP(GetLowByte(val));
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

// 0x06 (LD B, e)
void CPU::LDBe()
{
    m_PC += 1; // Look at e
    byte e = m_MMU->ReadByte(m_PC); // Read e
    SetHighByte(&m_BC, e); // Set B to e
    m_PC += 1; // Move onto the next instruction
    m_cycles += 8;

    // No flags affected
}

// 0x0C (INC C)
void CPU::INCC()
{
    m_PC += 1;

    byte C = GetLowByte(m_BC);
    bool isBit3Before = ISBITSET(C, 3);
    C += 1;
    bool isBit3After = ISBITSET(C, 3);
    SetLowByte(&m_BC, C);
    m_cycles += 4;

    // Flags affected: z0h- (znhc)
    // Affects Z, clears N, affects H
    if (C == 0x00)
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
        m_cycles += 8;
    }
    else
    {
        m_PC += 1;
        sbyte arg = static_cast<sbyte>(m_MMU->ReadByte(m_PC));
        m_PC += 1;
        m_PC += arg;

        m_cycles += 12;
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

// 0x4F (LD C, A)
void CPU::LDCA()
{
    m_PC += 1;
    SetLowByte(&m_BC, GetHighByte(m_AF));
    m_cycles += 4;
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

// 0xCD (CALL nn)
void CPU::CALLnn()
{
    // This instruction pushes the PC to the SP, then sets the PC to the target address(nn).
    m_PC += 1; // Look at the first byte of nn
    ushort nn = m_MMU->ReadUShort(m_PC); // Read nn
    m_PC += 2; // Move onto the next instruction
    PushUShortToSP(m_PC); // Push PC to SP
    m_PC = nn; // Set the PC to the target address

    m_cycles += 24;

    // No flags affected
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
    if (!ISBITSET(GetHighByte(m_HL), 7))
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
