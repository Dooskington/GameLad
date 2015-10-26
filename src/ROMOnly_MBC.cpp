#include "PCH.hpp"
#include "ROMOnly_MBC.hpp"

ROMOnly_MBC::ROMOnly_MBC(byte* data)
{
    m_ROM = data;

    Logger::Log("ROMOnly_MBC created.");
}

ROMOnly_MBC::~ROMOnly_MBC()
{
    Logger::Log("ROMOnly_MBC destroyed.");
}

// IMemoryUnit
byte ROMOnly_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x7FFF)
    {
        return m_ROM[address];
    }

    Logger::Log("ROMOnly_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool ROMOnly_MBC::WriteByte(const ushort& address, const byte val)
{
    Logger::Log("ROMOnly_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
