#pragma once

class IMMU : public IMemoryUnit
{
public:
    virtual ~IMMU() {}
    virtual bool Initialize() = 0;
    virtual void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit) = 0;
    virtual unsigned short ReadUShort(const ushort& address) = 0;
};
