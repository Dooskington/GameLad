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
    byte inputChanges = !ISBITSET(m_SelectValues, 4) ? (m_InputValues ^ input) : 0x00;
    byte buttonChanges = !ISBITSET(m_SelectValues, 5) ? (m_ButtonValues ^ buttons) : 0x00;

    m_InputValues = input;
    m_ButtonValues = buttons;

    // If either the input or buttons change and they were requested, trigger interrupt
    if ((m_CPU != nullptr) && ((inputChanges > 0x00) || (buttonChanges > 0x00)))
    {
        m_CPU->TriggerInterrupt(INT60);
    }
}

// IMemoryUnit
byte Joypad::ReadByte(const ushort& address)
{
    byte input = 0x00;

    switch (address)
    {
    case JoypadAddress:
        
        // Input included when 4th bit is LOW (not set)
        if (!ISBITSET(m_SelectValues, 4))
        {
            input |= m_InputValues;
        }

        // Buttons included when 5th bit is LOW (not set)
        if (!ISBITSET(m_SelectValues, 5))
        {
            input |= m_ButtonValues;
        }

        return ((m_SelectValues | 0x0F) ^ input) & 0x3F;
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
        // Only save BIT 5 and 4, the rest are unused/read only

        m_SelectValues = (val & 0x30);
        return true;
    default:
        Logger::Log("Joypad::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
