#pragma once

#define ROMOnly             0x00

class ROMOnly_MBC : public IMemoryUnit
{
public:
    ROMOnly_MBC(byte* data);
    ~ROMOnly_MBC();
    
    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
};

