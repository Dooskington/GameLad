#pragma once

class Timer : public IMemoryUnit
{
private:
    class Counter
    {
    public:
        Counter(byte frequency);
        bool Step(unsigned int cycles);

        byte GetValue();
        void SetValue(byte value);
        void SetFrequency(byte frequency);
        void Start();
        void Stop();

    private:
        bool m_IsRunning;
        byte m_Value;
        byte m_Frequency;
        int m_Cycles;
    };

public:
    Timer(ICPU* pCPU);
    ~Timer();

    void Step(unsigned long cycles);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    ICPU* m_CPU;

    std::unique_ptr<Counter> m_DividerCounter;
    std::unique_ptr<Counter> m_TimerCounter;

    byte m_TimerModulo;
    byte m_TimerControl;
};
