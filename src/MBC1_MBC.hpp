#pragma once

#define MBC1                0x01

class MBC1_MBC : public IMemoryUnit
{
public:
    MBC1_MBC(byte* data);
    ~MBC1_MBC();
    
    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte* m_ROM;
};

