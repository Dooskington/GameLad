#include "PCH.hpp"
#include "MBC4_MBC.hpp"

MBC4_MBC::MBC4_MBC(byte* pROM, byte* pRAM) :
    m_ROM(pROM),
    m_RAM(pRAM)
{
    Logger::Log("MBC4_MBC created.");
}

MBC4_MBC::~MBC4_MBC()
{
    Logger::Log("MBC4_MBC destroyed.");
}

// IMemoryUnit
byte MBC4_MBC::ReadByte(const ushort& address)
{
    Logger::Log("MBC4_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC4_MBC::WriteByte(const ushort& address, const byte val)
{
    Logger::Log("MBC4_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
