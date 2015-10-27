#include "PCH.hpp"
#include "Joypad.hpp"

Joypad::Joypad()
{
    Logger::Log("Joypad created.");
}

Joypad::~Joypad()
{
    Logger::Log("Joypad destroyed.");
}

// IMemoryUnit
byte Joypad::ReadByte(const ushort& address)
{
    return m_memory[address];
}

bool Joypad::WriteByte(const ushort& address, const byte val)
{
    m_memory[address] = val;
    return true;
}
