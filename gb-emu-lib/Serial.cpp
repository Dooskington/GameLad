#include "PCH.hpp"
#include "Serial.hpp"

// FF01 - SB - Serial transfer data (R/W)
// FF02 - SC - Serial Transfer Control (R/W)
#define SerialTransferData 0xFF01
#define SerialTransferControl 0xFF02

Serial::Serial()
{
    Logger::Log("Serial created.");
}

Serial::~Serial()
{
    Logger::Log("Serial destroyed.");
}

// IMemoryUnit
byte Serial::ReadByte(const ushort& address)
{
    switch (address)
    {
    case SerialTransferData:
    case SerialTransferControl:
        // TODO: NYI
        return 0x00;
    default:
        Logger::Log("Serial::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool Serial::WriteByte(const ushort& address, const byte val)
{
    switch (address)
    {
    case SerialTransferData:
    case SerialTransferControl:
        // TODO: NYI
        return true;
    default:
        Logger::Log("Serial::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
