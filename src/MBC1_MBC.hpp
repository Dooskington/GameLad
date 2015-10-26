#pragma once

#define MBC1                0x01
#define MBC1RAM             0x02
#define MBC1RAMBattery      0x03

class MBC1_MBC : public IMemoryUnit
{
public:
    MBC1_MBC(byte mbcType, byte* data);
    ~MBC1_MBC();
    
    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_mbcType;
    byte* m_ROM;
    byte m_ROMBankLower;
    byte m_ROMRAMBankUpper;
    byte m_ROMRAMMode;

    bool m_isRAMEnabled;
    byte m_RAM[0x7FFF + 1]; // 32KB, 4 banks of 8KB each
};

