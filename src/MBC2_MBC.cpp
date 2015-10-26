#include "PCH.hpp"
#include "MBC2_MBC.hpp"

MBC2_MBC::MBC2_MBC(byte* pROM, byte* pRAM) :
    m_ROM(pROM),
    m_RAM(pRAM)
{
    Logger::Log("MBC2_MBC created.");
}

MBC2_MBC::~MBC2_MBC()
{
    Logger::Log("MBC2_MBC destroyed.");
}

// IMemoryUnit
byte MBC2_MBC::ReadByte(const ushort& address)
{
    Logger::Log("MBC2_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC2_MBC::WriteByte(const ushort& address, const byte val)
{
    Logger::Log("MBC2_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
