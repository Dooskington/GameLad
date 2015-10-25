#include "PCH.hpp"
#include "GPU.hpp"

GPU::GPU()
{
    Logger::Log("GPU created.");
}

GPU::~GPU()
{
    Logger::Log("GPU destroyed.");
}

// IMemoryUnit
byte GPU::ReadByte(const ushort& address)
{
    return m_memory[address];
}

bool GPU::WriteByte(const ushort& address, const byte val)
{
    m_memory[address] = val;
    return true;
}
