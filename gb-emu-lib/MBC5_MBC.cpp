#include "PCH.hpp"
#include "MBC5_MBC.hpp"

MBC5_MBC::MBC5_MBC(byte* pROM, byte* pRAM) :
    m_ROM(pROM),
    m_RAM(pRAM)
{
    Logger::Log("MBC5_MBC created.");
}

MBC5_MBC::~MBC5_MBC()
{
    Logger::Log("MBC5_MBC destroyed.");
}

// IMemoryUnit
byte MBC5_MBC::ReadByte(const ushort& address)
{
    Logger::Log("MBC5_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC5_MBC::WriteByte(const ushort& address, const byte val)
{
    Logger::Log("MBC5_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
