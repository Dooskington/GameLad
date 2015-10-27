#pragma once

class ICPU
{
public:
    virtual ~ICPU() {}
    virtual bool Initialize() = 0;
    virtual bool LoadROM(std::string path) = 0;
    virtual void StepFrame() = 0;
};
