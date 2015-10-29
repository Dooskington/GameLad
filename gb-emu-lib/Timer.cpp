#include "PCH.hpp"
#include "Timer.hpp"

// FF04 - DIV - Divider Register (R/W)
// FF05 - TIMA - Timer counter (R/W)
// FF06 - TMA - Timer Modulo (R/W)
// FF07 - TAC - Timer Control (R/W)
#define Divider 0xFF04
#define TimerCounter 0xFF05
#define TimerModulo 0xFF06
#define TimerControl 0xFF07

#define Frequency4096   0x40
#define Frequency16384  0x10
#define Frequency65536  0x04
#define Frequency262144 0x01

Timer::Counter::Counter(byte frequency) :
    m_Value(0x00),
    m_Frequency(frequency)
{
}

bool Timer::Counter::Step(unsigned int cycles)
{
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

Timer::Timer(ICPU* pCPU) :
    m_CPU(pCPU),
    m_TimerModulo(0x00),
    m_TimerControl(0x00)
{
    m_DividerCounter = std::unique_ptr<Counter>(new Counter(Frequency16384));
    m_TimerCounter = std::unique_ptr<Counter>(new Counter(Frequency4096));

    Logger::Log("Timer created.");
}

Timer::~Timer()
{
    m_DividerCounter.reset();
    m_TimerCounter.reset();

    Logger::Log("Timer destroyed.");
}


void Timer::Step(unsigned long cycles)
{
    if (m_TimerCounter->Step(cycles))
    {
        if (m_CPU != nullptr)
        {
            m_CPU->TriggerInterrupt(INT50);
            m_TimerCounter->SetValue(m_TimerModulo);
        }
    }

    m_DividerCounter->Step(cycles);
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
        // TODO: NYI
        Logger::Log("Timer::WriteBytes: TimerControl is NYI!");
        return true;
    default:
        Logger::Log("Timer::ReadByte cannot write to address 0x%04X", address);
        return false;
    }
}
