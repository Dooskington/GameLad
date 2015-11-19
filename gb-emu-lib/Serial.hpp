#pragma once

class Serial : public IMemoryUnit
{
public:
    Serial();
    ~Serial();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_Data;
};
