#pragma once

#include <cstddef>
#include <vector>

// ---------------------------------------------------------------------------
// DMG APU (Audio Processing Unit)
//
// This implementation models the four DMG sound channels, the NR50/NR51/NR52
// mixer/power registers, and the 512 Hz "DIV-APU" frame sequencer that drives
// length, envelope, and sweep clocking. It advances entirely from the T-cycle
// counts the CPU reports through Step(), and produces a deterministic,
// resampled stereo sample stream that is independent of any host audio
// callback thread - see APU::ConsumeSamples().
//
// Clock integration note (see APU.cpp for more detail): real hardware clocks
// the frame sequencer from the falling edge of bit 4 of the shared system
// DIV counter (bit 5 in double-speed CGB mode), the same counter the Timer
// drives TIMA from. This implementation currently free-runs its own
// FRAME_SEQUENCER_PERIOD (8192 T-cycle) counter as an approximation of that
// edge, which is correct for normal operation but does not yet reproduce the
// "extra" frame-sequencer clock that occurs when a write to FF04 (DIV) resets
// the divider while bit 4 is high. Wiring this up correctly requires the
// Timer to expose its internal system-counter bit (or to notify the APU of
// DIV-falling-edge events); that work is tracked separately from this change
// so as to not duplicate the in-flight Timer edge-clock rework.
// ---------------------------------------------------------------------------

class SquareChannel
{
public:
    explicit SquareChannel(bool hasSweep);

    void Reset();
    void PowerOff(bool clearLength);

    // Register writes (raw hardware address semantics)
    void WriteSweep(byte value);        // NR10 (channel 1 only)
    void WriteLengthDuty(byte value);   // NR11 / NR21
    void WriteLengthOnly(byte value);   // NR11 / NR21 length field while APU is powered off:
                                         // on DMG the length counter reload remains writable
                                         // while powered down, but the duty bits are not.
    void WriteEnvelope(byte value);     // NR12 / NR22
    void WriteFrequencyLo(byte value);  // NR13 / NR23
    void WriteFrequencyHi(byte value);  // NR14 / NR24

    byte ReadSweep() const;
    byte ReadLengthDuty() const;
    byte ReadEnvelope() const;
    byte ReadFrequencyHi() const;

    void SetFrameSequencerStep(byte step) { m_FrameSequencerStep = step; }

    void StepFrequencyTimer(unsigned long cycles);
    void ClockLength();
    void ClockEnvelope();
    void ClockSweep();

    byte Amplitude() const;
    bool DacEnabled() const;
    bool IsEnabled() const { return m_Enabled; }

private:
    void Trigger();
    unsigned short CalculateSweepFrequency(bool& overflow) const;

private:
    const bool m_HasSweep;

    // NR10 (channel 1 only)
    byte m_SweepPeriod;
    bool m_SweepNegate;
    byte m_SweepShift;

    // NR11/NR21
    byte m_Duty;
    byte m_LengthLoad;

    // NR12/NR22
    byte m_EnvelopeInitialVolume;
    bool m_EnvelopeIncrease;
    byte m_EnvelopePeriod;

    // NR13/NR14 or NR23/NR24
    unsigned short m_Frequency;
    bool m_LengthEnabled;

    // Runtime state
    bool m_Enabled;
    byte m_FrameSequencerStep;
    int m_FrequencyTimer;
    byte m_DutyPosition;
    bool m_DutyStepped; // Obscure behavior: duty cycle clocking (and the
                         // audible waveform) doesn't begin until the duty
                         // step counter has advanced at least once since the
                         // APU was last powered on; the very first sample
                         // played is forced to a digital 0 until then.
    int m_LengthCounter;
    byte m_EnvelopeTimer;
    byte m_CurrentVolume;

    unsigned short m_ShadowFrequency;
    int m_SweepTimer;
    bool m_SweepEnabled;
    bool m_SweepNegateUsedSinceTrigger;
};

class WaveChannel
{
public:
    WaveChannel();

    void Reset();
    void PowerOff(bool clearLength);
    void OnPowerOn(); // Real hardware clears CH3's internal sample buffer only when
                       // the APU itself is powered on (not on every trigger).

    void WriteDacEnable(byte value);    // NR30
    void WriteLength(byte value);       // NR31
    void WriteVolume(byte value);       // NR32
    void WriteFrequencyLo(byte value);  // NR33
    void WriteFrequencyHi(byte value);  // NR34

    byte ReadDacEnable() const;
    byte ReadVolume() const;
    byte ReadFrequencyHi() const;

    byte ReadWaveRAM(const ushort& address) const;
    bool WriteWaveRAM(const ushort& address, byte value);

    void SetCGBWaveBehavior(bool enabled) { m_CGBWaveBehavior = enabled; }

    void SetFrameSequencerStep(byte step) { m_FrameSequencerStep = step; }

    void StepFrequencyTimer(unsigned long cycles);
    void ClockLength();

    byte Amplitude() const;
    bool DacEnabled() const { return m_DacEnabled; }
    bool IsEnabled() const { return m_Enabled; }

private:
    void Trigger();

private:
    bool m_DacEnabled;      // NR30
    byte m_LengthLoad;      // NR31
    byte m_VolumeCode;      // NR32
    unsigned short m_Frequency; // NR33/NR34
    bool m_LengthEnabled;

    bool m_Enabled;
    bool m_CGBWaveBehavior;
    byte m_FrameSequencerStep;
    int m_FrequencyTimer;
    int m_LengthCounter;
    byte m_WavePosition;
    byte m_LastReadByte;
    byte m_LastReadIndex;

    // DMG wave-RAM read/write locking: /RAM_CS remains asserted for roughly
    // two T-cycles around an internal fetch. CPU accesses outside that window
    // read 0xFF or drop writes.
    unsigned long m_CycleCounter;
    unsigned long m_LastReadTime;

