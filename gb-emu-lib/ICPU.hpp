#pragma once

#define INT40 0x40  // VBlank
#define INT48 0x48  // STAT
#define INT50 0x50  // Timer
#define INT58 0x58  // Serial
#define INT60 0x60  // Joypad

class ICPU
{
public:
    virtual ~ICPU() {}
    virtual bool Initialize() = 0;
    virtual bool LoadROM(const char* path) = 0;
    virtual void StepFrame() = 0;
    virtual void TriggerInterrupt(byte interrupt) = 0;
    virtual byte* GetCurrentFrame() = 0;
    virtual void SetInput(byte input, byte buttons) = 0;
};
