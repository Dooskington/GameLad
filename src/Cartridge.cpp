#include "PCH.hpp"
#include "Cartridge.hpp"

Cartridge::Cartridge()
{
    Logger::Log("Cartridge created.");
}

Cartridge::~Cartridge()
{
    Logger::Log("Cartridge destroyed.");
}

// IMemoryUnit
byte Cartridge::ReadByte(const ushort& address)
{
    return 0x00;
}

ushort Cartridge::ReadUShort(const ushort& address)
{
    return 0x0000;
}

bool Cartridge::WriteByte(const ushort& address, const byte val)
{
    return false;
}
