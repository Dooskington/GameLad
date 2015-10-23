#include "PCH.hpp"
#include "MMU.hpp"

MMU::MMU() :
    m_isBooting(true)
{
    Logger::Log("MMU Created.");

    // Initialize memory
    memset(m_memory, 0x00, ARRAYSIZE(m_memory));
}

MMU::~MMU()
{
    Logger::Log("MMU Destroyed.");
}

bool MMU::Initialize()
{
    // Initialize boot rom
    return LoadBootRom("res/bios.bin");
}

byte MMU::Read(unsigned short address)
{
    // If we are booting and reading below 0x00FF, read from the boot rom.
    if (m_isBooting && (address <= 0x00FF))
    {
        return m_bios[address];
    }
    else
    {
        return m_memory[address];
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
        if (file.read(m_bios, size))
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