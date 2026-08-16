#include "pch.hpp"
#include "MBC.hpp"

#include <algorithm>
#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>

namespace
{
const byte EnableRAM = 0x0A;
const byte RAMBankMode = 0x01;
const unsigned int ROMBankSize = 0x4000;
const unsigned int RAMBankSize = 0x2000;
const unsigned int MBC2RAMSize = 0x0200;
const unsigned long GameBoyClock = 4194304;

bool IsRAMEnableValue(byte val)
{
    return (val & 0x0F) == EnableRAM;
}
}

MBC::MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize) :
    m_ROM(pROM),
    m_RAM(pRAM),
    m_ROMSize(romSize),
    m_RAMSize(ramSize),
    m_ROMBanks(romSize / ROMBankSize),
    m_isRAMEnabled(false)
{
}

MBC::~MBC()
{
}

byte MBC::ReadROM(unsigned int bank, unsigned int offset) const
{
    if (m_ROM == nullptr || m_ROMSize == 0 || m_ROMBanks == 0)
    {
        return 0xFF;
    }

    bank %= m_ROMBanks;
    const unsigned int target = (bank * ROMBankSize) + (offset & (ROMBankSize - 1));
    return target < m_ROMSize ? m_ROM[target] : 0xFF;
}

byte MBC::ReadRAM(unsigned int bank, unsigned int offset) const
{
    if (m_RAM == nullptr || m_RAMSize == 0)
    {
        return 0xFF;
    }

    const unsigned int target = ((bank * RAMBankSize) + offset) % m_RAMSize;
    return m_RAM[target];
}

bool MBC::WriteRAM(unsigned int bank, unsigned int offset, byte val)
{
    if (m_RAM == nullptr || m_RAMSize == 0)
    {
        return false;
    }

    const unsigned int target = ((bank * RAMBankSize) + offset) % m_RAMSize;
    m_RAM[target] = val;
    return true;
}

ROMOnly_MBC::ROMOnly_MBC(byte* pROM, byte* pRAM) :
    MBC(pROM, 0x8000, pRAM, pRAM == nullptr ? 0 : RAMBankSize)
{
}

ROMOnly_MBC::ROMOnly_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize) :
    MBC(pROM, romSize, pRAM, ramSize)
{
}

byte ROMOnly_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x7FFF)
    {
        return ReadROM(address / ROMBankSize, address & 0x3FFF);
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        return ReadRAM(0, address - 0xA000);
    }
    return 0xFF;
}

bool ROMOnly_MBC::WriteByte(const ushort& address, const byte val)
{
    return address >= 0xA000 && address <= 0xBFFF &&
        WriteRAM(0, address - 0xA000, val);
}

MBC1_MBC::MBC1_MBC(byte* pROM, byte* pRAM) :
    MBC1_MBC(pROM, 0x10000, pRAM, pRAM == nullptr ? 0 : 0x8000, false)
{
}

MBC1_MBC::MBC1_MBC(
    byte* pROM,
    unsigned int romSize,
    byte* pRAM,
    unsigned int ramSize,
    bool isMulticart) :
    MBC(pROM, romSize, pRAM, ramSize),
    m_ROMBankLower(0x01),
    m_ROMRAMBankUpper(0x00),
    m_ROMRAMMode(0x00),
    m_IsMulticart(isMulticart)
{
}

unsigned int MBC1_MBC::LowerROMBank() const
{
    if (m_ROMRAMMode != RAMBankMode)
    {
        return 0;
    }
    return static_cast<unsigned int>(m_ROMRAMBankUpper) << (m_IsMulticart ? 4 : 5);
}

unsigned int MBC1_MBC::UpperROMBank() const
{
    const byte lowerMask = m_IsMulticart ? 0x0F : 0x1F;
    byte lower = m_ROMBankLower & lowerMask;
    if (m_ROMBankLower == 0)
    {
        lower = 1;
    }
    return (static_cast<unsigned int>(m_ROMRAMBankUpper) << (m_IsMulticart ? 4 : 5)) | lower;
}

unsigned int MBC1_MBC::RAMBank() const
{
    return m_ROMRAMMode == RAMBankMode ? m_ROMRAMBankUpper : 0;
}

byte MBC1_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        return ReadROM(LowerROMBank(), address);
    }
    if (address <= 0x7FFF)
    {
        return ReadROM(UpperROMBank(), address - 0x4000);
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        return m_isRAMEnabled ? ReadRAM(RAMBank(), address - 0xA000) : 0xFF;
    }
    return 0xFF;
}

