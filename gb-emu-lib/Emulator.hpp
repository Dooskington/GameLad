#pragma once

#include "ICPU.hpp"

class Emulator
{
public:
    Emulator();

    void StepFrame();
    void Stop();
    bool Initialize(std::string rom);
    byte* GetCurrentFrame();

private:
    std::unique_ptr<ICPU> m_cpu;
};
