#pragma once

#include "PCH.hpp"

class MMU
{
public:
    MMU();
    ~MMU();

    bool Initialize();
    byte ReadByte(unsigned short PC);
    unsigned short ReadUShort(unsigned short PC);

private:
    bool LoadBootRom(std::string path);

private:
    // Booting
    bool m_isBooting;
    byte m_bios[0xFF + 1]; // The boot rom, loaded from bios.bin

    // Memory
    byte m_memory[0xFFFF + 1]; // 64k array that will act as memory
};