#include "PCH.hpp"
#include "MMU.hpp"

MMU::MMU() :
    m_isBooting(true)
{
    Logger::Log("MMU Created.");

    // Initialize boot rom
    LoadBootRom("res/bios.bin");

    // Initialize memory
    memset(m_memory, 0x00, 0xFFFF);
}

MMU::~MMU()
{
    Logger::Log("MMU Destroyed.");
}

byte MMU::Read(unsigned short PC)
{
    if (PC <= 0xFF)
    {
        // If we are reading below 0xFF, and booting, read from the boot rom.
        if (m_isBooting)
        {
            return m_bios[PC];
        }
    }
    else
    {
        return m_memory[PC];
    }
}

void MMU::LoadBootRom(std::string path)
{
    std::streampos size;

    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        file.seekg(0, std::ios::beg);
        if (file.read(m_bios, size))
        {
            std::cout << "Loaded rom " << path << " (" << size << " bytes)" << std::endl;
        }
        file.close();
    }
}