bool MBC1_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        m_isRAMEnabled = IsRAMEnableValue(val);
        return true;
    }
    if (address <= 0x3FFF)
    {
        // MBC1M ignores bit 4 for addressing, but still uses the full register
        // when deciding whether bank zero must be translated to bank one.
        m_ROMBankLower = val & 0x1F;
        return true;
    }
    if (address <= 0x5FFF)
    {
        m_ROMRAMBankUpper = val & 0x03;
        return true;
    }
    if (address <= 0x7FFF)
    {
        m_ROMRAMMode = val & 0x01;
        return true;
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        return m_isRAMEnabled && WriteRAM(RAMBank(), address - 0xA000, val);
    }
    return false;
}

MBC2_MBC::MBC2_MBC(byte* pROM) :
    MBC(pROM, 0x10000, nullptr, 0),
    m_ROMBank(0x01),
    m_OwnedRAM(new byte[MBC2RAMSize]())
{
    m_RAM = m_OwnedRAM.get();
    m_RAMSize = MBC2RAMSize;
}

MBC2_MBC::MBC2_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize) :
    MBC(
        pROM,
        romSize,
        ramSize >= MBC2RAMSize ? pRAM : nullptr,
        ramSize >= MBC2RAMSize ? MBC2RAMSize : 0),
    m_ROMBank(0x01)
{
}

byte MBC2_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        return ReadROM(0, address);
    }
    if (address <= 0x7FFF)
    {
        return ReadROM(m_ROMBank, address - 0x4000);
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!m_isRAMEnabled || m_RAM == nullptr || m_RAMSize == 0)
        {
            return 0xFF;
        }
        return 0xF0 | (m_RAM[(address - 0xA000) & 0x01FF] & 0x0F);
    }
    return 0xFF;
}

bool MBC2_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x3FFF)
    {
        if ((address & 0x0100) == 0)
        {
            m_isRAMEnabled = IsRAMEnableValue(val);
        }
        else
        {
            m_ROMBank = val & 0x0F;
            if (m_ROMBank == 0)
            {
                m_ROMBank = 1;
            }
        }
        return true;
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!m_isRAMEnabled || m_RAM == nullptr || m_RAMSize == 0)
        {
            return false;
        }
        m_RAM[(address - 0xA000) & 0x01FF] = val & 0x0F;
        return true;
    }
    return false;
}

MBC3_MBC::MBC3_MBC(byte* pROM, byte* pRAM) :
    MBC3_MBC(pROM, 0x10000, pRAM, pRAM == nullptr ? 0 : 0x8000, true)
{
}

MBC3_MBC::MBC3_MBC(
    byte* pROM,
    unsigned int romSize,
    byte* pRAM,
    unsigned int ramSize,
    bool hasRTC) :
    MBC(pROM, romSize, pRAM, ramSize),
    m_ROMBank(0x01),
    m_RAMRTCSelect(0x00),
    m_LastLatchWrite(0xFF),
    m_HasRTC(hasRTC),
    m_IsMBC30(romSize > 0x200000 || ramSize > 0x8000),
    m_HasLatchedRTC(false),
    m_RTCCycles(0),
    m_LastRTCUpdate(std::time(nullptr))
{
    std::memset(m_RTCRegisters, 0, sizeof(m_RTCRegisters));
    std::memset(m_LatchedRTCRegisters, 0, sizeof(m_LatchedRTCRegisters));
}

void MBC3_MBC::AddSeconds(unsigned long long seconds)
{
    while (seconds != 0 &&
        (m_RTCRegisters[0] > 59 ||
         m_RTCRegisters[1] > 59 ||
         m_RTCRegisters[2] > 23))
    {
        IncrementRTC();
        --seconds;
    }

    if (seconds == 0)
    {
        return;
    }

    unsigned long long day = m_RTCRegisters[3] | ((m_RTCRegisters[4] & 0x01) << 8);
    unsigned long long total =
        m_RTCRegisters[0] +
        (static_cast<unsigned long long>(m_RTCRegisters[1]) * 60) +
        (static_cast<unsigned long long>(m_RTCRegisters[2]) * 3600) +
        seconds;

    day += total / 86400;
    total %= 86400;
    if (day > 0x01FF)
    {
        m_RTCRegisters[4] |= 0x80;
        day %= 0x0200;
    }

    m_RTCRegisters[0] = static_cast<byte>(total % 60);
    total /= 60;
    m_RTCRegisters[1] = static_cast<byte>(total % 60);
    m_RTCRegisters[2] = static_cast<byte>(total / 60);
    m_RTCRegisters[3] = static_cast<byte>(day & 0xFF);
    m_RTCRegisters[4] =
        (m_RTCRegisters[4] & 0xC0) |
        static_cast<byte>((day >> 8) & 0x01);
}

