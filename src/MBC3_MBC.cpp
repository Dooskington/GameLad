#include "PCH.hpp"
#include "MBC3_MBC.hpp"

MBC3_MBC::MBC3_MBC(byte* pROM, byte* pRAM) :
    m_ROM(pROM),
    m_RAM(pRAM)
{
    Logger::Log("MBC3_MBC created.");
}

MBC3_MBC::~MBC3_MBC()
{
    Logger::Log("MBC3_MBC destroyed.");
}

// IMemoryUnit
byte MBC3_MBC::ReadByte(const ushort& address)
{
    Logger::Log("MBC3_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC3_MBC::WriteByte(const ushort& address, const byte val)
{
    Logger::Log("MBC3_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
