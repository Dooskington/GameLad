#pragma once

class IMemoryUnit
{
public:
    virtual ~IMemoryUnit() {}
    virtual byte ReadByte(const ushort& address) = 0;
    virtual bool WriteByte(const ushort& address, const byte val) = 0;
};
