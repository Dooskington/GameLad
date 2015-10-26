#include "PCH.hpp"
#include "Cartridge.hpp"

#include "ROMOnly_MBC.hpp"
#include "MBC1_MBC.hpp"

#define CartridgeTypeAddress 0x0147
#define ROMSizeAddress 0x0148
#define RAMSizeAddress 0x0149

/*
0x0147 - Cartridge Type
Specifies which Memory Bank Controller (if any) is used in the cartridge, and if further external hardware exists in the cartridge.
00h  ROM ONLY                 13h  MBC3+RAM+BATTERY
01h  MBC1                     15h  MBC4
02h  MBC1+RAM                 16h  MBC4+RAM
03h  MBC1+RAM+BATTERY         17h  MBC4+RAM+BATTERY
05h  MBC2                     19h  MBC5
06h  MBC2+BATTERY             1Ah  MBC5+RAM
08h  ROM+RAM                  1Bh  MBC5+RAM+BATTERY
09h  ROM+RAM+BATTERY          1Ch  MBC5+RUMBLE
0Bh  MMM01                    1Dh  MBC5+RUMBLE+RAM
0Ch  MMM01+RAM                1Eh  MBC5+RUMBLE+RAM+BATTERY
0Dh  MMM01+RAM+BATTERY        FCh  POCKET CAMERA
0Fh  MBC3+TIMER+BATTERY       FDh  BANDAI TAMA5
10h  MBC3+TIMER+RAM+BATTERY   FEh  HuC3
11h  MBC3                     FFh  HuC1+RAM+BATTERY
12h  MBC3+RAM
*/
#define MBC2                0x05
#define MBC2Battery         0x06
#define ROMRAM              0x08
#define ROMRAMBattery       0x09
#define MMM01               0x0B
#define MMM01RAM            0x0C
#define MMM01RAMBattery     0x0D
#define MBC3TimerBattery    0x0F
#define MBC3TimerRAMBattery 0x10
#define MBC3                0x11
#define MBC3RAM             0x12
#define MBC3RAMBattery      0x13
#define MBC4                0x15
#define MBC4RAM             0x16
#define MBC4RAMBattery      0x17
#define MBC5                0x19
#define MBC5RAM             0x1A
#define MBC5RAMBattery      0x1B
#define MBC5Rumble          0x1C
#define MBC5RumbleRAM       0x1D
#define MBC5RumbleRAMBattery 0x1E
#define PocketCamera        0xFC
#define BandaiTama5         0xFD
#define HuC3                0xFE
#define HuC1RAMBattery      0xFF

/*
0x0148 - ROM Size
Specifies the ROM Size of the cartridge. Typically calculated as "32KB shl N".
00h -  32KByte (no ROM banking)
01h -  64KByte (4 banks)
02h - 128KByte (8 banks)
03h - 256KByte (16 banks)
04h - 512KByte (32 banks)
05h -   1MByte (64 banks)  - only 63 banks used by MBC1
06h -   2MByte (128 banks) - only 125 banks used by MBC1
07h -   4MByte (256 banks)
52h - 1.1MByte (72 banks)
53h - 1.2MByte (80 banks)
54h - 1.5MByte (96 banks)
*/
#define ROM_32KB        0x00
#define ROM_64KB        0x01
#define ROM_128KB       0x02
#define ROM_256KB       0x03
#define ROM_512KB       0x04
#define ROM_1MB         0x05
#define ROM_2MB         0x06
#define ROM_4MB         0x07
#define ROM_1_1MB       0x52
#define ROM_1_2MB       0x53
#define ROM_1_5MB       0x54

/*
0x0149 - RAM Size
Specifies the size of the external RAM in the cartridge (if any).
00h - None
01h - 2 KBytes
02h - 8 Kbytes
03h - 32 KBytes (4 banks of 8KBytes each)
When using a MBC2 chip 00h must be specified in this entry, even though the MBC2 includes a built-in
RAM of 512 x 4 bits.
*/
#define RAM_None        0x00
#define RAM_2KB         0x01
#define RAM_8KB         0x02
#define RAM_32KB        0x03

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

        m_ROM = std::unique_ptr<byte>(new byte[size.seekpos()]);

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
                succeeded = LoadMBC(size.seekpos());
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

bool Cartridge::LoadMBC(__int64 actualSize)
{
    byte mbcType = m_ROM.get()[CartridgeTypeAddress];
    byte romSizeFlag = m_ROM.get()[ROMSizeAddress];
    byte ramSizeFlag = m_ROM.get()[RAMSizeAddress];

    __int64 romSize = (32 * 1024) << romSizeFlag;
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

    __int64 ramSize = 0;
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
    default:
        Logger::Log("Unsupported Cartridge MBC type: 0x%02X", mbcType);
        return false;
    }

}
