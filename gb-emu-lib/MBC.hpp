#pragma once

#include <cstddef>
#include <ctime>
#include <iosfwd>
#include <memory>

#define ROMOnly                 0x00
#define ROMRAM                  0x08
#define ROMRAMBattery           0x09

#define MBC1                    0x01
#define MBC1RAM                 0x02
#define MBC1RAMBattery          0x03

#define MBC2                    0x05
#define MBC2Battery             0x06

#define MBC3TimerBattery        0x0F
#define MBC3TimerRAMBattery     0x10
#define MBC3                    0x11
#define MBC3RAM                 0x12
#define MBC3RAMBattery          0x13

#define MBC5                    0x19
#define MBC5RAM                 0x1A
#define MBC5RAMBattery          0x1B
#define MBC5Rumble              0x1C
#define MBC5RumbleRAM           0x1D
#define MBC5RumbleRAMBattery    0x1E

class MBC : public IMemoryUnit
{
public:
    MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize);
    virtual ~MBC();

protected:
    byte ReadROM(unsigned int bank, unsigned int offset) const;
    byte ReadRAM(unsigned int bank, unsigned int offset) const;
    bool WriteRAM(unsigned int bank, unsigned int offset, byte val);

    byte* m_ROM;
    byte* m_RAM;
    unsigned int m_ROMSize;
    unsigned int m_RAMSize;
    unsigned int m_ROMBanks;
    bool m_isRAMEnabled;
};

class ROMOnly_MBC : public MBC
{
public:
    ROMOnly_MBC(byte* pROM, byte* pRAM);
    ROMOnly_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize);

    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);
};

class MBC1_MBC : public MBC
{
public:
    MBC1_MBC(byte* pROM, byte* pRAM);
    MBC1_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize, bool isMulticart);

    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    unsigned int LowerROMBank() const;
    unsigned int UpperROMBank() const;
    unsigned int RAMBank() const;

    byte m_ROMBankLower;
    byte m_ROMRAMBankUpper;
    byte m_ROMRAMMode;
    bool m_IsMulticart;
};

class MBC2_MBC : public MBC
{
public:
    explicit MBC2_MBC(byte* pROM);
    MBC2_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize);

    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_ROMBank;
    std::unique_ptr<byte[]> m_OwnedRAM;
};

class MBC3_MBC : public MBC
{
public:
    MBC3_MBC(byte* pROM, byte* pRAM);
    MBC3_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize, bool hasRTC);

    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

    void Step(unsigned long cycles);
    bool LoadRTC(std::istream& stream);
    bool SaveRTC(std::ostream& stream);

private:
    void AddSeconds(unsigned long long seconds);
    void IncrementRTC();
    void LatchRTC();
    byte ReadRTC(byte index);
    void WriteRTC(byte index, byte val);

    byte m_ROMBank;
    byte m_RAMRTCSelect;
    byte m_RTCRegisters[0x05];
    byte m_LatchedRTCRegisters[0x05];
    byte m_LastLatchWrite;
    bool m_HasRTC;
    bool m_IsMBC30;
    bool m_HasLatchedRTC;
    unsigned long m_RTCCycles;
    std::time_t m_LastRTCUpdate;
};

class MBC5_MBC : public MBC
{
public:
    MBC5_MBC(byte* pROM, byte* pRAM);
    MBC5_MBC(byte* pROM, unsigned int romSize, byte* pRAM, unsigned int ramSize, bool hasRumble);

    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    ushort m_ROMBank;
    byte m_RAMBank;
    bool m_HasRumble;
    bool m_RumbleEnabled;
};
