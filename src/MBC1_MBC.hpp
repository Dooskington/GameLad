#pragma once

#define MBC1                0x01
#define MBC1RAM             0x02
#define MBC1RAMBattery      0x03

class MBC1_MBC : public IMemoryUnit
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

