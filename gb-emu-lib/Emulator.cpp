#include "pch.hpp"
#include "Emulator.hpp"

#include "CPU.hpp"

Emulator::Emulator()
{
}

int Emulator::Step()
{
    return m_cpu->Step();
}

void Emulator::Stop()
{
    m_cpu.reset();
}

bool Emulator::Initialize(const char* bootROMPath, const char* cartridgePath)
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

    if (!m_cpu->LoadROM(bootROMPath, cartridgePath))
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
