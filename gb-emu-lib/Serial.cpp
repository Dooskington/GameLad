#include "PCH.hpp"
#include "Serial.hpp"

Serial::Serial()
{
    Logger::Log("Serial created.");
}

Serial::~Serial()
{
    Logger::Log("Serial destroyed.");
}

// IMemoryUnit
byte Serial::ReadByte(const ushort& address)
{
    return m_memory[address];
}

bool Serial::WriteByte(const ushort& address, const byte val)
{
    m_memory[address] = val;
    return true;
}
