#include "pch.hpp"
#include "MMU.hpp"

/*
    General Memory Map
    ==================
    When m_isBooting = 0x00, override with:
    -MMU:
    0x0000-0x00FF   Boot ROM (256 bytes) [DONE]

    When m_isBooting != 0x00:
    -Cartridge:
    0x0000-0x3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
    0x4000-0x7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)

    -GPU:
    0x8000-0x9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)

    -Cartridge:
    0xA000-0xBFFF   8KB External RAM     (in cartridge, switchable bank, if any)

    -MMU:
    0xC000-0xCFFF   4KB Work RAM Bank 0 (WRAM)
    0xD000-0xDFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
    0xE000-0xFDFF   Same as C000-DDFF (ECHO)    (typically not used)

    -GPU:
    0xFE00-0xFE9F   Sprite Attribute Table (OAM)

    -N/A:
    0xFEA0-0xFEFF   Not Usable

    -GamePad/SerialData/Timer/Audio/GPU:
    0xFF00-0xFF7F   I/O Ports

    -MMU:
    0xFF80-0xFFFE   High RAM (HRAM)
    0xFFFF          Interrupt Enable Register
*/

MMU::MMU() :
    m_isBooting(0x00)
{
    RegisterMemoryUnit(0x0000, 0xFFFF, this);
}

MMU::~MMU()
{
}

void MMU::RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit)
{
    for (int index = startRange; index <= endRange; index++)
    {
        m_memoryUnits[index] = pUnit;
    }
}

byte MMU::Read(const ushort& address)
{
    // If we are booting and reading below 0x00FF, read from the boot rom.
    if ((m_isBooting == 0x00) && (address <= 0x00FF))
    {
        if (m_BIOS == nullptr)
        {
            Logger::LogError("Access Violation! You can't read from the boot rom if it isn't loaded!");
            return 0x00;
        }

        return m_BIOS.get()[address];
    }

    return m_memoryUnits[address]->ReadByte(address);
}

ushort MMU::ReadUShort(const ushort& address)
{
    ushort val = Read(address + 1);
    val = val << 8;
    val |= Read(address);
    return val;
}

bool MMU::LoadBootROM(const char* bootROMPath)
{
    if (bootROMPath == nullptr)
    {
        // Set the booted flag, this will trigger the CPU to "preboot".
        m_isBooting = 0x01;
        return true;
    }
    else
    {
        bool succeeded = false;
        m_isBooting = 0x00;
        std::streampos size;

        std::ifstream file(bootROMPath, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            size = file.tellg();

#if WINDOWS
            int iSize = static_cast<int>(size.seekpos());
#else
            int iSize = size;
#endif
            if (iSize < 256)
            {
                Logger::Log("Boot rom at '%s' is the wrong size!", bootROMPath);
            }
            else
            {
                file.seekg(0, std::ios::beg);
                m_BIOS = std::unique_ptr<byte>(new byte[static_cast<unsigned int>(iSize)]);
                if (file.read(reinterpret_cast<char*>(m_BIOS.get()), iSize))
                {
                    Logger::Log("Loaded boot rom %s (%d bytes)", bootROMPath, iSize);
                    succeeded = true;
                }
                else
                {
                    Logger::Log("Failed to load boot ROM %s", bootROMPath);
                }
            }
        }

        return succeeded;
    }
}

bool MMU::Write(const ushort& address, const byte val)
{
    if ((m_isBooting == 0x00) && (address <= 0x00FF))
    {
        Logger::LogError("Access Violation! You can't write to the boot ROM [0x%04X = 0x%02X]", address, val);
        return false;
    }

    return m_memoryUnits[address]->WriteByte(address, val);
}

byte MMU::ReadByte(const ushort& address)
{
    /*
    -MMU:
    0xC000-0xCFFF   4KB Work RAM Bank 0 (WRAM)
    0xD000-0xDFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
    0xE000-0xFDFF   Same as C000-DDFF (ECHO)    (typically not used)\
    0xFF50          Boot indicator
    0xFF80-0xFFFE   High RAM (HRAM)
    0xFFFF          Interrupt Enable Register
    */

    if (address >= 0xC000 && address <= 0xCFFF)
    {
        return m_bank0[address - 0xC000];
    }
    else if (address >= 0xD000 && address <= 0xDFFF)
    {
        return m_bank1[address - 0xD000];
    }
    else if (address >= 0xE000 && address <= 0xEFFF)
    {
        return m_bank0[address - 0xE000];
    }
    else if (address >= 0xF000 && address <= 0xFDFF)
    {
        return m_bank1[address - 0xF000];
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF)
    {
        // Unusable memory
        return 0x00;
    }
    else if (address >= 0xFF80 && address <= 0xFFFE)
    {
        return m_HRAM[address - 0xFF80];
    }
    else if (address == 0xFFFF)
    {
        return m_IE;
    }
    else if (address == 0xFF0F)
    {
        return m_IF;
    }
    else if (address == 0xFF50)
    {
        return m_isBooting;
    }
    else if (address == 0xFF4D)
    {
        return m_Key1;
    }
    else
    {
        return 0x00;
    }
}

bool MMU::WriteByte(const ushort& address, const byte val)
{
    if (address >= 0xC000 && address <= 0xCFFF)
    {
        m_bank0[address - 0xC000] = val;
    }
    else if (address >= 0xD000 && address <= 0xDFFF)
    {
        m_bank1[address - 0xD000] = val;
    }
    else if (address >= 0xE000 && address <= 0xEFFF)
    {
        m_bank0[address - 0xE000] = val;
    }
    else if (address >= 0xF000 && address <= 0xFDFF)
    {
        m_bank1[address - 0xF000] = val;
    }
    else if (address >= 0xFF80 && address <= 0xFFFE)
    {
        m_HRAM[address - 0xFF80] = val;
    }
    else if (address == 0xFFFF)
    {
        m_IE = val;
    }
    else if (address == 0xFF0F)
    {
        m_IF = val;
    }
    else if (address == 0xFF50)
    {
        m_isBooting = val;
    }
    else if (address == 0xFF4D)
    {
        m_Key1 = val;
    }

    return true;
}
