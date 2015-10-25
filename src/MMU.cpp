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

-MMU:
0xA000-0xBFFF   8KB External RAM     (in cartridge, switchable bank, if any)
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

    // Initialize memory
    memset(m_memory, 0x00, ARRAYSIZE(m_memory));
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

byte MMU::ReadByte(const ushort& address)
{
    // If we are booting and reading below 0x00FF, read from the boot rom.
    if (m_isBooting && (address <= 0x00FF))
    {
        return m_bios[address];
    }
    else
    {
        // TODO: Eventually, this will go away and instead it will read from the correct
        // device.  For example 0x2345 will read from the 16KB ROM Bank 00 on the cartridge.
        return m_memory[address];
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
    else
    {
        // TODO: Eventually, this will go away and instead it will read from the correct
        // device.  For example 0x2345 will read from the 16KB ROM Bank 00 on the cartridge.
        //Logger::LogError("Access Violation! You can't write to the cartridge, you silly goose.");
        m_memory[address] = val;
        return true;
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
            Logger::Log("Loaded boot rom %s (%d bytes)", path, size.seekpos());
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
