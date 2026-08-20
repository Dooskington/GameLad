#include "pch.hpp"
#include "Cartridge.hpp"

#include "MBC.hpp"

#include <limits>

namespace
{
bool DecodeROMSize(byte flag, unsigned int& size)
{
    switch (flag)
    {
    case ROM_32KB:  size = 32 * 1024; return true;
    case ROM_64KB:  size = 64 * 1024; return true;
    case ROM_128KB: size = 128 * 1024; return true;
    case ROM_256KB: size = 256 * 1024; return true;
    case ROM_512KB: size = 512 * 1024; return true;
    case ROM_1MB:   size = 1024 * 1024; return true;
    case ROM_2MB:   size = 2 * 1024 * 1024; return true;
    case ROM_4MB:   size = 4 * 1024 * 1024; return true;
    case ROM_8MB:   size = 8 * 1024 * 1024; return true;
    case ROM_1_1MB: size = 72 * 16 * 1024; return true;
    case ROM_1_2MB: size = 80 * 16 * 1024; return true;
    case ROM_1_5MB: size = 96 * 16 * 1024; return true;
    default: return false;
    }
}

bool DecodeRAMSize(byte flag, unsigned int& size)
{
    switch (flag)
    {
    case RAM_None:  size = 0; return true;
    case RAM_2KB:   size = 2 * 1024; return true;
    case RAM_8KB:   size = 8 * 1024; return true;
    case RAM_32KB:  size = 32 * 1024; return true;
    case RAM_128KB: size = 128 * 1024; return true;
    case RAM_64KB:  size = 64 * 1024; return true;
    default: return false;
    }
}

bool IsSupportedPhysicalROMSize(unsigned int size)
{
    unsigned int decodedSize = 0;
    const byte flags[] =
    {
        ROM_32KB, ROM_64KB, ROM_128KB, ROM_256KB, ROM_512KB,
        ROM_1MB, ROM_2MB, ROM_4MB, ROM_8MB, ROM_1_1MB, ROM_1_2MB, ROM_1_5MB
    };
    for (unsigned int index = 0; index < sizeof(flags); ++index)
    {
        if (DecodeROMSize(flags[index], decodedSize) && decodedSize == size)
        {
            return true;
        }
    }
    return false;
}

bool CartridgeTypeHasRAM(byte type)
{
    switch (type)
    {
    case ROMRAM:
    case ROMRAMBattery:
    case MBC1RAM:
    case MBC1RAMBattery:
    case MBC2:
    case MBC2Battery:
    case MBC3TimerRAMBattery:
    case MBC3RAM:
    case MBC3RAMBattery:
    case MBC5RAM:
    case MBC5RAMBattery:
    case MBC5RumbleRAM:
    case MBC5RumbleRAMBattery:
        return true;
    default:
        return false;
    }
}

bool CartridgeTypeHasBattery(byte type)
{
    switch (type)
    {
    case ROMRAMBattery:
    case MBC1RAMBattery:
    case MBC2Battery:
    case MBC3TimerBattery:
    case MBC3TimerRAMBattery:
    case MBC3RAMBattery:
    case MBC5RAMBattery:
    case MBC5RumbleRAMBattery:
        return true;
    default:
        return false;
    }
}

bool CartridgeTypeHasRTC(byte type)
{
    return type == MBC3TimerBattery || type == MBC3TimerRAMBattery;
}
}

Cartridge::Cartridge() :
    m_MBCType(ROMOnly),
    m_ROMSize(0),
    m_RAMSize(0),
    m_HasBattery(false),
    m_HasRTC(false),
    m_ManagePersistentData(false),
    m_ROMHash(0)
{
}

Cartridge::~Cartridge()
{
    SavePersistentData();
}

bool Cartridge::LoadROM(const char* path)
{
    if (path == nullptr || path[0] == '\0')
    {
        Logger::LogError("Cartridge::LoadROM - Invalid ROM path.");
        return false;
    }

    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        Logger::LogError("Failed to open game ROM %s", path);
        return false;
    }

    const std::streampos streamSize = file.tellg();
    if (streamSize < static_cast<std::streampos>(0x0150) ||
        streamSize > static_cast<std::streampos>(std::numeric_limits<unsigned int>::max()))
    {
        Logger::LogError("Cartridge ROM has an invalid size: %s", path);
        return false;
    }

    const unsigned int actualSize = static_cast<unsigned int>(streamSize);
    std::unique_ptr<byte[]> rom(new byte[actualSize]);
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(rom.get()), actualSize))
    {
        Logger::LogError("Failed to read game ROM %s", path);
        return false;
    }

    return LoadROM(rom.get(), actualSize, path, true);
}

