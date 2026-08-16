#include "pch.hpp"
#include "Joypad.hpp"

Joypad::Joypad(ICPU* pCPU) :
    m_CPU(pCPU),
    m_SelectValues(0x00),
    m_InputValues(0x00),
    m_ButtonValues(0x00)
{
}

Joypad::~Joypad()
{
}

void Joypad::SetInput(byte input, byte buttons)
{
    byte previousLines = ReadInputLines();

    m_InputValues = input & 0x0F;
    m_ButtonValues = buttons & 0x0F;

    TriggerFallingEdgeInterrupt(previousLines);
}

byte Joypad::ReadInputLines() const
{
    byte pressed = 0x00;

    if (!ISBITSET(m_SelectValues, 4))
    {
        pressed |= m_InputValues;
    }

    if (!ISBITSET(m_SelectValues, 5))
    {
        pressed |= m_ButtonValues;
    }

    return static_cast<byte>((~pressed) & 0x0F);
}

void Joypad::TriggerFallingEdgeInterrupt(byte previousLines)
{
    byte fallingEdges = previousLines & static_cast<byte>(~ReadInputLines());
    if ((m_CPU != nullptr) && (fallingEdges != 0x00))
    {
        m_CPU->TriggerInterrupt(INT60);
    }
}

// IMemoryUnit
byte Joypad::ReadByte(const ushort& address)
{
    switch (address)
    {
    case JoypadAddress:
        return 0xC0 | m_SelectValues | ReadInputLines();
    default:
        Logger::Log("Joypad::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool Joypad::WriteByte(const ushort& address, const byte val)
{
    switch (address)
    {
    case JoypadAddress:
    {
        byte previousLines = ReadInputLines();
        m_SelectValues = (val & 0x30);
        TriggerFallingEdgeInterrupt(previousLines);
        return true;
    }
    default:
        Logger::Log("Joypad::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
