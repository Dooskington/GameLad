#include "pch.hpp"
#include "MMU.hpp"

#include <limits>

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
    m_isBooting(0x00),
    m_BIOSSize(0),
    m_mode(GameBoyMode::DMG),
    m_SVBK(0x00),
    m_speedSwitchArmed(false),
    m_doubleSpeed(false),
    m_RP(0x00),
    m_OPRI(0x00),
    m_undocumented72(0x00),
    m_undocumented73(0x00),
    m_undocumented74(0x00),
    m_undocumented75(0x00),
    m_IE(0x00),
    m_IF(0x00)
{
    memset(m_WRAM, 0x00, sizeof(m_WRAM));
    memset(m_HRAM, 0x00, sizeof(m_HRAM));
    RegisterMemoryUnit(0x0000, 0xFFFF, this);
}

MMU::~MMU()
{
}

void MMU::SetGameBoyMode(GameBoyMode mode)
{
    m_mode = mode;

    if (!IsCGBHardware(mode))
    {
        // A DMG has no second speed and no WRAM banking; make sure a mode
        // change can never strand the MMU in a CGB-only state.
        m_speedSwitchArmed = false;
        m_doubleSpeed = false;
        m_SVBK = 0x00;
    }
}

void MMU::CompleteSpeedSwitch()
{
    m_doubleSpeed = !m_doubleSpeed;
    m_speedSwitchArmed = false;
}

unsigned int MMU::CurrentWRAMBank() const
{
    if (!IsCGBFeatureMode(m_mode))
    {
        return 1;
    }

    const unsigned int bank = m_SVBK & 0x07;
    return (bank == 0) ? 1 : bank;
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
    // CGB boot ROM keeps 0x0100-0x01FF mapped to the cartridge header.
    const bool bootROMAddress =
        address <= 0x00FF ||
        (m_BIOSSize >= 0x0900 && address >= 0x0200 && address <= 0x08FF);
    if (m_isBooting == 0x00 && bootROMAddress)
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
        return LoadBootROM(nullptr, 0);
    }

    std::ifstream file(bootROMPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        Logger::Log("Failed to load boot ROM %s", bootROMPath);
        return false;
    }

    const std::streampos streamSize = file.tellg();
    if (streamSize < static_cast<std::streampos>(256) ||
        streamSize > static_cast<std::streampos>((std::numeric_limits<unsigned int>::max)()))
    {
        Logger::Log("Boot rom at '%s' is the wrong size!", bootROMPath);
        return false;
    }

    const size_t size = static_cast<size_t>(streamSize);
    std::unique_ptr<byte[]> bios(new byte[size]);
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(bios.get()), size))
    {
        Logger::Log("Failed to load boot ROM %s", bootROMPath);
        return false;
    }

    if (!LoadBootROM(bios.get(), size))
    {
        return false;
    }
    Logger::Log("Loaded boot rom %s (%u bytes)", bootROMPath, static_cast<unsigned int>(size));
    return true;
}

bool MMU::LoadBootROM(const byte* bootROMData, size_t bootROMSize)
{
    if (bootROMData == nullptr || bootROMSize == 0)
    {
        m_BIOS.reset();
        m_BIOSSize = 0;
        // Set the booted flag, which causes the CPU to install post-boot state.
        m_isBooting = 0x01;
        return true;
    }

    if (bootROMSize < 256 ||
        bootROMSize > static_cast<size_t>((std::numeric_limits<unsigned int>::max)()))
    {
        return false;
    }

    m_BIOS.reset(new byte[bootROMSize]);
    std::memcpy(m_BIOS.get(), bootROMData, bootROMSize);
    m_BIOSSize = bootROMSize;
    m_isBooting = 0x00;
    return true;
}

void MMU::Serialize(StateSerializer& state)
{
    unsigned long long biosSize = static_cast<unsigned long long>(m_BIOSSize);
    unsigned long long biosHash = 1469598103934665603ULL;
    for (size_t index = 0; index < m_BIOSSize; ++index)
    {
        biosHash ^= m_BIOS[index];
        biosHash *= 1099511628211ULL;
    }
    state.Sync(biosSize);
    state.Sync(biosHash);
    state.Sync(m_isBooting);
    state.SyncEnum(m_mode);
    state.SyncBytes(m_WRAM, sizeof(m_WRAM));
    state.Sync(m_SVBK);
    state.SyncBytes(m_HRAM, sizeof(m_HRAM));
    state.Sync(m_speedSwitchArmed);
    state.Sync(m_doubleSpeed);
    state.Sync(m_RP);
    state.Sync(m_OPRI);
    state.Sync(m_undocumented72);
    state.Sync(m_undocumented73);
    state.Sync(m_undocumented74);
    state.Sync(m_undocumented75);
    state.Sync(m_IE);
    state.Sync(m_IF);

    if (state.IsReading() &&
        (biosSize != m_BIOSSize ||
         biosHash != [&]()
         {
             unsigned long long hash = 1469598103934665603ULL;
             for (size_t index = 0; index < m_BIOSSize; ++index)
             {
                 hash ^= m_BIOS[index];
                 hash *= 1099511628211ULL;
             }
             return hash;
         }() ||
         static_cast<unsigned int>(m_mode) >
             static_cast<unsigned int>(GameBoyMode::CGBCompatibility) ||
         m_SVBK > 0x07 ||
         m_IF > 0x1F ||
         (!IsCGBHardware(m_mode) && (m_speedSwitchArmed || m_doubleSpeed))))
    {
        state.Invalidate();
    }
}