bool Cartridge::LoadROM(
    const byte* data,
    size_t size,
    const char* persistencePath,
    bool managePersistentData)
{
    if (data == nullptr ||
        size < 0x0150 ||
        size > static_cast<size_t>((std::numeric_limits<unsigned int>::max)()))
    {
        Logger::LogError("Cartridge::LoadROM - Invalid ROM data.");
        return false;
    }

    SavePersistentData();
    m_MBC.reset();
    m_ROM.reset();
    m_RAM.reset();
    m_ROMSize = 0;
    m_RAMSize = 0;
    m_HasBattery = false;
    m_HasRTC = false;
    m_ManagePersistentData = managePersistentData;
    m_ROMHash = 0;
    m_ROMPatches.clear();
    m_Path = persistencePath == nullptr ? "" : persistencePath;

    const unsigned int actualSize = static_cast<unsigned int>(size);
    m_ROM.reset(new byte[actualSize]);
    std::memcpy(m_ROM.get(), data, actualSize);
    m_ROMSize = actualSize;
    m_ROMHash = GetROMHash();
    if (!LoadMBC(actualSize))
    {
        m_MBC.reset();
        m_ROM.reset();
        m_RAM.reset();
        m_ROMSize = 0;
        m_RAMSize = 0;
        m_HasBattery = false;
        m_HasRTC = false;
        m_ManagePersistentData = false;
        m_ROMHash = 0;
        m_Path.clear();
        return false;
    }

    Logger::Log(
        "Loaded game ROM %s (%u bytes)",
        m_Path.empty() ? "<memory>" : m_Path.c_str(),
        actualSize);
    if (m_ManagePersistentData)
    {
        LoadPersistentData();
    }
    return true;
}

byte Cartridge::ReadByte(const ushort& address)
{
    const byte value = m_MBC == nullptr ? 0xFF : m_MBC->ReadByte(address);
    for (size_t index = m_ROMPatches.size(); index != 0; --index)
    {
        const ROMPatch& patch = m_ROMPatches[index - 1];
        if (address == patch.Address &&
            (patch.CompareValue < 0 ||
             value == static_cast<byte>(patch.CompareValue)))
        {
            return patch.Value;
        }
    }
    return value;
}

bool Cartridge::WriteByte(const ushort& address, const byte val)
{
    return m_MBC != nullptr && m_MBC->WriteByte(address, val);
}

void Cartridge::Step(unsigned long cycles)
{
    MBC3_MBC* mbc3 = dynamic_cast<MBC3_MBC*>(m_MBC.get());
    if (mbc3 != nullptr)
    {
        mbc3->Step(cycles);
    }
}

unsigned long long Cartridge::GetROMHash() const
{
    if (m_ROMHash != 0)
    {
        return m_ROMHash;
    }

    const unsigned long long offsetBasis = 1469598103934665603ULL;
    const unsigned long long prime = 1099511628211ULL;
    unsigned long long hash = offsetBasis;
    for (unsigned int index = 0; index < m_ROMSize; ++index)
    {
        hash ^= m_ROM[index];
        hash *= prime;
    }
    return hash;
}

void Cartridge::ClearROMPatches()
{
    m_ROMPatches.clear();
}

void Cartridge::ApplyROMPatch(byte value, ushort address, int compareValue)
{
    if (m_ROM == nullptr || address >= 0x8000)
    {
        return;
    }

    ROMPatch patch = { value, address, compareValue };
    m_ROMPatches.push_back(patch);
}

byte* Cartridge::GetRTCData()
{
    MBC* mbc = dynamic_cast<MBC*>(m_MBC.get());
    return mbc == nullptr ? nullptr : mbc->GetRTCData();
}

size_t Cartridge::GetRTCDataSize() const
{
    const MBC* mbc = dynamic_cast<const MBC*>(m_MBC.get());
    return mbc == nullptr ? 0 : mbc->GetRTCDataSize();
}

