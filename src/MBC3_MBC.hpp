#pragma once

#define MBC3TimerBattery    0x0F
#define MBC3TimerRAMBattery 0x10
#define MBC3                0x11
#define MBC3RAM             0x12
#define MBC3RAMBattery      0x13

class MBC3_MBC : public IMemoryUnit
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
};