bool MMU::Write(const ushort& address, const byte val)
{
    // The boot ROM overlays cartridge reads only; MBC register writes still reach the cartridge.
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
        return m_WRAM[0][address - 0xC000];
    }
    else if (address >= 0xD000 && address <= 0xDFFF)
    {
        return m_WRAM[CurrentWRAMBank()][address - 0xD000];
    }
    else if (address >= 0xE000 && address <= 0xEFFF)
    {
        return m_WRAM[0][address - 0xE000];
    }
    else if (address >= 0xF000 && address <= 0xFDFF)
    {
        return m_WRAM[CurrentWRAMBank()][address - 0xF000];
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
        return m_IF | 0xE0;
    }
    else if (address == 0xFF50)
    {
        return m_isBooting;
    }
    else if (address >= 0xFF00 && address <= 0xFF7F)
    {
        return ReadIORegister(address);
    }
    else if (address == 0xFF7F)
    {
        return 0xFF;
    }
    else
    {
        return 0x00;
    }
}

/*
    I/O registers that reach the MMU rather than a dedicated component:
    0xFF4D KEY1, 0xFF56 RP, 0xFF6C OPRI, 0xFF70 SVBK, 0xFF72-0xFF75.
    Everything else in the I/O page that is not claimed by a component is
    unmapped and reads back as 0xFF on hardware.
*/
byte MMU::ReadIORegister(const ushort& address)
{
    const bool cgbFeatures = IsCGBFeatureMode(m_mode);

    switch (address)
    {
    case 0xFF4D: // KEY1
        if (!IsCGBHardware(m_mode))
        {
            return 0xFF;
        }
        // Bit 7 current speed, bit 0 prepare, bits 1-6 always read 1.
        return (m_doubleSpeed ? 0x80 : 0x00) | (m_speedSwitchArmed ? 0x01 : 0x00) | 0x7E;
    case 0xFF56: // RP (infrared)
        if (!cgbFeatures)
        {
            return 0xFF;
        }
        // Bits 2-5 are unused and read 1. Bit 1 is the received signal, which
        // reads 1 ("no signal") whenever the read enable bits are not both set.
        return static_cast<byte>((m_RP & 0xC1) | 0x3C | (((m_RP & 0xC0) == 0xC0) ? 0x00 : 0x02));
    case 0xFF6C: // OPRI
        if (!cgbFeatures)
        {
            return 0xFF;
        }
        return static_cast<byte>((m_OPRI & 0x01) | 0xFE);
    case 0xFF70: // SVBK
        if (!cgbFeatures)
        {
            return 0xFF;
        }
        return static_cast<byte>((m_SVBK & 0x07) | 0xF8);
    case 0xFF72:
        return cgbFeatures ? m_undocumented72 : 0xFF;
    case 0xFF73:
        return cgbFeatures ? m_undocumented73 : 0xFF;
    case 0xFF74:
        return cgbFeatures ? m_undocumented74 : 0xFF;
    case 0xFF75:
        return cgbFeatures ? static_cast<byte>((m_undocumented75 & 0x70) | 0x8F) : 0xFF;
    default:
        return 0xFF;
    }
}

bool MMU::WriteByte(const ushort& address, const byte val)
{
    if (address >= 0xC000 && address <= 0xCFFF)
    {
        m_WRAM[0][address - 0xC000] = val;
    }
    else if (address >= 0xD000 && address <= 0xDFFF)
    {
        m_WRAM[CurrentWRAMBank()][address - 0xD000] = val;
    }
    else if (address >= 0xE000 && address <= 0xEFFF)
    {
        m_WRAM[0][address - 0xE000] = val;
    }
    else if (address >= 0xF000 && address <= 0xFDFF)
    {
        m_WRAM[CurrentWRAMBank()][address - 0xF000] = val;
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
        m_IF = val & 0x1F;
    }
    else if (address == 0xFF50)
    {
        if (m_isBooting == 0x00 && val != 0x00)
        {
            m_isBooting = val;
        }
    }
    else if (address >= 0xFF00 && address <= 0xFF7F)
    {
        return WriteIORegister(address, val);
    }

    return true;
}

bool MMU::WriteIORegister(const ushort& address, const byte val)
{
    const bool cgbFeatures = IsCGBFeatureMode(m_mode);

    switch (address)
    {
    case 0xFF4D: // KEY1
        if (IsCGBHardware(m_mode))
        {
            // Only the prepare bit is writable; bit 7 is set by the switch itself.
            m_speedSwitchArmed = (val & 0x01) != 0;
        }
        break;
    case 0xFF56: // RP
        if (cgbFeatures)
        {
            m_RP = val;
        }
        break;
    case 0xFF6C: // OPRI
        if (cgbFeatures)
        {
            m_OPRI = val & 0x01;
        }
        break;
    case 0xFF70: // SVBK
        if (cgbFeatures)
        {
            m_SVBK = val & 0x07;
        }
        break;
    case 0xFF72:
        if (cgbFeatures) { m_undocumented72 = val; }
        break;
    case 0xFF73:
        if (cgbFeatures) { m_undocumented73 = val; }
        break;
    case 0xFF74:
        if (cgbFeatures) { m_undocumented74 = val; }
        break;
    case 0xFF75:
        if (cgbFeatures) { m_undocumented75 = val & 0x70; }
        break;
    default:
        break;
    }

    return true;
}
