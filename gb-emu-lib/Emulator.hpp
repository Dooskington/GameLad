#pragma once

#include "ICPU.hpp"

#define JOYPAD_NONE             0

#define JOYPAD_INPUT_DOWN       1 << 3
#define JOYPAD_INPUT_UP         1 << 2
#define JOYPAD_INPUT_LEFT       1 << 1
#define JOYPAD_INPUT_RIGHT      1 << 0

#define JOYPAD_BUTTONS_START    1 << 3
#define JOYPAD_BUTTONS_SELECT   1 << 2
#define JOYPAD_BUTTONS_B        1 << 1
#define JOYPAD_BUTTONS_A        1 << 0

class Emulator
{
public:
    Emulator();

    void StepFrame();
    void Stop();
    bool Initialize(std::string rom);
    byte* GetCurrentFrame();
    void SetInput(byte input, byte buttons);

private:
    std::unique_ptr<ICPU> m_cpu;
};