    bool IsWaveRamAccessible() const;

    byte m_WaveRAM[0x10];
};

class NoiseChannel
{
public:
    NoiseChannel();

    void Reset();
    void PowerOff(bool clearLength);

    void WriteLength(byte value);       // NR41
    void WriteEnvelope(byte value);     // NR42
    void WritePolynomial(byte value);   // NR43
    void WriteCounter(byte value);      // NR44

    byte ReadEnvelope() const;
    byte ReadPolynomial() const;
    byte ReadCounter() const;

    void SetFrameSequencerStep(byte step) { m_FrameSequencerStep = step; }

    void StepFrequencyTimer(unsigned long cycles);
    void ClockLength();
    void ClockEnvelope();

    byte Amplitude() const;
    bool DacEnabled() const;
    bool IsEnabled() const { return m_Enabled; }

private:
    void Trigger();

private:
    byte m_LengthLoad;       // NR41

    // NR42
    byte m_EnvelopeInitialVolume;
    bool m_EnvelopeIncrease;
    byte m_EnvelopePeriod;

    // NR43
    byte m_ClockShift;
    bool m_WidthMode7Bit;
    byte m_DivisorCode;

    bool m_LengthEnabled;    // NR44 bit 6

    bool m_Enabled;
    byte m_FrameSequencerStep;
    int m_FrequencyTimer;
    int m_LengthCounter;
    byte m_EnvelopeTimer;
    byte m_CurrentVolume;
    unsigned short m_LFSR;
};

class APU : public IMemoryUnit
{
public:
    struct Sample
    {
        float Left;
        float Right;
    };

public:
    APU();
    ~APU();

    void Step(unsigned long cycles);

    // Real-hardware DIV coupling: the frame sequencer has no clock of its
    // own, it is clocked by Timer via falling edges of the shared DIV/system
    // counter's bit 12 (see Timer::DivApuInput()). CPU forwards those edges
    // here through ICPU::ClockAPUFrameSequencer(). By default (bare APU,
    // e.g. every existing unit test) the APU keeps self-clocking internally
    // via its own free-running FRAME_SEQUENCER_PERIOD counter in Step(), so
    // standalone tests are unaffected; SetFrameSequencerSelfClocked(false)
    // demotes that free-running counter once real Timer edges are wired up.
    void ClockFrameSequencerEdge();
    void SetFrameSequencerSelfClocked(bool selfClocked) { m_FrameSequencerSelfClocked = selfClocked; }

    /*
        Hardware model. Documented APU differences that depend on it:
        - length counters stay writable while the APU is powered off on DMG but
          not on CGB (blargg dmg_sound/cgb_sound "length ctr during power");
        - CH3 wave RAM remains accessible while playing on CGB, and CGB avoids
          the DMG retrigger-corruption glitch;
        - PCM12/PCM34 (0xFF76/0xFF77) only exist on CGB.
    */
    void SetGameBoyMode(GameBoyMode mode)
    {
        m_mode = mode;
        m_Channel3.SetCGBWaveBehavior(IsCGBHardware(mode));
    }

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

    // Testable, thread-independent sample stream. The emulated hardware state
    // above never touches this buffer's consumer side; a host audio callback
    // (or a test) can pull from it at its own pace via ConsumeSamples().
    void SetSampleRate(unsigned int sampleRate);
    size_t GetPendingSampleCount() const;
    size_t ConsumeSamples(Sample* pBuffer, size_t maxCount);
    void ClearSampleBuffer();

    // Debug/test accessors - do not affect emulated state.
    bool IsPowered() const { return m_Powered; }
    byte GetFrameSequencerStep() const { return m_FrameSequencerStep; }
    bool IsChannelEnabled(int channel) const;
    byte GetChannelAmplitude(int channel) const;
    // Raw analog mixer output for the most recently generated sample, before
    // the DC-blocking stage below. Lets the NR50/NR51 mixing math be verified
    // independently of the output filter.
    Sample GetMixerOutput() const { return m_LastMixerOutput; }

private:
    void PowerOn();
    void PowerOff();
    void ClockFrameSequencer();
    void GenerateSample();
    Sample MixSample() const;

private:
    SquareChannel m_Channel1;
    SquareChannel m_Channel2;
    WaveChannel m_Channel3;
    NoiseChannel m_Channel4;

    byte m_NR50; // FF24 - Master volume / Vin panning
    byte m_NR51; // FF25 - Channel panning
    bool m_Powered; // NR52 bit 7
    GameBoyMode m_mode;

    int m_FrameSequencerCounter;
    byte m_FrameSequencerStep;
    bool m_FrameSequencerSelfClocked;

    unsigned int m_SampleRate;
    double m_CyclesPerSample;
    double m_SampleCycleAccumulator;

    /*
        DC-blocking (high-pass) output stage.

        A channel whose DAC is enabled but whose digital output is 0 still
        drives its DAC to a rail, so the raw mixer output carries a large
        constant offset - four idle-but-enabled DACs pin the mix at full
        scale. Hardware removes that offset with the output coupling
        capacitor; without it the audible signal rides at the rail, DAC
        enable/disable and NR51 routing changes become full-scale steps, and
        the result sounds like static rather than music.

        This models that capacitor with the standard one-pole high-pass:
        the charge decays by 0.999958 per 4194304Hz tick, raised to the
        number of ticks each output sample represents.
    */
    double m_CapacitorChargeFactor;
    float m_CapacitorLeft;
    float m_CapacitorRight;
    Sample m_LastMixerOutput;

    std::vector<Sample> m_SampleBuffer;
    size_t m_SampleReadIndex;
    static const size_t MaxBufferedSamples = 1 << 15;
};
