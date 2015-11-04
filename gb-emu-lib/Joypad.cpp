#include "PCH.hpp"
#include "Joypad.hpp"

Joypad::Joypad() :
    m_SelectValues(0x00),
    m_InputValues(0x00),
    m_ButtonValues(0x00)
{
    Logger::Log("Joypad created.");
}

Joypad::~Joypad()
{
    Logger::Log("Joypad destroyed.");
}

void Joypad::SetInput(byte input, byte buttons)
{
    m_InputValues = input;
    m_ButtonValues = buttons;
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
    default:
        Logger::Log("Joypad::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
