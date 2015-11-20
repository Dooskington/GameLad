#include "pch.hpp"
#include "Emulator.hpp"

#include "CPU.hpp"

Emulator::Emulator()
{
}

void Emulator::Step()
{
    m_cpu->Step();
}

void Emulator::Stop()
{
    m_cpu.reset();
}

bool Emulator::Initialize(const char* rom)
{
    // Create CPU
    m_cpu = std::make_unique<CPU>();
    if (m_cpu == nullptr)
    {
        Logger::LogError("CPU could not be created!");
        return false;
    }

    if (!m_cpu->Initialize())
    {
        Logger::LogError("CPU could not be initialized!");
        return false;
    }

    if (!m_cpu->LoadROM(rom))
    {
        Logger::Log("Failed to load the Gameboy ROM");
        return false;
    }

    return true;
}

byte* Emulator::GetCurrentFrame()
{
    return m_cpu->GetCurrentFrame();
}

void Emulator::SetInput(byte input, byte buttons)
{
    m_cpu->SetInput(input, buttons);
}

void Emulator::SetVSyncCallback(void(*pCallback)())
{
    m_cpu->SetVSyncCallback(pCallback);
}
