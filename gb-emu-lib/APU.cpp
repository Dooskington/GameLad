#include "PCH.hpp"
#include "APU.hpp"

APU::APU()
{
    Logger::Log("APU created.");
}

APU::~APU()
{
    Logger::Log("APU destroyed.");
}

// IMemoryUnit
byte APU::ReadByte(const ushort& address)
{
    return m_memory[address];
}

bool APU::WriteByte(const ushort& address, const byte val)
{
    m_memory[address] = val;
    return true;
}
