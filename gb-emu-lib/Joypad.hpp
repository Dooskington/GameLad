#pragma once

class Joypad : public IMemoryUnit
{
public:
    Joypad();
    ~Joypad();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);
};