void MBC3_MBC::IncrementRTC()
{
    const byte seconds = m_RTCRegisters[0] & 0x3F;
    if (seconds != 59 && seconds != 63)
    {
        m_RTCRegisters[0] = seconds + 1;
        return;
    }
    m_RTCRegisters[0] = 0;
    if (seconds == 63)
    {
        return;
    }

    const byte minutes = m_RTCRegisters[1] & 0x3F;
    if (minutes != 59 && minutes != 63)
    {
        m_RTCRegisters[1] = minutes + 1;
        return;
    }
    m_RTCRegisters[1] = 0;
    if (minutes == 63)
    {
        return;
    }

    const byte hours = m_RTCRegisters[2] & 0x1F;
    if (hours != 23 && hours != 31)
    {
        m_RTCRegisters[2] = hours + 1;
        return;
    }
    m_RTCRegisters[2] = 0;
    if (hours == 31)
    {
        return;
    }

    unsigned int day = m_RTCRegisters[3] | ((m_RTCRegisters[4] & 0x01) << 8);
    ++day;
    if (day > 0x01FF)
    {
        day = 0;
        m_RTCRegisters[4] |= 0x80;
    }
    m_RTCRegisters[3] = static_cast<byte>(day & 0xFF);
    m_RTCRegisters[4] =
        (m_RTCRegisters[4] & 0xC0) |
        static_cast<byte>((day >> 8) & 0x01);
}

void MBC3_MBC::Step(unsigned long cycles)
{
    if (!m_HasRTC || (m_RTCRegisters[4] & 0x40) != 0)
    {
        return;
    }

    m_RTCCycles += cycles;
    if (m_RTCCycles >= GameBoyClock)
    {
        AddSeconds(m_RTCCycles / GameBoyClock);
        m_RTCCycles %= GameBoyClock;
    }
}

void MBC3_MBC::LatchRTC()
{
    std::memcpy(m_LatchedRTCRegisters, m_RTCRegisters, sizeof(m_RTCRegisters));
    m_HasLatchedRTC = true;
}

byte MBC3_MBC::ReadRTC(byte index)
{
    if (!m_HasRTC || index >= sizeof(m_RTCRegisters))
    {
        return 0xFF;
    }
    return m_HasLatchedRTC ? m_LatchedRTCRegisters[index] : m_RTCRegisters[index];
}

void MBC3_MBC::WriteRTC(byte index, byte val)
{
    if (!m_HasRTC || index >= sizeof(m_RTCRegisters))
    {
        return;
    }

    switch (index)
    {
    case 0:
        m_RTCRegisters[index] = val & 0x3F;
        m_RTCCycles = 0;
        break;
    case 1:
        m_RTCRegisters[index] = val & 0x3F;
        break;
    case 2:
        m_RTCRegisters[index] = val & 0x1F;
        break;
    case 3:
        m_RTCRegisters[index] = val;
        break;
    case 4:
        m_RTCRegisters[index] = val & 0xC1;
        m_LastRTCUpdate = std::time(nullptr);
        break;
    }
}

byte MBC3_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        return ReadROM(0, address);
    }
    if (address <= 0x7FFF)
    {
        return ReadROM(m_ROMBank, address - 0x4000);
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!m_isRAMEnabled)
        {
            return 0xFF;
        }
        if (m_RAMRTCSelect <= (m_IsMBC30 ? 0x07 : 0x03))
        {
            return ReadRAM(m_RAMRTCSelect, address - 0xA000);
        }
        if (m_RAMRTCSelect >= 0x08 && m_RAMRTCSelect <= 0x0C)
        {
            return ReadRTC(m_RAMRTCSelect - 0x08);
        }
        return 0xFF;
    }
    return 0xFF;
}

bool MBC3_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        m_isRAMEnabled = IsRAMEnableValue(val);
        return true;
    }
    if (address <= 0x3FFF)
    {
        m_ROMBank = m_IsMBC30 ? val : val & 0x7F;
        if (m_ROMBank == 0)
        {
            m_ROMBank = 1;
        }
        return true;
    }
    if (address <= 0x5FFF)
    {
        m_RAMRTCSelect = val;
        return true;
    }
    if (address <= 0x7FFF)
    {
        if (m_HasRTC && m_LastLatchWrite == 0 && val == 1)
        {
            LatchRTC();
        }
        m_LastLatchWrite = val;
        return true;
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!m_isRAMEnabled)
        {
            return false;
        }
        if (m_RAMRTCSelect <= (m_IsMBC30 ? 0x07 : 0x03))
        {
            return WriteRAM(m_RAMRTCSelect, address - 0xA000, val);
        }
        if (m_RAMRTCSelect >= 0x08 && m_RAMRTCSelect <= 0x0C && m_HasRTC)
        {
            WriteRTC(m_RAMRTCSelect - 0x08, val);
            return true;
        }
    }
    return false;
}

