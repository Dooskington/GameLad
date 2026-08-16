#include "pch.hpp"
#include "Serial.hpp"

// FF01 - SB - Serial transfer data (R/W)
// FF02 - SC - Serial Transfer Control (R/W)
#define SerialTransferData 0xFF01
#define SerialTransferControl 0xFF02

#define SerialStartBit 0x80
#define SerialFastClock 0x02
#define SerialInternalClock 0x01
#define SerialControlMask (SerialStartBit | SerialInternalClock)
#define SerialControlMaskCGB (SerialStartBit | SerialFastClock | SerialInternalClock)
#define SerialUnusedBits 0x7E
#define SerialUnusedBitsCGB 0x7C

Serial::Serial(ICPU* pCPU) :
    m_CPU(pCPU),
    m_mode(GameBoyMode::DMG),
    m_Data(0x00),
    m_Control(0x00),
    m_BitsTransferred(0x00),
    m_MasterClock(false),
    m_ClockCycles(0),
    m_InterruptSuppressCycles(0)
{
}

Serial::~Serial()
{
}

byte Serial::ControlMask() const
{
    return IsCGBFeatureMode(m_mode) ? SerialControlMaskCGB : SerialControlMask;
}

unsigned long Serial::ClockMask() const
{
    if (IsCGBFeatureMode(m_mode) && ISBITSET(m_Control, 1))
    {
        return 1UL << 2;
    }

    return 1UL << 7;
}

void Serial::PreBoot()
{
    // Match the divider phase installed when the boot ROM is skipped.
    m_ClockCycles = IsCGBHardware(m_mode) ? 0x267B : 0xABCB;
}

void Serial::Step(unsigned long cycles)
{
    while (cycles-- != 0)
    {
        const unsigned long clockMask = ClockMask();
        const bool oldClock = (m_ClockCycles & clockMask) != 0;
        m_ClockCycles++;
        const bool newClock = (m_ClockCycles & clockMask) != 0;
        if (oldClock && !newClock)
        {
            ClockMasterEdge();
        }

        if (m_InterruptSuppressCycles != 0)
        {
            m_InterruptSuppressCycles--;
        }
    }
}

void Serial::ResetDivider()
{
    if ((m_ClockCycles & ClockMask()) != 0)
    {
        ClockMasterEdge();
    }

    m_ClockCycles = 0;
}

void Serial::AcknowledgeInterrupt(bool doubleSpeed)
{
    m_InterruptSuppressCycles =
        IsCGBHardware(m_mode) ? static_cast<byte>(doubleSpeed ? 8 : 5) : 3;
}

void Serial::ClockMasterEdge()
{
    m_MasterClock = !m_MasterClock;
    if (!m_MasterClock && (m_Control & SerialControlMask) == SerialControlMask)
    {
        ShiftBit(true);
    }
}

bool Serial::IsWaitingForExternalClock() const
{
    return ISBITSET(m_Control, 7) && !ISBITSET(m_Control, 0);
}

bool Serial::ClockExternalBit(bool incomingBit, bool& outgoingBit)
{
    if (!IsWaitingForExternalClock())
    {
        return false;
    }

    outgoingBit = ISBITSET(m_Data, 7);
    ShiftBit(incomingBit);
    return true;
}

void Serial::ShiftBit(bool incomingBit)
{
    m_Data = static_cast<byte>((m_Data << 1) | (incomingBit ? 0x01 : 0x00));
    m_BitsTransferred++;

    if (m_BitsTransferred == 8)
    {
        CompleteTransfer();
    }
}

void Serial::CompleteTransfer()
{
    m_Control &= static_cast<byte>(~SerialStartBit);
    m_BitsTransferred = 0x00;

    if ((m_CPU != nullptr) && (m_InterruptSuppressCycles == 0))
    {
        m_CPU->TriggerInterrupt(INT58);
    }
}

// IMemoryUnit
byte Serial::ReadByte(const ushort& address)
{
    switch (address)
    {
    case SerialTransferData:
        return m_Data;
    case SerialTransferControl:
        return m_Control | (IsCGBFeatureMode(m_mode) ? SerialUnusedBitsCGB : SerialUnusedBits);
    default:
        Logger::Log("Serial::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool Serial::WriteByte(const ushort& address, const byte val)
{
    switch (address)
    {
    case SerialTransferData:
        m_Data = val;
        return true;
    case SerialTransferControl:
    {
        m_BitsTransferred = 0x00;

        // SC writes force the master flip-flop low. A falling edge can clock
        // the old transfer before the new control value takes effect.
        if (m_MasterClock)
        {
            ClockMasterEdge();
        }

        m_Control = val & ControlMask();
        if ((m_Control & SerialControlMask) == SerialControlMask)
        {
            // Test ROMs conventionally print through SB/SC; keep this as diagnostics only.
            Logger::LogCharacter(m_Data);
        }

        return true;
    }
    default:
        Logger::Log("Serial::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
