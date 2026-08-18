#pragma once

#define INT40 0x40  // VBlank
#define INT48 0x48  // STAT
#define INT50 0x50  // Timer
#define INT58 0x58  // Serial
#define INT60 0x60  // Joypad

class ICPU
{
public:
    virtual ~ICPU() {}
    virtual bool Initialize() = 0;
    virtual bool LoadROM(const char* bootROMPath, const char* cartridgePath) = 0;
    virtual int Step() = 0;

    /*
        Elapsed cycles in the base (non-doubled) clock domain.

        Step() reports raw CPU cycles, which run twice as fast while CGB
        double speed is active. Anything pacing against wall-clock time - a
        host frame loop or an audio window - must use this instead, or it
        will emulate only half a frame per real frame once a game switches
        to double speed.
    */
    virtual unsigned long long GetBaseClockCycles() const { return 0; }

    virtual void TriggerInterrupt(byte interrupt) = 0;
    virtual void QueueInterrupt(byte interrupt) { TriggerInterrupt(interrupt); }
    virtual byte* GetCurrentFrame() = 0;
    virtual void SetInput(byte input, byte buttons) = 0;
    virtual void SetVSyncCallback(void(*pCallback)()) = 0;

    // Host audio playback wiring. Samples are interleaved stereo floats
    // (Left, Right, Left, Right, ...); pInterleavedBuffer must have room for
    // maxFrames * 2 floats. This is a thin adapter over the APU's own
    // testable, thread-independent sample buffer (see APU::ConsumeSamples())
    // - the emulated audio hardware state never depends on when or whether
    // a host pulls from it.
    virtual void SetAudioSampleRate(unsigned int sampleRate) = 0;
    virtual size_t ConsumeAudioSamples(float* pInterleavedBuffer, size_t maxFrames) = 0;

    // Called by Timer once per real falling edge of its shared DIV/system
    // counter's bit 12 (including edges synthesized by a DIV write/reset),
    // so the APU's frame sequencer can be driven by the real hardware clock
    // relationship. Timer intentionally has no direct APU dependency - this
    // keeps that coupling mediated through the CPU, mirroring the existing
    // TriggerInterrupt() pattern.
    virtual void ClockAPUFrameSequencer() = 0;
};
