#include "pch.hpp"
#include "MBC.hpp"
/*
MBC1 (max 2MByte ROM and/or 32KByte RAM)

This is the first MBC chip for the gameboy. Any newer MBC chips are working similiar, so that is
relative easy to upgrade a program from one MBC chip to another - or even to make it compatible to
several different types of MBCs.

Note that the memory in range 0000-7FFF is used for both reading from ROM, and for writing to the
MBCs Control Registers.
*/

#define EnableRAM   0x0A
#define ROMBankMode 0x00
#define RAMBankMode 0x01

MBC::MBC(byte* pROM, byte* pRAM) :
    m_ROM(pROM),
    m_RAM(pRAM),
    m_isRAMEnabled(false)
{
}

MBC::~MBC()
{
}

/*
Small games of not more than 32KBytes ROM do not require a MBC chip for ROM banking.
The ROM is directly mapped to memory at 0000-7FFFh. Optionally up to 8KByte of RAM could be
connected at A000-BFFF, even though that could require a tiny MBC-like circuit, but no real MBC chip.
*/

ROMOnly_MBC::ROMOnly_MBC(byte* pROM, byte* pRAM) :
    MBC(pROM, pRAM)
{
}

ROMOnly_MBC::~ROMOnly_MBC()
{
}

// IMemoryUnit
byte ROMOnly_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x7FFF)
    {
        return m_ROM[address];
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (m_RAM == nullptr)
        {
            //Logger::Log("ROMOnly_MBC::ReadByte doesn't support reading from 0x%04X, RAM not initialized.", address);
            return 0x00;
        }

        return m_RAM[address - 0xA000];
    }

    Logger::Log("ROMOnly_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool ROMOnly_MBC::WriteByte(const ushort& address, const byte val)
{
    if (m_RAM == nullptr)
    {
        //Logger::Log("ROMOnly_MBC::WriteByte doesn't support writing to 0x%04X, RAM not initialized.", address);
        return false;
    }

    if (address >= 0xA000 && address <= 0xBFFF)
    {
        m_RAM[address - 0xA000] = val;
        return true;
    }

    Logger::Log("ROMOnly_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}

MBC1_MBC::MBC1_MBC(byte* pROM, byte* pRAM) :
    MBC(pROM, pRAM),
    m_ROMBankLower(0x01),
    m_ROMRAMBankUpper(0x00),
    m_ROMRAMMode(ROMBankMode)
{
}

MBC1_MBC::~MBC1_MBC()
{
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
            // The upper bank values are only available in ROM Bank Mode
            targetBank |= (m_ROMRAMBankUpper << 4);
        }

        unsigned int target = (address - 0x4000);
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
            // RAM disabled
            return 0xFF;
        }

        if (m_RAM == nullptr)
        {
            // RAM not initialized
            return 0xFF;
        }

        // In ROM Mode, only bank 0x00 is available
        unsigned int target = address - 0xA000;
        if (m_ROMRAMMode == RAMBankMode)
        {
            // Offset based on the bank number
            target += (0x2000 * m_ROMRAMBankUpper);
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

        return true;
    }
    else if (address <= 0x5FFF)
    {
        /*
        4000-5FFF - RAM Bank Number - or - Upper Bits of ROM Bank Number (Write Only) This 2bit register
        can be used to select a RAM Bank in range from 00-03h, or to specify the upper two bits (Bit 5-6) of
        the ROM Bank number, depending on the current ROM/RAM Mode. (See below.)
        */

        m_ROMRAMBankUpper = val & 0x03;
        return true;
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
        return true;
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
            // RAM disabled
            return false;
        }

        if (m_RAM == nullptr)
        {
            // RAM not initialized
            return false;
        }

        // In ROM Mode, only bank 0x00 is available
        unsigned int target = address - 0xA000;
        if (m_ROMRAMMode == RAMBankMode)
        {
            // Offset based on the bank number
            target += (0x2000 * m_ROMRAMBankUpper);
        }

        m_RAM[target] = val;
        return true;
    }

    Logger::Log("MBC1_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}

/*
MBC2 (max 256KByte ROM and 512x4 bits RAM)
*/

MBC2_MBC::MBC2_MBC(byte* pROM) :
    MBC(pROM, new byte[0x1FF + 1]),
    m_ROMBank(0x01)
{
}

MBC2_MBC::~MBC2_MBC()
{
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
        unsigned int target = (address - 0x4000);
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
            //Logger::Log("MBC2_MBC::ReadByte doesn't support reading from 0x%04X, RAM disabled.", address);
            return 0xFF;
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
            //Logger::Log("MBC2_MBC::ReadByte doesn't support writing to 0x%04X, RAM disabled.", address);
            return false;
        }

        m_RAM[address - 0xA000] = (val & 0x0F);
        return true;
    }

    Logger::Log("MBC2_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}


/*
MBC3 (max 2MByte ROM and/or 32KByte RAM and Timer)

Beside for the ability to access up to 2MB ROM (128 banks), and 32KB RAM (4 banks), the MBC3 also
includes a built-in Real Time Clock (RTC). The RTC requires an external 32.768 kHz Quartz
Oscillator, and an external battery (if it should continue to tick when the gameboy is turned off).
*/

// TODO: Implement a timer

/*
The Clock Counter Registers
08h  RTC S   Seconds   0-59 (0-3Bh)
09h  RTC M   Minutes   0-59 (0-3Bh)
0Ah  RTC H   Hours     0-23 (0-17h)
0Bh  RTC DL  Lower 8 bits of Day Counter (0-FFh)
0Ch  RTC DH  Upper 1 bit of Day Counter, Carry Bit, Halt Flag
Bit 0  Most significant bit of Day Counter (Bit 8)
Bit 6  Halt (0=Active, 1=Stop Timer)
Bit 7  Day Counter Carry Bit (1=Counter Overflow)
The Halt Flag is supposed to be set before <writing> to the RTC Registers.

The Day Counter
The total 9 bits of the Day Counter allow to count days in range from 0-511 (0-1FFh). The Day
Counter Carry Bit becomes set when this value overflows. In that case the Carry Bit remains set
until the program does reset it.
Note that you can store an offset to the Day Counter in battery RAM. For example, every time you
read a non-zero Day Counter, add this Counter to the offset in RAM, and reset the Counter to zero.
This method allows to count any number of days, making your program Year-10000-Proof, provided that
the cartridge gets used at least every 511 days.

Delays
When accessing the RTC Registers it is recommended to execute a 4ms delay (4 Cycles in Normal
Speed Mode) between the separate accesses.
*/

MBC3_MBC::MBC3_MBC(byte* pROM, byte* pRAM) :
    MBC(pROM, pRAM),
    m_ROMBank(0x01),
    m_RAMBank(0x00)
{
    memset(m_RTCRegisters, 0x00, ARRAYSIZE(m_RTCRegisters));
}

MBC3_MBC::~MBC3_MBC()
{
}

// IMemoryUnit
byte MBC3_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        /*
        0000-3FFF - ROM Bank 00 (Read Only)
        Same as for MBC1.
        */
        return m_ROM[address];
    }
    else if (address <= 0x7FFF)
    {
        /*
        4000-7FFF - ROM Bank 01-7F (Read Only)
        Same as for MBC1, except that accessing banks 20h, 40h, and 60h is supported now.
        */
        unsigned int target = (address - 0x4000);
        target += (0x4000 * m_ROMBank);
        return m_ROM[target];
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        /*
        A000-BFFF - RAM Bank 00-03, if any (Read/Write)
        A000-BFFF - RTC Register 08-0C (Read/Write)
        Depending on the current Bank Number/RTC Register selection (see below), this memory space is used
        to access an 8KByte external RAM Bank, or a single RTC Register.
        */
        if (!m_isRAMEnabled)
        {
            //Logger::Log("MBC3_MBC::ReadByte doesn't support reading from 0x%04X, RAM disabled.", address);
            return 0xFF;
        }

        if (m_RAM == nullptr)
        {
            //Logger::Log("MBC3_MBC::ReadByte doesn't support reading from 0x%04X, RAM not initialized.", address);
            return 0xFF;
        }

        if (m_RAMBank <= 0x03)
        {
            unsigned int target = address - 0xA000;
            // Offset based on the bank number
            target += (0x2000 * m_RAMBank);
            return m_RAM[target];
        }
        else if (m_RAMBank >= 0x08 && m_RAMBank <= 0x0C)
        {
            return m_RTCRegisters[m_RAMBank - 0x08];
        }
    }

    Logger::Log("MBC3_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC3_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        /*
        0000-1FFF - RAM and Timer Enable (Write Only)
        Mostly the same as for MBC1, a value of 0Ah will enable reading and writing to external RAM - and
        to the RTC Registers! A value of 00h will disable either.
        */
        m_isRAMEnabled = ((val & EnableRAM) == EnableRAM);
        return true;
    }
    else if (address <= 0x3FFF)
    {
        /*
        2000-3FFF - ROM Bank Number (Write Only)
        Same as for MBC1, except that the whole 7 bits of the RAM Bank Number are written directly to this
        address. As for the MBC1, writing a value of 00h, will select Bank 01h instead. All other values
        01-7Fh select the corresponding ROM Banks.
        */
        m_ROMBank = (val & 0x7F);
        if (m_ROMBank == 0x00)
        {
            m_ROMBank = 0x01;
        }

        return true;
    }
    else if (address <= 0x5FFF)
    {
        /*
        4000-5FFF - RAM Bank Number - or - RTC Register Select (Write Only)
        As for the MBC1s RAM Banking Mode, writing a value in range for 00h-03h maps the corresponding
        external RAM Bank (if any) into memory at A000-BFFF.
        When writing a value of 08h-0Ch, this will map the corresponding RTC register into memory at
        A000-BFFF. That register could then be read/written by accessing any address in that area,
        typically that is done by using address A000.
        */
        m_RAMBank = val;
        return true;
    }
    else if (address <= 0x7FFF)
    {
        /*
        6000-7FFF - Latch Clock Data (Write Only)
        When writing 00h, and then 01h to this register, the current time becomes latched into the RTC
        registers. The latched data will not change until it becomes latched again, by repeating the
        write 00h->01h procedure.
        This is supposed for <reading> from the RTC registers. It is proof to read the latched (frozen)
        time from the RTC registers, while the clock itself continues to tick in background.
        */

        // TODO: Look at this for ideas:
        // https://github.com/creker/Cookieboy/blob/a476f8ec5baffd176ee1572885edb7f41b241571/CookieboyMBC3.h
        Logger::Log("MBC3_MBC::WriteByte doesn't support Latch Clock Data yet. 0x%04X", address);
        return false;
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        /*
        A000-BFFF - RAM Bank 00-03, if any (Read/Write)
        A000-BFFF - RTC Register 08-0C (Read/Write)
        Depending on the current Bank Number/RTC Register selection (see below), this memory space is used
        to access an 8KByte external RAM Bank, or a single RTC Register.
        */

        if (!m_isRAMEnabled)
        {
            //Logger::Log("MBC3_MBC::WriteByte doesn't support writing to 0x%04X, RAM disabled.", address);
            return false;
        }

        if (m_RAM == nullptr)
        {
            //Logger::Log("MBC3_MBC::WriteByte doesn't support writing to 0x%04X, RAM not initialized.", address);
            return false;
        }

        if (m_RAMBank <= 0x03)
        {
            unsigned int target = address - 0xA000;
            // Offset based on the bank number
            target += (0x2000 * m_RAMBank);
            m_RAM[target] = val;
            return true;
        }
        else if (m_RAMBank >= 0x08 && m_RAMBank <= 0x0C)
        {
            m_RTCRegisters[m_RAMBank - 0x08] = val;
            return true;
        }
    }

    Logger::Log("MBC3_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}

/*
MBC5 (max 2MByte ROM and/or 32KByte RAM and Timer)

- ROM upto 64MBit (8MByte) divided into 512 banks, each 16kByte.
- RAM upto 1MBit (128kByte) divided into 16 banks, each 8kByte
*/

MBC5_MBC::MBC5_MBC(byte* pROM, byte* pRAM) :
    MBC(pROM, pRAM),
    m_RAMG(0x00),
    m_ROMBank(0x0000),
    m_RAMBank(0x00)
{
}

MBC5_MBC::~MBC5_MBC()
{
}

// IMemoryUnit
byte MBC5_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        /*
        0000-3FFF - ROM Bank 00 (Read Only)
        Same as for MBC1.
        */
        return m_ROM[address];
    }
    else if (address <= 0x7FFF)
    {
        /*
        16kByte switchable ROM bank (Latched to MBC5 pins "RA")
        Bank 0-511 (The 0 bank here is a MBC5 speciality)
        HLLLLLL LLAAAAAA AAAAAAAA (64MBit)
        (i/o resides in this bank, see below)
        */
        unsigned int target = (address - 0x4000);
        target += (0x4000 * m_ROMBank);
        return m_ROM[target];
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        /*
        8kByte switchable RAM bank (Latched via MBC5 pins "AA")
        This is 17 bit wide in order to access 1MBit.
        B BBBAAAAA AAAAAAAA
        */
        unsigned int target = address - 0xA000;
        target += (0x2000 * m_RAMBank);
        return m_RAM[target];
    }

    Logger::Log("MBC5_MBC::ReadByte doesn't support reading from 0x%04X", address);
    return 0x00;
}

