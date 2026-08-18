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

unsigned long long Emulator::GetBaseClockCycles() const
{
    return m_cpu->GetBaseClockCycles();
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

void Emulator::SetAudioSampleRate(unsigned int sampleRate)
{
    m_cpu->SetAudioSampleRate(sampleRate);
}

size_t Emulator::ConsumeAudioSamples(float* pInterleavedBuffer, size_t maxFrames)
{
    return m_cpu->ConsumeAudioSamples(pInterleavedBuffer, maxFrames);
}
