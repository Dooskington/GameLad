#pragma once

#define MBC5                0x19
#define MBC5RAM             0x1A
#define MBC5RAMBattery      0x1B
#define MBC5Rumble          0x1C
#define MBC5RumbleRAM       0x1D
#define MBC5RumbleRAMBattery 0x1E

class MBC5_MBC : public IMemoryUnit
{
public:
    MBC5_MBC(byte* pROM, byte* pRAM);
    ~MBC5_MBC();
    
    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
    byte* m_RAM;
};
