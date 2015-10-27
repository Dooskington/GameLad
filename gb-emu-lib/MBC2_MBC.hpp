#pragma once

#define MBC2                0x05
#define MBC2Battery         0x06

class MBC2_MBC : public IMemoryUnit
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
