#pragma once

#define MBC1                0x01
#define MBC1RAM             0x02
#define MBC1RAMBattery      0x03

#define MBC2                0x05
#define MBC2Battery         0x06

#define MBC3TimerBattery    0x0F
#define MBC3TimerRAMBattery 0x10
#define MBC3                0x11
#define MBC3RAM             0x12
#define MBC3RAMBattery      0x13

class MBC : public IMemoryUnit
{
public:
    // IMemoryUnit
    virtual byte ReadByte(const ushort& address) = 0;
    virtual bool WriteByte(const ushort& address, const byte val) = 0;
};

class MBC1_MBC : public MBC
{
public:
    MBC1_MBC(byte* pROM, byte* pRAM);
    ~MBC1_MBC();
    
    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
    byte* m_RAM;
    byte m_ROMBankLower;
    byte m_ROMRAMBankUpper;
    byte m_ROMRAMMode;

    bool m_isRAMEnabled;
};

class MBC2_MBC : public MBC
{
public:
    MBC2_MBC(byte* pROM);
    ~MBC2_MBC();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
    byte m_RAM[0x1FF + 1];
    byte m_ROMBank;

    bool m_isRAMEnabled;
};

class MBC3_MBC : public MBC
{
public:
    MBC3_MBC(byte* pROM, byte* pRAM);
    ~MBC3_MBC();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
    byte* m_RAM;

    byte m_ROMBank;
    byte m_RAMBank;
    byte m_RTCRegisters[0x05];

    bool m_isRAMEnabled;
};
