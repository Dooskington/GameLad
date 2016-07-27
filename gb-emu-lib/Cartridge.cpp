#include "pch.hpp"
#include "Cartridge.hpp"

#include "MBC.hpp"

Cartridge::Cartridge() :
    m_MBCType(ROMOnly)
{
}

Cartridge::~Cartridge()
{
    switch (m_MBCType)
    {
    case MBC1RAMBattery:
    case MBC3TimerBattery:
    case MBC3TimerRAMBattery:
    case MBC3RAMBattery:
    case MBC5RAMBattery:
    case MBC5RumbleRAMBattery:
        // Theses all have batteries to backup the ram, save now
        std::ofstream file(m_Path + "_RAM", std::ios::out | std::ios::binary | std::ios::trunc);
        if (file.is_open())
        {
            file.write((char*)m_RAM.get(), m_RAMSize);
            file.close();
        }
        break;
    }

    m_MBC.reset();
    m_ROM.reset();
    m_RAM.reset();
}

bool Cartridge::LoadROM(const char* path)
{
    m_Path = path;
    bool succeeded = false;
    std::streampos size;

    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
#if WINDOWS
        int iSize = static_cast<int>(size.seekpos());
#else
        int iSize = size;
#endif

        file.seekg(0, std::ios::beg);

        m_ROM = std::unique_ptr<byte>(new byte[static_cast<unsigned int>(iSize)]);

        if (file.read(reinterpret_cast<char*>(m_ROM.get()), size))
        {
            Logger::Log("Loaded game ROM %s (%d bytes)", path, iSize);

            if (iSize < 0x014F)
            {
                Logger::Log("Cartridge doesn't have enough data!", path, iSize);
                succeeded = false;
            }
            else
            {
                succeeded = LoadMBC(static_cast<unsigned int>(iSize));
            }
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
    return m_MBC->ReadByte(address);
}

bool Cartridge::WriteByte(const ushort& address, const byte val)
{
    return m_MBC->WriteByte(address, val);
}

bool Cartridge::LoadMBC(unsigned int actualSize)
{
    m_MBCType = m_ROM.get()[CartridgeTypeAddress];
    byte romSizeFlag = m_ROM.get()[ROMSizeAddress];
    byte ramSizeFlag = m_ROM.get()[RAMSizeAddress];

    unsigned int romSize = (32 * 1024) << romSizeFlag;
    switch (romSizeFlag)
    {
    case ROM_1_1MB:
        romSize = 1179648;
        break;
    case ROM_1_2MB:
        romSize = 1310720;
        break;
    case ROM_1_5MB:
        romSize = 1572864;
        break;
    }

    if (actualSize != romSize)
    {
        Logger::Log("Cartridge::LoadMBC - Unexpected ROM file size. Got: %d   Expected: %d", actualSize, romSize);
        return true;
    }

    switch (ramSizeFlag)
    {
    case RAM_None:
        m_RAMSize = 0;
        break;
    case RAM_2KB:
        m_RAMSize = (1024 * 2);
        break;
    case RAM_8KB:
        m_RAMSize = (1024 * 8);
        break;
    case RAM_32KB:
        m_RAMSize = (1024 * 32);
        break;
    default:
        Logger::Log("Cartridge::LoadMBC - Unexpected RAM size flag: 0x%02X", ramSizeFlag);
        return false;
    }

    if (m_RAMSize > 0)
    {
        std::string ramPath = m_Path + "_RAM";
        m_RAM = std::unique_ptr<byte>(new byte[m_RAMSize]);
        // If _RAM file exists
        std::ifstream file(ramPath, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            std::streampos size = file.tellg();
#if WINDOWS
            unsigned int iSize = static_cast<int>(size.seekpos());
#else
            unsigned int iSize = size;
#endif
            file.seekg(0, std::ios::beg);

            if (iSize != m_RAMSize)
            {
                Logger::Log("Cartridge::LoadMBC - Saved RAM was not the expected size. Got: %d   Expected : %d", iSize, m_RAMSize);
            }
            else if (file.read(reinterpret_cast<char*>(m_RAM.get()), size))
            {
                Logger::Log("Loaded game RAM %s (%d bytes)", ramPath.data(), m_RAMSize);
            }
        }
    }

    switch (m_MBCType)
    {
    case ROMOnly:
        m_MBC = std::unique_ptr<ROMOnly_MBC>(new ROMOnly_MBC(m_ROM.get(), m_RAM.get()));
        return true;
    case MBC1:
    case MBC1RAM:
    case MBC1RAMBattery:
        m_MBC = std::unique_ptr<MBC1_MBC>(new MBC1_MBC(m_ROM.get(), m_RAM.get()));
        return true;
    case MBC2:
    case MBC2Battery:
        m_RAM.reset();
        m_MBC = std::unique_ptr<MBC2_MBC>(new MBC2_MBC(m_ROM.get()));
        return true;
    case MBC3TimerBattery:
    case MBC3TimerRAMBattery:
    case MBC3:
    case MBC3RAM:
    case MBC3RAMBattery:
        m_MBC = std::unique_ptr<MBC3_MBC>(new MBC3_MBC(m_ROM.get(), m_RAM.get()));
        return true;
    case MBC5:
    case MBC5RAM:
    case MBC5RAMBattery:
    case MBC5Rumble:
    case MBC5RumbleRAM:
    case MBC5RumbleRAMBattery:
        m_MBC = std::unique_ptr<MBC5_MBC>(new MBC5_MBC(m_ROM.get(), m_RAM.get()));
        return true;
    default:
        Logger::Log("Unsupported Cartridge MBC type: 0x%02X", m_MBCType);
        return false;
    }
}
