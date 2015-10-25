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
    byte m_memory[0xFFFF + 1];
};
