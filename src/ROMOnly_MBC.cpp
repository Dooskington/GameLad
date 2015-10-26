#include "PCH.hpp"
#include "ROMOnly_MBC.hpp"

/*
Small games of not more than 32KBytes ROM do not require a MBC chip for ROM banking.
The ROM is directly mapped to memory at 0000-7FFFh. Optionally up to 8KByte of RAM could be
connected at A000-BFFF, even though that could require a tiny MBC-like circuit, but no real MBC chip.
*/

ROMOnly_MBC::ROMOnly_MBC(byte* data) :
    m_ROM(data)
{
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
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        m_RAM[address - 0xA000] = val;
        return true;
    }

    Logger::Log("ROMOnly_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
