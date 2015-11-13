#include "pch.hpp"
#include "MBC2_MBC.hpp"

/*
MBC2 (max 256KByte ROM and 512x4 bits RAM)
*/

#define EnableRAM   0x0A

MBC2_MBC::MBC2_MBC(byte* pROM) :
    m_ROM(pROM),
    m_ROMBank(0x01),
    m_isRAMEnabled(false)
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
    if (address <= 0x3FFF)
    {
        /*
        0000-3FFF - ROM Bank 00 (Read Only)
        This area always contains the first 16KBytes of the cartridge ROM.
        */
        return m_ROM[address];
    }
    else if (address <= 0x7FFF)
    {
        /*
        4000-7FFF - ROM Bank 01-7F (Read Only)
        This area may contain any of the further 16KByte banks of the ROM, allowing to address up to 16 ROM
        Banks (almost 256KByte).
        */
        ushort target = (address - 0x4000);
        target += (0x4000 * m_ROMBank);
        return m_ROM[target];
    }
    else if (address >= 0xA000 && address <= 0xA1FF)
    {
        /*
        A000-A1FF - 512x4bits RAM, built-in into the MBC2 chip (Read/Write)
        The MBC2 doesn't support external RAM, instead it includes 512x4 bits of built-in RAM (in the MBC2
        chip itself). It still requires an external battery to save data during power-off though.
        As the data consists of 4bit values, only the lower 4 bits of the "bytes" in this memory area are used.
        */
        if (!m_isRAMEnabled)
        {
            Logger::Log("MBC2_MBC::ReadByte doesn't support reading from 0x%04X, RAM disabled.", address);
            return 0x00;
        }

        return m_RAM[address - 0xA000];
    }

    Logger::Log("MBC2_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC2_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        /*
        0000-1FFF - RAM Enable (Write Only)
        The least significant bit of the upper address byte must be zero to enable/disable cart RAM. For
        example the following addresses can be used to enable/disable cart RAM: 0000-00FF, 0200-02FF,
        0400-04FF, ..., 1E00-1EFF.
        The suggested address range to use for MBC2 ram enable/disable is 0000-00FF.
        */
        if ((address & 0x0100) == 0x0000)
        {
            m_isRAMEnabled = ((val & EnableRAM) == EnableRAM);
            return true;
        }
    }
    else if (address <= 0x3FFF)
    {
        /*
        2000-3FFF - ROM Bank Number (Write Only)
        Writing a value (XXXXBBBB - X = Don't cares, B = bank select bits) into 2000-3FFF area will select
        an appropriate ROM bank at 4000-7FFF.

        The least significant bit of the upper address byte must be one to select a ROM bank. For example
        the following addresses can be used to select a ROM bank: 2100-21FF, 2300-23FF, 2500-25FF, ...,
        3F00-3FFF.
        The suggested address range to use for MBC2 rom bank selection is 2100-21FF.
        */
        if ((address & 0x0100) == 0x0000)
        {
            m_ROMBank = (val & 0x0F);
            return true;
        }
    }
    else if (address >= 0xA000 && address <= 0xA1FF)
    {
        /*
        A000-A1FF - 512x4bits RAM, built-in into the MBC2 chip (Read/Write)
        The MBC2 doesn't support external RAM, instead it includes 512x4 bits of built-in RAM (in the MBC2
        chip itself). It still requires an external battery to save data during power-off though.
        As the data consists of 4bit values, only the lower 4 bits of the "bytes" in this memory area are used.
        */
        if (!m_isRAMEnabled)
        {
            Logger::Log("MBC2_MBC::ReadByte doesn't support writing to 0x%04X, RAM disabled.", address);
            return false;
        }

        m_RAM[address - 0xA000] = (val & 0x0F);
        return true;
    }

    Logger::Log("MBC2_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
