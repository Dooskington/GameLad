#pragma once
/*
Bit 7 - Not used
Bit 6 - Not used
Bit 5 - P15 Select Button Keys      (0=Select)
Bit 4 - P14 Select Direction Keys   (0=Select)
Bit 3 - P13 Input Down  or Start    (0=Pressed) (Read Only)
Bit 2 - P12 Input Up    or Select   (0=Pressed) (Read Only)
Bit 1 - P11 Input Left  or Button B (0=Pressed) (Read Only)
Bit 0 - P10 Input Right or Button A (0=Pressed) (Read Only)
*/

// FF00 - P1/JOYP - Joypad (R/W)
#define JoypadAddress 0xFF00

class Joypad : public IMemoryUnit
{
public:
    Joypad(ICPU* pCPU);
    ~Joypad();

    void SetInput(byte input, byte buttons);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    ICPU* m_CPU;

    byte m_SelectValues;
    byte m_InputValues;
    byte m_ButtonValues;
};
