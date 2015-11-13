#include "pch.hpp"
#include "Serial.hpp"

// FF01 - SB - Serial transfer data (R/W)
// FF02 - SC - Serial Transfer Control (R/W)
#define SerialTransferData 0xFF01
#define SerialTransferControl 0xFF02

Serial::Serial()
{
}

Serial::~Serial()
{
}

// IMemoryUnit
byte Serial::ReadByte(const ushort& address)
{
    switch (address)
    {
    case SerialTransferData:
    case SerialTransferControl:
        Logger::Log("Serial::ReadByte is not implemented for address 0x%04X", address);
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
        Logger::Log("Serial::WriteByte is not implemented for address 0x%04X", address);
        return true;
    default:
        Logger::Log("Serial::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
