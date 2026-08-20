#pragma once

#include "HardwareModel.hpp"
#include "ICPU.hpp"

#include <vector>

#define JOYPAD_NONE             0

#define JOYPAD_INPUT_DOWN       1 << 3
#define JOYPAD_INPUT_UP         1 << 2
#define JOYPAD_INPUT_LEFT       1 << 1
#define JOYPAD_INPUT_RIGHT      1 << 0

#define JOYPAD_BUTTONS_START    1 << 3
#define JOYPAD_BUTTONS_SELECT   1 << 2
#define JOYPAD_BUTTONS_B        1 << 1
#define JOYPAD_BUTTONS_A        1 << 0

class Emulator
{
public:
    Emulator();

    int Step();
    unsigned long long GetBaseClockCycles() const;
    void Stop();
    bool Initialize(
        const char* bootROMPath,
        const char* cartridgePath,
        ModelPreference modelPreference = ModelPreference::Auto);
    bool Initialize(
        const byte* bootROMData,
        size_t bootROMSize,
        const byte* cartridgeData,
        size_t cartridgeSize,
        ModelPreference modelPreference = ModelPreference::Auto,
        const char* persistencePath = nullptr,
        bool managePersistentData = false);
    bool Serialize(std::vector<byte>& state);
    bool Deserialize(const void* state, size_t size);
    byte* GetCurrentFrame();
    void SetInput(byte input, byte buttons);
    void SetVSyncCallback(void(*pCallback)());

    void SetAudioSampleRate(unsigned int sampleRate);
    size_t ConsumeAudioSamples(float* pInterleavedBuffer, size_t maxFrames);
    byte ReadMemory(unsigned short address) const;
    bool WriteMemory(unsigned short address, byte value);
    byte* GetSaveRAM();
    size_t GetSaveRAMSize() const;
    byte* GetRTCData();
    size_t GetRTCDataSize() const;
    byte* GetWorkRAM();
    size_t GetWorkRAMSize() const;
    byte* GetHighRAM();
    size_t GetHighRAMSize() const;
    byte* GetVideoRAM();
    size_t GetVideoRAMSize() const;
    byte* GetOAM();
    size_t GetOAMSize() const;
    bool IsRumbleEnabled() const;
    bool HasBattery() const;
    GameBoyMode GetGameBoyMode() const;
    const byte* GetROM() const;
    size_t GetROMSize() const;
    void ClearROMPatches();
    void ApplyROMPatch(
        byte value,
        unsigned short address,
        int compareValue = -1);
    void SetSerialLinkCallback(SerialLinkCallback callback, void* context);
    bool ClockExternalSerialBit(bool incomingBit, bool& outgoingBit);

private:
    bool DeserializeUnchecked(const void* state, size_t size);

    std::unique_ptr<ICPU> m_cpu;
};
