#pragma once

#include "PCH.hpp"

class MMU : public IMemoryUnit
{
public:
    MMU();
    ~MMU();

    bool Initialize();
    void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    unsigned short ReadUShort(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    bool LoadBootRom(std::string path);

    byte ReadByteInternal(const ushort& address);
    unsigned short ReadUShortInternal(const ushort& address);
    bool WriteByteInternal(const ushort& address, const byte val);

private:
    // Booting
    bool m_isBooting;
    byte m_bios[0xFF + 1]; // The boot rom, loaded from bios.bin

    // Memory
    IMemoryUnit* m_memoryUnits[0xFFFF + 1];
    byte m_memory[0xFFFF + 1]; // 64k array that will act as memory
};