bool MBC3_MBC::LoadRTC(std::istream& stream)
{
    if (!m_HasRTC)
    {
        return false;
    }

    char magic[6] = {};
    byte registers[5] = {};
    byte timestampBytes[8] = {};
    if (!stream.read(magic, sizeof(magic)) ||
        std::memcmp(magic, "GLRTC1", sizeof(magic)) != 0 ||
        !stream.read(reinterpret_cast<char*>(registers), sizeof(registers)) ||
        !stream.read(reinterpret_cast<char*>(timestampBytes), sizeof(timestampBytes)))
    {
        return false;
    }

    std::uint64_t timestamp = 0;
    for (unsigned int index = 0; index < sizeof(timestampBytes); ++index)
    {
        timestamp |= static_cast<std::uint64_t>(timestampBytes[index]) << (index * 8);
    }

    const std::uint64_t maxTimestamp =
        static_cast<std::uint64_t>((std::numeric_limits<std::time_t>::max)());
    const std::time_t now = std::time(nullptr);
    if (timestamp > maxTimestamp || now == static_cast<std::time_t>(-1))
    {
        return false;
    }

    std::memcpy(m_RTCRegisters, registers, sizeof(m_RTCRegisters));
    m_RTCRegisters[0] &= 0x3F;
    m_RTCRegisters[1] &= 0x3F;
    m_RTCRegisters[2] &= 0x1F;
    m_RTCRegisters[4] &= 0xC1;
    m_LastRTCUpdate = static_cast<std::time_t>(timestamp);
    if ((m_RTCRegisters[4] & 0x40) == 0 && now > m_LastRTCUpdate)
    {
        AddSeconds(static_cast<unsigned long long>(now - m_LastRTCUpdate));
    }
    m_LastRTCUpdate = now;
    m_RTCCycles = 0;
    m_HasLatchedRTC = false;
    return true;
}

bool MBC3_MBC::SaveRTC(std::ostream& stream)
{
    if (!m_HasRTC)
    {
        return false;
    }

    m_LastRTCUpdate = std::time(nullptr);
    if (m_LastRTCUpdate == static_cast<std::time_t>(-1))
    {
        return false;
    }
    byte timestampBytes[8] = {};
    const std::uint64_t timestamp = static_cast<std::uint64_t>(m_LastRTCUpdate);
    for (unsigned int index = 0; index < sizeof(timestampBytes); ++index)
    {
        timestampBytes[index] = static_cast<byte>((timestamp >> (index * 8)) & 0xFF);
    }

    stream.write("GLRTC1", 6);
    stream.write(reinterpret_cast<const char*>(m_RTCRegisters), sizeof(m_RTCRegisters));
    stream.write(reinterpret_cast<const char*>(timestampBytes), sizeof(timestampBytes));
    return stream.good();
}

MBC5_MBC::MBC5_MBC(byte* pROM, byte* pRAM) :
    MBC5_MBC(pROM, 0x10000, pRAM, pRAM == nullptr ? 0 : 0x8000, false)
{
}

MBC5_MBC::MBC5_MBC(
    byte* pROM,
    unsigned int romSize,
    byte* pRAM,
    unsigned int ramSize,
    bool hasRumble) :
    MBC(pROM, romSize, pRAM, ramSize),
    m_ROMBank(0x0001),
    m_RAMBank(0x00),
    m_HasRumble(hasRumble),
    m_RumbleEnabled(false)
{
}

byte MBC5_MBC::ReadByte(const ushort& address)
{
    if (address <= 0x3FFF)
    {
        return ReadROM(0, address);
    }
    if (address <= 0x7FFF)
    {
        return ReadROM(m_ROMBank, address - 0x4000);
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        return m_isRAMEnabled ? ReadRAM(m_RAMBank, address - 0xA000) : 0xFF;
    }
    return 0xFF;
}

bool MBC5_MBC::WriteByte(const ushort& address, const byte val)
{
    if (address <= 0x1FFF)
    {
        m_isRAMEnabled = IsRAMEnableValue(val);
        return true;
    }
    if (address <= 0x2FFF)
    {
        m_ROMBank = (m_ROMBank & 0x0100) | val;
        return true;
    }
    if (address <= 0x3FFF)
    {
        m_ROMBank = (m_ROMBank & 0x00FF) | ((val & 0x01) << 8);
        return true;
    }
    if (address <= 0x5FFF)
    {
        m_RumbleEnabled = m_HasRumble && (val & 0x08) != 0;
        m_RAMBank = val & (m_HasRumble ? 0x07 : 0x0F);
        return true;
    }
    if (address <= 0x7FFF)
    {
        return true;
    }
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        return m_isRAMEnabled && WriteRAM(m_RAMBank, address - 0xA000, val);
    }
    return false;
}