bool Cartridge::IsRumbleEnabled() const
{
    const MBC* mbc = dynamic_cast<const MBC*>(m_MBC.get());
    return mbc != nullptr && mbc->IsRumbleEnabled();
}

void Cartridge::Serialize(StateSerializer& state)
{
    unsigned int romSize = m_ROMSize;
    unsigned int ramSize = m_RAMSize;
    byte mbcType = m_MBCType;
    bool hasBattery = m_HasBattery;
    bool hasRTC = m_HasRTC;
    unsigned long long romHash = GetROMHash();

    state.Sync(romSize);
    state.Sync(ramSize);
    state.Sync(mbcType);
    state.Sync(hasBattery);
    state.Sync(hasRTC);
    state.Sync(romHash);

    if (state.IsReading() &&
        (romSize != m_ROMSize ||
         ramSize != m_RAMSize ||
         mbcType != m_MBCType ||
         hasBattery != m_HasBattery ||
         hasRTC != m_HasRTC ||
         romHash != GetROMHash()))
    {
        state.Invalidate();
        return;
    }

    state.SyncBytes(m_RAM.get(), m_RAMSize);
    if (m_MBC == nullptr)
    {
        state.Invalidate();
        return;
    }
    static_cast<MBC*>(m_MBC.get())->Serialize(state);
}

byte Cartridge::GetCGBFlag() const
{
    if (m_ROM == nullptr || m_ROMSize <= CGBFlagAddress)
    {
        return 0x00;
    }

    return m_ROM[CGBFlagAddress];
}

bool Cartridge::IsCGBCartridge() const
{
    return IsCGBCartridgeFlag(GetCGBFlag());
}

bool Cartridge::IsCGBOnlyCartridge() const
{
    return IsCGBOnlyCartridgeFlag(GetCGBFlag());
}

bool Cartridge::IsMBC1Multicart() const
{
    const unsigned int logoOffset = 0x0104;
    const unsigned int logoSize = 0x30;
    const unsigned int secondHeader = (0x10 * 0x4000) + logoOffset;
    return m_ROM != nullptr &&
        m_ROMSize >= secondHeader + logoSize &&
        std::memcmp(m_ROM.get() + logoOffset, m_ROM.get() + secondHeader, logoSize) == 0;
}

bool Cartridge::LoadMBC(unsigned int actualSize)
{
    m_MBCType = m_ROM[CartridgeTypeAddress];
    const byte romSizeFlag = m_ROM[ROMSizeAddress];
    const byte ramSizeFlag = m_ROM[RAMSizeAddress];

    if (!DecodeROMSize(romSizeFlag, m_ROMSize))
    {
        Logger::LogError("Cartridge::LoadMBC - Unsupported ROM size flag: 0x%02X", romSizeFlag);
        return false;
    }
    if (actualSize != m_ROMSize)
    {
        if (!IsSupportedPhysicalROMSize(actualSize))
        {
            Logger::LogError(
                "Cartridge::LoadMBC - ROM size mismatch. Got: %u Expected: %u",
                actualSize,
                m_ROMSize);
            return false;
        }
        Logger::Log(
            "Cartridge::LoadMBC - Header ROM size differs from physical image. Got: %u Header: %u",
            actualSize,
            m_ROMSize);
        m_ROMSize = actualSize;
    }

    unsigned int declaredRAMSize = 0;
    if (!DecodeRAMSize(ramSizeFlag, declaredRAMSize))
    {
        Logger::LogError("Cartridge::LoadMBC - Unsupported RAM size flag: 0x%02X", ramSizeFlag);
        return false;
    }

    m_HasBattery = CartridgeTypeHasBattery(m_MBCType);
    m_HasRTC = CartridgeTypeHasRTC(m_MBCType);
    m_RAMSize = CartridgeTypeHasRAM(m_MBCType) ? declaredRAMSize : 0;
    if (m_MBCType == MBC2 || m_MBCType == MBC2Battery)
    {
        m_RAMSize = 0x0200;
    }
    if (m_RAMSize != 0)
    {
        m_RAM.reset(new byte[m_RAMSize]());
    }

    switch (m_MBCType)
    {
    case ROMOnly:
    case ROMRAM:
    case ROMRAMBattery:
        m_MBC.reset(new ROMOnly_MBC(m_ROM.get(), m_ROMSize, m_RAM.get(), m_RAMSize));
        break;
    case MBC1:
    case MBC1RAM:
    case MBC1RAMBattery:
        m_MBC.reset(new MBC1_MBC(
            m_ROM.get(),
            m_ROMSize,
            m_RAM.get(),
            m_RAMSize,
            IsMBC1Multicart()));
        break;
    case MBC2:
    case MBC2Battery:
        m_MBC.reset(new MBC2_MBC(m_ROM.get(), m_ROMSize, m_RAM.get(), m_RAMSize));
        break;
    case MBC3TimerBattery:
    case MBC3TimerRAMBattery:
    case MBC3:
    case MBC3RAM:
    case MBC3RAMBattery:
        m_MBC.reset(new MBC3_MBC(
            m_ROM.get(),
            m_ROMSize,
            m_RAM.get(),
            m_RAMSize,
            m_HasRTC));
        break;
    case MBC5:
    case MBC5RAM:
    case MBC5RAMBattery:
    case MBC5Rumble:
    case MBC5RumbleRAM:
    case MBC5RumbleRAMBattery:
        m_MBC.reset(new MBC5_MBC(
            m_ROM.get(),
            m_ROMSize,
            m_RAM.get(),
            m_RAMSize,
            m_MBCType == MBC5Rumble ||
                m_MBCType == MBC5RumbleRAM ||
                m_MBCType == MBC5RumbleRAMBattery));
        break;
    default:
        Logger::LogError("Unsupported Cartridge MBC type: 0x%02X", m_MBCType);
        return false;
    }

    return m_MBC != nullptr;
}

