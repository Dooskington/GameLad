#pragma once

class Timer : public IMemoryUnit
{
public:
    Timer(ICPU* pCPU);
    ~Timer();

    void PreBoot();
    void Step(unsigned long cycles);

    void SetGameBoyMode(GameBoyMode mode) { m_mode = mode; }
    /*
        In CGB double speed the system counter this timer exposes as DIV runs at
        twice the base rate. The DIV-APU tap has to move up one bit so the frame
        sequencer keeps firing at 512 Hz in wall-clock terms.
    */
    void SetDoubleSpeed(bool doubleSpeed) { m_doubleSpeed = doubleSpeed; }

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    bool TimerInput() const;
    bool DivApuInput() const;
    void IncrementTimer();
    void Tick();

    ICPU* m_CPU;

    GameBoyMode m_mode;
    bool m_doubleSpeed;
    ushort m_SystemCounter;
    byte m_TimerCounter;
    byte m_TimerModulo;
    byte m_TimerControl;
    byte m_OverflowDelay;
    bool m_ReloadedThisCycle;
};
