#pragma once
#include "PCH.hpp"

class Cartridge : public IMemoryUnit
{
public:
    Cartridge();
    ~Cartridge();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    ushort ReadUShort(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);
};
