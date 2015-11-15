#pragma once

class MMU : public IMMU
{
public:
    MMU();
    ~MMU();

    void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit);
    unsigned short ReadUShort(const ushort& address);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte ReadByteInternal(const ushort& address);
    bool WriteByteInternal(const ushort& address, const byte val);

private:
    // Booting
    byte m_isBooting;

    // Memory
    IMemoryUnit* m_memoryUnits[0xFFFF + 1];
    byte m_bank0[0x0FFF + 1];   // 4k work RAM Bank 0
    byte m_bank1[0x0FFF + 1];   // 4k work RAM Bank 1
    byte m_HRAM[0x007E + 1];    // HRAM
};
