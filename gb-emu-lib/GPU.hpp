#pragma once

class GPU : public IMemoryUnit
{
public:
    GPU();
    ~GPU();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_VRAM[0x1FFF + 1];
    byte m_OAM[0x009F + 1];
};