bool MBC5_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        /*
        External Extended Memory Register (RAMG)
        (This is ram enable on original MBC5 with 0x0A)

        7  6  5  4  3  2  1  0
        |  |              |
        |  |           1 = RAM write enable
        |  |           0 = RAM write inhibit
        |  |
        |  1 = LED on
        |  0 = LED off
        |
        1 = IO enable
        0 = IO disable
        */
        m_RAMG = val;
        return true;
    }
    else if (address <= 0x2FFF)
    {
        /*
        Lower ROM Bank Register (ROMB0)
        Switchable ROM bank low select (First 8 RA bits)
        LLLL LLLL
        */
        m_ROMBank = (m_ROMBank & 0xFF00) | val;
        return true;
    }
    else if (address <= 0x3FFF)
    {
        /*
        Upper ROM Bank Register (ROMB1)
        Switchable ROM bank high select (9th RA bit)
        XXXX XXXH
        */
        ushort upper = (ushort)(val & 0x01);
        m_ROMBank = (m_ROMBank & 0x00FF) | (upper << 8);
        return true;
    }
    else if (address <= 0x4FFF)
    {
        /*
        RAM Bank Register (RAMB)
        Switchable RAM bank select (4 AA bits)
        XXXX BBBB
        */
        m_RAMBank = (val & 0x0F);
        return true;
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        /*
        8kByte switchable RAM bank (Latched via MBC5 pins "AA")
        This is 17 bit wide in order to access 1MBit.
        B BBBAAAAA AAAAAAAA
        */
        unsigned int target = address - 0xA000;
        target += (0x2000 * m_RAMBank);
        m_RAM[target] = val;
        return true;
    }

    Logger::Log("MBC5_MBC::WriteByte doesn't support writing to 0x%04X", address);
    return false;
}
