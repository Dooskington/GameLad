#include "PCH.hpp"
#include "MBC1_MBC.hpp"

MBC1_MBC::MBC1_MBC(byte* data)
{
    m_ROM = data;

    Logger::Log("MBC1_MBC created.");
}

MBC1_MBC::~MBC1_MBC()
{
    Logger::Log("MBC1_MBC destroyed.");
}

// IMemoryUnit
byte MBC1_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        return m_ROM[address];
    }
    else
    {
        // TODO: Implement this for MBC1
    }

    Logger::Log("MBC1_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC1_MBC::WriteByte(const ushort& address, const byte val)
{
    Logger::Log("MBC1_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
