#include "PCH.hpp"
#include "Cartridge.hpp"

#include "ROMOnly_MBC.hpp"
#include "MBC1_MBC.hpp"
#include "MBC2_MBC.hpp"
#include "MBC3_MBC.hpp"
#include "MBC4_MBC.hpp"
#include "MBC5_MBC.hpp"

Cartridge::Cartridge()
{
    Logger::Log("Cartridge created.");
}

Cartridge::~Cartridge()
{
    m_MBC.reset();
    m_ROM.reset();
    m_RAM.reset();

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

        m_ROM = std::unique_ptr<byte>(new byte[static_cast<unsigned int>(size.seekpos())]);

        if (file.read(reinterpret_cast<char*>(m_ROM.get()), size))
        {
            Logger::Log("Loaded game rom %s (%d bytes)", path.data(), size.seekpos());

            if (size.seekpos() < 0x014F)
            {
                Logger::Log("Cartridge doesn't have enough data!", path.data(), size.seekpos());
                succeeded = false;
            }
            else
            {
                succeeded = LoadMBC(static_cast<unsigned int>(size.seekpos()));
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
    byte mbcType = m_ROM.get()[CartridgeTypeAddress];
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
        return false;
    }

    unsigned int ramSize = 0;
    switch (ramSizeFlag)
    {
    case RAM_None:
        ramSize = 0;
        break;
    case RAM_2KB:
        ramSize = (1024 * 2);
        break;
    case RAM_8KB:
        ramSize = (1024 * 8);
        break;
    case RAM_32KB:
        ramSize = (1024 * 32);
        break;
    default:
        Logger::Log("Cartridge::LoadMBC - Unexpected RAM size flag: 0x%02X", ramSizeFlag);
        return false;
    }

    if (ramSize > 0)
    {
        m_RAM = std::unique_ptr<byte>(new byte[ramSize]);
    }

    switch (mbcType)
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
    case MBC4:
    case MBC4RAM:
    case MBC4RAMBattery:
        m_MBC = std::unique_ptr<MBC4_MBC>(new MBC4_MBC(m_ROM.get(), m_RAM.get()));
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
        Logger::Log("Unsupported Cartridge MBC type: 0x%02X", mbcType);
        return false;
    }
}
