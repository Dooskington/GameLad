#pragma once

class APU : public IMemoryUnit
{
public:
    APU();
    ~APU();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte m_SoundOnOffRegister;
};
