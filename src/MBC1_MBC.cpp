#include "PCH.hpp"
#include "MBC1_MBC.hpp"
/*
This is the first MBC chip for the gameboy. Any newer MBC chips are working similiar, so that is
relative easy to upgrade a program from one MBC chip to another - or even to make it compatible to
several different types of MBCs.

Note that the memory in range 0000-7FFF is used for both reading from ROM, and for writing to the
MBCs Control Registers.
*/

#define EnableRAM   0x0A
#define ROMBankMode 0x00
#define RAMBankMode 0x01

MBC1_MBC::MBC1_MBC(byte mbcType, byte* data) :
    m_mbcType(mbcType),
    m_ROM(data),
    m_ROMBankLower(0x00),
    m_ROMRAMBankUpper(0x00),
    m_ROMRAMMode(ROMBankMode),
    m_isRAMEnabled(false)
{
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
        This area may contain any of the further 16KByte banks of the ROM, allowing to address up to 125 ROM
        Banks (almost 2MByte). As described below, bank numbers 20h, 40h, and 60h cannot be used, resulting
        in the odd amount of 125 banks.
        */
        byte targetBank = m_ROMBankLower;
        if (m_ROMRAMMode == ROMBankMode)
        {
            // The upper bank values are only availabel in ROM Bank Mode
            targetBank |= (m_ROMRAMBankUpper << 4);
        }

        byte target = (address - 0x4000);
        target += (0x4000 * targetBank);
        return m_ROM[target];
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        /*
        A000-BFFF - RAM Bank 00-03, if any (Read/Write)
        This area is used to address external RAM in the cartridge (if any). External RAM is often battery
        buffered, allowing to store game positions or high score tables, even if the gameboy is turned off,
        or if the cartridge is removed from the gameboy. Available RAM sizes are: 2KByte (at A000-A7FF),
        8KByte (at A000-BFFF), and 32KByte (in form of four 8K banks at A000-BFFF).
        */
        if (!m_isRAMEnabled)
        {
            Logger::Log("MBC1_MBC::ReadByte doesn't support reading to 0x%04X, RAM disabled.", address);
            return false;
        }

        // In ROM Mode, only bank 0x00 is available
        ushort target = address - 0xA000;
        if (m_ROMRAMMode == RAMBankMode)
        {
            // Offset based on the bank number
            target += (0x2000 * m_ROMBankLower);
        }

        return m_RAM[target];
    }

    Logger::Log("MBC1_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC1_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        /*
        0000-1FFF - RAM Enable (Write Only)
        Before external RAM can be read or written, it must be enabled by writing to this address space.
        It is recommended to disable external RAM after accessing it, in order to protect its contents from
        damage during power down of the gameboy. Usually the following values are used:
        00h  Disable RAM (default)
        0Ah  Enable RAM
        Practically any value with 0Ah in the lower 4 bits enables RAM, and any other value disables RAM.
        */
        m_isRAMEnabled = ((val & EnableRAM) == EnableRAM);
        return true;
    }
    else if (address <= 0x3FFF)
    {
        /*
        2000-3FFF - ROM Bank Number (Write Only)
        Writing to this address space selects the lower 5 bits of the ROM Bank Number (in range 01-1Fh).
        When 00h is written, the MBC translates that to bank 01h also. That doesn't harm so far, because ROM
        Bank 00h can be always directly accessed by reading from 0000-3FFF.
        But (when using the register below to specify the upper ROM Bank bits), the same happens for Bank
        20h, 40h, and 60h. Any attempt to address these ROM Banks will select Bank 21h, 41h, and 61h instead.
        */
        m_ROMBankLower = val & 0x1F;
        if (m_ROMBankLower == 0x00)
        {
            m_ROMBankLower = 0x01;
        }
    }
    else if (address <= 0x5FFF)
    {
        /*
        4000-5FFF - RAM Bank Number - or - Upper Bits of ROM Bank Number (Write Only) This 2bit register
        can be used to select a RAM Bank in range from 00-03h, or to specify the upper two bits (Bit 5-6) of
        the ROM Bank number, depending on the current ROM/RAM Mode. (See below.)
        */

        m_ROMRAMBankUpper = val & 0x03;
    }
    else if (address <= 0x7FFF)
    {
        /*
        6000-7FFF - ROM/RAM Mode Select (Write Only)
        This 1bit Register selects whether the two bits of the above register should be used as upper two
        bits of the ROM Bank, or as RAM Bank Number.
        00h = ROM Banking Mode (up to 8KByte RAM, 2MByte ROM) (default)
        01h = RAM Banking Mode (up to 32KByte RAM, 512KByte ROM)
        The program may freely switch between both modes, the only limitiation is that only RAM Bank 00h
        can be used during Mode 0, and only ROM Banks 00-1Fh can be used during Mode 1.
        */
        m_ROMRAMMode = val & 0x01;
    }

    if (address >= 0xA000 && address <= 0xBFFF)
    {
        /*
        A000-BFFF - RAM Bank 00-03, if any (Read/Write)
        This area is used to address external RAM in the cartridge (if any). External RAM is often battery
        buffered, allowing to store game positions or high score tables, even if the gameboy is turned off,
        or if the cartridge is removed from the gameboy. Available RAM sizes are: 2KByte (at A000-A7FF),
        8KByte (at A000-BFFF), and 32KByte (in form of four 8K banks at A000-BFFF).
        */

        if (!m_isRAMEnabled)
        {
            Logger::Log("MBC1_MBC::WriteByte doesn't support writing to 0x%04X, RAM disabled.", address);
            return false;
        }

        // In ROM Mode, only bank 0x00 is available
        ushort target = address - 0xA000;
        if (m_ROMRAMMode == RAMBankMode)
        {
            // Offset based on the bank number
            target += (0x2000 * m_ROMBankLower);
        }

        m_RAM[target] = val;
        return true;
    }

    Logger::Log("MBC1_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
