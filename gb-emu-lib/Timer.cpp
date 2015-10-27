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

Timer::Timer()
{
    Logger::Log("Timer created.");
}

Timer::~Timer()
{
    Logger::Log("Timer destroyed.");
}

// IMemoryUnit
byte Timer::ReadByte(const ushort& address)
{
    switch (address)
    {
    case Divider:
    case TimerCounter:
    case TimerModulo:
    case TimerControl:
        // TODO: NYI
        return 0x00;
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
    case TimerCounter:
    case TimerModulo:
    case TimerControl:
        // TODO: NYI
        return true;
    default:
        Logger::Log("Timer::ReadByte cannot write to address 0x%04X", address);
        return false;
    }
}