void Cartridge::LoadPersistentData()
{
    if (!m_ManagePersistentData || !m_HasBattery)
    {
        return;
    }

    if (m_RAM != nullptr && m_RAMSize != 0)
    {
        const std::string ramPath = m_Path + "_RAM";
        std::ifstream ramFile(ramPath, std::ios::in | std::ios::binary | std::ios::ate);
        if (ramFile.is_open())
        {
            const std::streampos size = ramFile.tellg();
            if (size == static_cast<std::streampos>(m_RAMSize))
            {
                ramFile.seekg(0, std::ios::beg);
                if (!ramFile.read(reinterpret_cast<char*>(m_RAM.get()), m_RAMSize))
                {
                    std::memset(m_RAM.get(), 0, m_RAMSize);
                    Logger::LogError("Failed to load saved RAM %s", ramPath.c_str());
                }
            }
            else
            {
                Logger::LogError(
                    "Saved RAM has the wrong size. Got: %lld Expected: %u",
                    static_cast<long long>(size),
                    m_RAMSize);
            }
        }
    }

    if (m_HasRTC)
    {
        MBC3_MBC* mbc3 = dynamic_cast<MBC3_MBC*>(m_MBC.get());
        std::ifstream rtcFile(m_Path + "_RTC", std::ios::in | std::ios::binary);
        if (mbc3 != nullptr && rtcFile.is_open() && !mbc3->LoadRTC(rtcFile))
        {
            Logger::LogError("Failed to load saved RTC %s", (m_Path + "_RTC").c_str());
        }
    }
}

void Cartridge::SavePersistentData()
{
    if (!m_ManagePersistentData ||
        !m_HasBattery ||
        m_Path.empty() ||
        m_MBC == nullptr)
    {
        return;
    }

    if (m_RAM != nullptr && m_RAMSize != 0)
    {
        const std::string ramPath = m_Path + "_RAM";
        std::ofstream ramFile(ramPath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ramFile.is_open() ||
            !ramFile.write(reinterpret_cast<const char*>(m_RAM.get()), m_RAMSize))
        {
            Logger::LogError("Failed to save cartridge RAM %s", ramPath.c_str());
        }
    }

    if (m_HasRTC)
    {
        MBC3_MBC* mbc3 = dynamic_cast<MBC3_MBC*>(m_MBC.get());
        const std::string rtcPath = m_Path + "_RTC";
        std::ofstream rtcFile(rtcPath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (mbc3 == nullptr || !rtcFile.is_open() || !mbc3->SaveRTC(rtcFile))
        {
            Logger::LogError("Failed to save cartridge RTC %s", rtcPath.c_str());
        }
    }
}
