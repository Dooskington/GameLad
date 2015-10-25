#include "PCH.hpp"
#include "Cartridge.hpp"

Cartridge::Cartridge()
{
    Logger::Log("Cartridge created.");
}

Cartridge::~Cartridge()
{
    m_ROM.reset();
    Logger::Log("Cartridge destroyed.");
}

bool Cartridge::LoadROM(std::string path)
{
    bool succeeded = false;
    std::streampos size;

    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        file.seekg(0, std::ios::beg);

        m_ROM = std::unique_ptr<byte>(new byte[size.seekpos()]);

        if (file.read(reinterpret_cast<char*>(m_ROM.get()), size))
        {
            Logger::Log("Loaded game rom %s (%d bytes)", path.data(), size.seekpos());
            succeeded = true;
        }
        else
        {
            Logger::Log("Failed to load game rom %s", path);
        }

        file.close();
    }

    return succeeded;
}

// IMemoryUnit
byte Cartridge::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        return m_ROM.get()[address];
    }
    else
    {
        // TODO: Implement MBC as necessary
        Logger::Log("Cartridge::ReadByte isn't ready to support reading from 0x%04X", address);
        return 0x00;
    }
}

bool Cartridge::WriteByte(const ushort& address, const byte val)
{
    return false;
}
