#include "PCH.hpp"
#include "Timer.hpp"

Timer::Timer()
{
    Logger::Log("Timer created.");
}

Timer::~Timer()
{
    Logger::Log("Timer destroyed.");
}

// IMemoryUnit
byte Timer::ReadByte(const ushort& address)
{
    return m_memory[address];
}

bool Timer::WriteByte(const ushort& address, const byte val)
{
    m_memory[address] = val;
    return true;
}
