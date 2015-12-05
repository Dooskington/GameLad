#pragma once

class IMMU
{
public:
    virtual ~IMMU() {}
    virtual void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit) = 0;
    virtual unsigned short ReadUShort(const ushort& address) = 0;
    virtual bool LoadBootROM(const char* bootROMPath) = 0;

    virtual byte Read(const ushort& address) = 0;
    virtual bool Write(const ushort& address, const byte val) = 0;
};
