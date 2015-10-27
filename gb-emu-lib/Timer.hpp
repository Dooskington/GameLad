#pragma once

class Timer : public IMemoryUnit
{
public:
    Timer();
    ~Timer();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_memory[0xFFFF + 1];
};
