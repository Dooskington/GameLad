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

Timer::Timer(ICPU* pCPU) :
    m_CPU(pCPU),
    m_mode(GameBoyMode::DMG),
    m_doubleSpeed(false),
    m_SystemCounter(0x0000),
    m_TimerCounter(0x00),
    m_TimerModulo(0x00),
    m_TimerControl(0x00),
    m_OverflowDelay(0),
    m_ReloadedThisCycle(false)
{
}

Timer::~Timer()
{
}

void Timer::Serialize(StateSerializer& state)
{
    state.SyncEnum(m_mode);
    state.Sync(m_doubleSpeed);
    state.Sync(m_SystemCounter);
    state.Sync(m_TimerCounter);
    state.Sync(m_TimerModulo);
    state.Sync(m_TimerControl);
    state.Sync(m_OverflowDelay);
    state.Sync(m_ReloadedThisCycle);

    if (state.IsReading() &&
        (static_cast<unsigned int>(m_mode) >
             static_cast<unsigned int>(GameBoyMode::CGBCompatibility) ||
         m_TimerControl > 0x07 ||
         m_OverflowDelay > 4 ||
         (!IsCGBHardware(m_mode) && m_doubleSpeed)))
    {
        state.Invalidate();
    }
}

void Timer::PreBoot()
{
    /*
        The next T-cycle advances to the documented post-boot divider phase.
        DMG leaves its boot ROM with the system counter at 0xABCC; a CGB runs a
        much longer boot ROM and leaves it at 0x267C (the values asserted by
        mooneye-gb's boot_div tests for each console).
    */
    m_SystemCounter = IsCGBHardware(m_mode) ? 0x267B : 0xABCB;
}

bool Timer::TimerInput() const
{
    if (!ISBITSET(m_TimerControl, 2))
    {
        return false;
    }

    static const byte dividerBits[] = { 9, 3, 5, 7 };
    return ISBITSET(m_SystemCounter, dividerBits[m_TimerControl & 0x03]);
}

bool Timer::DivApuInput() const
{
    return ISBITSET(m_SystemCounter, (m_doubleSpeed ? 13 : 12));
}

void Timer::IncrementTimer()
{
    if (m_OverflowDelay != 0 || m_ReloadedThisCycle)
    {
        return;
    }

    if (m_TimerCounter == 0xFF)
    {
        m_TimerCounter = 0x00;
        m_OverflowDelay = 4;
        return;
    }

    m_TimerCounter++;
}

void Timer::Tick()
{
    m_ReloadedThisCycle = false;

    if (m_OverflowDelay != 0)
    {
        m_OverflowDelay--;
        if (m_OverflowDelay == 0)
        {
            m_TimerCounter = m_TimerModulo;
            m_ReloadedThisCycle = true;

            if (m_CPU != nullptr)
            {
                m_CPU->TriggerInterrupt(INT50);
            }
        }
    }

    bool oldTimerInput = TimerInput();
    bool oldDivApuInput = DivApuInput();
    m_SystemCounter++;
    if (oldTimerInput && !TimerInput())
    {
        IncrementTimer();
    }

    if (oldDivApuInput && !DivApuInput() && m_CPU != nullptr)
    {
        m_CPU->ClockAPUFrameSequencer();
    }
}

void Timer::Step(unsigned long cycles)
{
    while (cycles-- != 0)
    {
        Tick();
    }
}

// IMemoryUnit
byte Timer::ReadByte(const ushort& address)
{
    switch (address)
    {
    case Divider:
        return static_cast<byte>(m_SystemCounter >> 8);
    case TimerCounter:
        return m_TimerCounter;
    case TimerModulo:
        return m_TimerModulo;
    case TimerControl:
        return m_TimerControl | 0xF8;
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
    {
        bool oldTimerInput = TimerInput();
        bool oldDivApuInput = DivApuInput();
        m_SystemCounter = 0x0000;
        if (oldTimerInput && !TimerInput())
        {
            IncrementTimer();
        }

        if (oldDivApuInput && m_CPU != nullptr)
        {
            m_CPU->ClockAPUFrameSequencer();
        }
        return true;
    }
    case TimerCounter:
        if (m_ReloadedThisCycle)
        {
            return true;
        }

        m_TimerCounter = val;
        m_OverflowDelay = 0;
        return true;
    case TimerModulo:
        m_TimerModulo = val;
        if (m_ReloadedThisCycle)
        {
            m_TimerCounter = val;
        }
        return true;
    case TimerControl:
    {
        bool oldTimerInput = TimerInput();
        m_TimerControl = val & 0x07;
        if (oldTimerInput && !TimerInput())
        {
            IncrementTimer();
        }
        return true;
    }
    default:
        Logger::Log("Timer::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
