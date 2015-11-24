#pragma once

#define ROMOnly             0x00

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

#define MBC5                0x19
#define MBC5RAM             0x1A
#define MBC5RAMBattery      0x1B
#define MBC5Rumble          0x1C
#define MBC5RumbleRAM       0x1D
#define MBC5RumbleRAMBattery 0x1E

class MBC : public IMemoryUnit
{
public:
    MBC(byte* pROM, byte* pRAM);
    ~MBC();

public:
    // IMemoryUnit
    virtual byte ReadByte(const ushort& address) = 0;
    virtual bool WriteByte(const ushort& address, const byte val) = 0;
    
protected:
    byte* m_ROM;
    byte* m_RAM;
    bool m_isRAMEnabled;
};

class ROMOnly_MBC : public MBC
{
public:
    ROMOnly_MBC(byte* pROM, byte* pRAM);
    ~ROMOnly_MBC();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);
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
    byte m_ROMBankLower;
    byte m_ROMRAMBankUpper;
    byte m_ROMRAMMode;
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
    byte m_ROMBank;
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
    byte m_ROMBank;
    byte m_RAMBank;
    byte m_RTCRegisters[0x05];
};

class MBC5_MBC : public MBC
{
public:
    MBC5_MBC(byte* pROM, byte* pRAM);
    ~MBC5_MBC();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_RAMG;

    ushort m_ROMBank;
    byte m_RAMBank;
};
