#pragma once

#include "PCH.hpp"

class MMU : public IMemoryUnit
{
public:
    MMU();
    ~MMU();

    bool Initialize();
    void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit);
    unsigned short ReadUShort(const ushort& address);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    bool LoadBootRom(std::string path);

    byte ReadByteInternal(const ushort& address);
    bool WriteByteInternal(const ushort& address, const byte val);

private:
    // Booting
    bool m_isBooting;
    byte m_bios[0xFF + 1]; // The boot rom, loaded from bios.bin

    // Memory
    IMemoryUnit* m_memoryUnits[0xFFFF + 1];
    byte m_bank0[0x0FFF + 1];   // 4k work RAM Bank 0
    byte m_bank1[0x0FFF + 1];   // 4k work RAM Bank 1
    byte m_HRAM[0x007E + 1];    // HRAM
};
