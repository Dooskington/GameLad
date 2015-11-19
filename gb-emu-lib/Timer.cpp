#include "pch.hpp"
#include "Timer.hpp"

// FF04 - DIV - Divider Register (R/W)
// FF05 - TIMA - Timer counter (R/W)
// FF06 - TMA - Timer Modulo (R/W)
// FF07 - TAC - Timer Control (R/W)
#define Divider 0xFF04
#define TimerCounter 0xFF05
#define TimerModulo 0xFF06
#define TimerControl 0xFF07

/*
00:   4096 Hz(~4194 Hz SGB)     (1024 cycles)
01 : 262144 Hz(~268400 Hz SGB)  (16 cycles)
10 : 65536 Hz(~67110 Hz SGB)    (64 cycles)
11 : 16384 Hz(~16780 Hz SGB)    (256 cycles)
*/

const int FrequencyCounts[]
{
    1024, 16, 64, 256
};

#define Frequency4096   0x00
#define Frequency262144 0x01
#define Frequency65536  0x02
#define Frequency16384  0x03

Timer::Counter::Counter(byte frequency) :
    m_IsRunning(true),
    m_Value(0x00),
    m_Frequency(frequency),
    m_Cycles(FrequencyCounts[frequency])
{
}

bool Timer::Counter::Step(unsigned int cycles)
{
    if (!m_IsRunning)
    {
        return false;
    }

    // Count cycles until we reach the correct number for this timer frequency
    m_Cycles -= cycles;
    while (m_Cycles <= 0)
    {
        // Subtract cycles and increment value
        m_Cycles += FrequencyCounts[m_Frequency];
        m_Value++;
        if (m_Value == 0x00)
        {
            // If this overflowed, return true
            return true;
        }
    }

    return false;
}

byte Timer::Counter::GetValue()
{
    return m_Value;
}

void Timer::Counter::SetValue(byte value)
{
    m_Value = value;
}

void Timer::Counter::SetFrequency(byte frequency)
{
    m_Frequency = frequency;
    m_Cycles = FrequencyCounts[frequency];
}

void Timer::Counter::Start()
{
    m_IsRunning = true;
}

void Timer::Counter::Stop()
{
    m_IsRunning = false;
}

Timer::Timer(ICPU* pCPU) :
    m_CPU(pCPU),
    m_TimerModulo(0x00),
    m_TimerControl(0x00)
{
    m_DividerCounter = std::unique_ptr<Counter>(new Counter(Frequency16384));
    m_TimerCounter = std::unique_ptr<Counter>(new Counter(Frequency4096));
}

Timer::~Timer()
{
    m_DividerCounter.reset();
    m_TimerCounter.reset();
}


void Timer::Step(unsigned long cycles)
{
    m_DividerCounter->Step(cycles);

    // If the timer counter overflows, reset to TimerModulo and trigger interrupt
    if (m_TimerCounter->Step(cycles))
    {
        // If the timer counter overflows, set back to this value
        m_TimerCounter->SetValue(m_TimerModulo);

        if (m_CPU != nullptr)
        {
            m_CPU->TriggerInterrupt(INT50);
        }
    }
}

// IMemoryUnit
byte Timer::ReadByte(const ushort& address)
{
    switch (address)
    {
    case Divider:
        return m_DividerCounter->GetValue();
    case TimerCounter:
        return m_TimerCounter->GetValue();
    case TimerModulo:
        return m_TimerModulo;
    case TimerControl:
        return m_TimerControl;
    default:
        Logger::Log("Timer::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool Timer::WriteByte(const ushort& address, const byte val)
{
    switch (address)
    {
    case Divider:
        m_DividerCounter->SetValue(0x00);
        return true;
    case TimerCounter:
        m_TimerCounter->SetValue(val);
        return true;
    case TimerModulo:
        m_TimerModulo = val;
    case TimerControl:
        if (ISBITSET(val, 2))
        {
            m_TimerCounter->Start();
        }
        else
        {
            m_TimerCounter->Stop();
        }

        m_TimerCounter->SetFrequency(val & 0x03);
        return true;
    default:
        Logger::Log("Timer::ReadByte cannot write to address 0x%04X", address);
        return false;
    }
}
