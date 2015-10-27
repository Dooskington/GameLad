#include "PCH.hpp"
#include "MMU.hpp"

/*
General Memory Map
==================
When m_isBooting = true, override with:
-MMU:
0x0000-0x00FF   Boot ROM (256 bytes) [DONE]

When m_isBooting = false:
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
    m_isBooting(true)
{
    Logger::Log("MMU created.");

    RegisterMemoryUnit(0x0000, 0xFFFF, nullptr);
}

MMU::~MMU()
{
    Logger::Log("MMU destroyed.");
}

bool MMU::Initialize()
{
    // Initialize boot rom
    return LoadBootRom("res/bios.bin");
}

void MMU::RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit)
{
    for (ushort index = startRange; index <= endRange; index++)
    {
        m_memoryUnits[index] = pUnit;

        // Prevent wrap around
        if (index == 0xFFFF)
            break;
    }
}

byte MMU::ReadByte(const ushort& address)
{
    // If we are booting and reading below 0x00FF, read from the boot rom.
    if (m_isBooting && (address <= 0x00FF))
    {
        return m_bios[address];
    }

    auto spUnit = std::move(m_memoryUnits[address]);
    if (spUnit == nullptr)
    {
        return ReadByteInternal(address);
    }
    else
    {
        return spUnit->ReadByte(address);
    }
}

ushort MMU::ReadUShort(const ushort& address)
{
    ushort val = ReadByte(address + 1);
    val = val << 8;
    val |= ReadByte(address);
    return val;
}

bool MMU::WriteByte(const ushort& address, const byte val)
{
    if (m_isBooting && (address <= 0x00FF))
    {
        Logger::LogError("Access Violation! You can't write to the boot rom, you silly goose.");
        return false;
    }

    auto spUnit = std::move(m_memoryUnits[address]);
    if (spUnit == nullptr)
    {
        return WriteByteInternal(address, val);
    }
    else
    {
        return spUnit->WriteByte(address, val);
    }
}

bool MMU::LoadBootRom(std::string path)
{
    bool succeeded = false;
    std::streampos size;

    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        file.seekg(0, std::ios::beg);
        if (file.read(reinterpret_cast<char*>(m_bios), size))
        {
            Logger::Log("Loaded boot rom %s (%d bytes)", path.data(), size.seekpos());
            succeeded = true;
        }
        else
        {
            Logger::Log("Failed to load boot rom %s", path);
        }

        file.close();
    }

    return succeeded;
}

byte MMU::ReadByteInternal(const ushort& address)
{
    /*
    -MMU:
    0xC000-0xCFFF   4KB Work RAM Bank 0 (WRAM)
    0xD000-0xDFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
    0xE000-0xFDFF   Same as C000-DDFF (ECHO)    (typically not used)
    0xFF80-0xFFFE   High RAM (HRAM)
    0xFFFF          Interrupt Enable Register
    */

    if (address >= 0xC000 && address <= 0xCFFF)
    {
        return m_bank0[address - 0xC000];
    }
    else if (address >= 0xC000 && address <= 0xDFFF)
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
    else if (address >= 0xFF80 && address <= 0xFFFE)
    {
        return m_HRAM[address - 0xFF80];
    }
    else if (address == 0xFFFF)
    {
        // TODO: Interrupt bit
        Logger::Log("MMU doesn't support interrupts yet!");
        return 0x00;
    }
    else
    {
        Logger::Log("MMU::ReadByteInternal isn't ready to support reading from 0x%04X", address);
        return 0x00;
    }
}

bool MMU::WriteByteInternal(const ushort& address, const byte val)
{
    if (address >= 0xC000 && address <= 0xCFFF)
    {
        m_bank0[address - 0xC000] = val;
    }
    else if (address >= 0xC000 && address <= 0xDFFF)
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
        // TODO: Interrupt bit
        Logger::Log("MMU doesn't support interrupts yet!");
        return false;
    }
    else
    {
        Logger::Log("MMU::WriteByteInternal isn't ready to support writing to 0x%04X", address);
        return false;
    }

    return true;
}
