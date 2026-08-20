#include "pch.hpp"
#include "Emulator.hpp"

#include "CPU.hpp"

#include <cstring>

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

bool Emulator::Initialize(
    const char* bootROMPath,
    const char* cartridgePath,
    ModelPreference modelPreference)
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

    static_cast<CPU*>(m_cpu.get())->SetModelPreference(modelPreference);
    if (!m_cpu->LoadROM(bootROMPath, cartridgePath))
    {
        Logger::Log("Failed to load the Gameboy ROM");
        return false;
    }

    return true;
}

bool Emulator::Initialize(
    const byte* bootROMData,
    size_t bootROMSize,
    const byte* cartridgeData,
    size_t cartridgeSize,
    ModelPreference modelPreference,
    const char* persistencePath,
    bool managePersistentData)
{
    std::unique_ptr<CPU> cpu(new CPU());
    if (!cpu->Initialize())
    {
        Logger::LogError("CPU could not be initialized!");
        return false;
    }

    cpu->SetModelPreference(modelPreference);
    if (!cpu->LoadROM(
            bootROMData,
            bootROMSize,
            cartridgeData,
            cartridgeSize,
            persistencePath,
            managePersistentData))
    {
        Logger::Log("Failed to load the Gameboy ROM");
        return false;
    }

    m_cpu = std::move(cpu);
    return true;
}

bool Emulator::Serialize(std::vector<byte>& output)
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    if (cpu == nullptr)
    {
        return false;
    }

    StateSerializer state(output);
    byte magic[8] = { 'G', 'L', 'S', 'T', 'A', 'T', 'E', 0 };
    unsigned int version = 1;
    state.SyncBytes(magic, sizeof(magic));
    state.Sync(version);
    return cpu->Serialize(state) && state.IsValid();
}

bool Emulator::DeserializeUnchecked(const void* input, size_t size)
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    if (cpu == nullptr)
    {
        return false;
    }

    StateSerializer state(input, size);
    byte magic[8] = {};
    unsigned int version = 0;
    state.SyncBytes(magic, sizeof(magic));
    state.Sync(version);
    const byte expectedMagic[8] = { 'G', 'L', 'S', 'T', 'A', 'T', 'E', 0 };
    if (!state.IsValid() ||
        std::memcmp(magic, expectedMagic, sizeof(magic)) != 0 ||
        version != 1)
    {
        return false;
    }

    const bool loaded = cpu->Serialize(state) &&
        state.IsValid() &&
        state.Remaining() == 0;
    if (!loaded)
    {
        Logger::LogError(
            "Save state was rejected near byte %u.",
            static_cast<unsigned int>(
                state.IsValid() ? state.Size() : state.ErrorOffset()));
    }
    return loaded;
}

bool Emulator::Deserialize(const void* input, size_t size)
{
    const byte expectedMagic[8] = { 'G', 'L', 'S', 'T', 'A', 'T', 'E', 0 };
    if (input == nullptr ||
        size < sizeof(expectedMagic) + sizeof(unsigned int) ||
        std::memcmp(input, expectedMagic, sizeof(expectedMagic)) != 0)
    {
        return false;
    }

    std::vector<byte> backup;
    if (!Serialize(backup))
    {
        return false;
    }

    if (DeserializeUnchecked(input, size))
    {
        return true;
    }

    if (!DeserializeUnchecked(backup.data(), backup.size()))
    {
        Logger::LogError("Failed to restore emulator state after a rejected save state.");
    }
    return false;
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

byte Emulator::ReadMemory(ushort address) const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0xFF : cpu->ReadMemoryForHost(address);
}

bool Emulator::WriteMemory(ushort address, byte value)
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu != nullptr && cpu->WriteMemoryForHost(address, value);
}

byte* Emulator::GetSaveRAM()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetSaveRAM();
}

size_t Emulator::GetSaveRAMSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetSaveRAMSize();
}

byte* Emulator::GetRTCData()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetRTCData();
}

size_t Emulator::GetRTCDataSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetRTCDataSize();
}

byte* Emulator::GetWorkRAM()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetWorkRAM();
}

size_t Emulator::GetWorkRAMSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetWorkRAMSize();
}

byte* Emulator::GetHighRAM()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetHighRAM();
}

size_t Emulator::GetHighRAMSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetHighRAMSize();
}

byte* Emulator::GetVideoRAM()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetVideoRAM();
}

size_t Emulator::GetVideoRAMSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetVideoRAMSize();
}

byte* Emulator::GetOAM()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetOAM();
}

size_t Emulator::GetOAMSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetOAMSize();
}

bool Emulator::IsRumbleEnabled() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu != nullptr && cpu->IsRumbleEnabled();
}

bool Emulator::HasBattery() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu != nullptr && cpu->HasBattery();
}

GameBoyMode Emulator::GetGameBoyMode() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? GameBoyMode::DMG : cpu->GetGameBoyMode();
}

const byte* Emulator::GetROM() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? nullptr : cpu->GetROM();
}

size_t Emulator::GetROMSize() const
{
    const CPU* cpu = dynamic_cast<const CPU*>(m_cpu.get());
    return cpu == nullptr ? 0 : cpu->GetROMSize();
}

void Emulator::ClearROMPatches()
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    if (cpu != nullptr)
    {
        cpu->ClearROMPatches();
    }
}

void Emulator::ApplyROMPatch(byte value, ushort address, int compareValue)
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    if (cpu != nullptr)
    {
        cpu->ApplyROMPatch(value, address, compareValue);
    }
}

void Emulator::SetSerialLinkCallback(
    SerialLinkCallback callback,
    void* context)
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    if (cpu != nullptr)
    {
        cpu->SetSerialLinkCallback(callback, context);
    }
}

bool Emulator::ClockExternalSerialBit(bool incomingBit, bool& outgoingBit)
{
    CPU* cpu = dynamic_cast<CPU*>(m_cpu.get());
    return cpu != nullptr &&
        cpu->ClockExternalSerialBit(incomingBit, outgoingBit);
}
