#include "PCH.hpp"
#include "Joypad.hpp"

// FF00 - P1/JOYP - Joypad (R/W)
#define JoyPadAddress 0xFF00

Joypad::Joypad()
{
    Logger::Log("Joypad created.");
}

Joypad::~Joypad()
{
    Logger::Log("Joypad destroyed.");
}

// IMemoryUnit
byte Joypad::ReadByte(const ushort& address)
{
    switch (address)
    {
    case JoyPadAddress:
        // TODO: NYI
        return 0x00;
    default:
        Logger::Log("APU::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool Joypad::WriteByte(const ushort& address, const byte val)
{
    switch (address)
    {
    case JoyPadAddress:
        // TODO: NYI
        return true;
    default:
        Logger::Log("Joypad::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
