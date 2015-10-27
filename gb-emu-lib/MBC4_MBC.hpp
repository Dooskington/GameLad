#pragma once

#define MBC4                0x15
#define MBC4RAM             0x16
#define MBC4RAMBattery      0x17

class MBC4_MBC : public IMemoryUnit
{
public:
    MBC4_MBC(byte* pROM, byte* pRAM);
    ~MBC4_MBC();
    
    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
    byte* m_RAM;
};
