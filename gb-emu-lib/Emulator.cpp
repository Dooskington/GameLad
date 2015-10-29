#include "PCH.hpp"
#include "Emulator.hpp"

#include "CPU.hpp"

Emulator::Emulator()
{
}

void Emulator::StepFrame()
{
    // Emulate one frame on the CPU (70244 cycles or CyclesPerFrame)
    m_cpu->StepFrame();
}

void Emulator::Stop()
{
    m_cpu.reset();
}

bool Emulator::Initialize()
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

    if (!m_cpu->LoadROM("res/tests/cpu_instrs.gb"))
    {
        Logger::Log("Failed to load the Gameboy ROM");
        return false;
    }

    return true;
}
