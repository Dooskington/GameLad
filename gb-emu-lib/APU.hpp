#pragma once

#include <SDL2/SDL.h>
#include <mutex>
#include <functional>

#define MaxHarmonicsCount 52

class APU : public IMemoryUnit
{
public:
    APU();
    ~APU();

    void Step(unsigned long cycles);
    void AudioDeviceCallback(Uint8* pStream, int length);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    void LoadAudioDevice(SDL_AudioCallback callback);

    void UpdateChannel1Generator();
    void UpdateChannel2Generator();
    void UpdateChannel3Generator();
    void UpdateChannel4Generator();

    typedef enum
    {
        UP,
        DOWN
    } EnvelopeDirection;

    // Ring buffer for streaming audio data
    class Buffer
    {
    public:
        Buffer(size_t element_count, size_t element_size);
        ~Buffer();

        void Reset();
        void Put(Uint8 *element);
        Uint8 *Get();

    private:
        size_t m_ElementCount;
        size_t m_ElementSize;
        size_t m_BufferSize;
        Uint8 *m_Bytes;
        Uint8 *m_DefaultBytes;
        size_t m_ReadIndex;
        size_t m_WriteIndex;
        std::mutex mutex;

        void AdvanceReadIndex();
        void AdvanceWriteIndex();
    };

    class AdditiveSquareWaveGenerator
    {
    public:
        AdditiveSquareWaveGenerator();
        ~AdditiveSquareWaveGenerator() = default;

        void SetFrequency(double frequency_hz);
        void SetDutyCycle(double duty_cycle);
        void SetCounterModeEnabled(bool is_enabled);
        void SetSoundLength(double sound_length_seconds);
        void SetSweepDirection(EnvelopeDirection direction);
        void SetSweepShiftFrequencyExponent(uint exponent);
        void SetSweepStepLength(double sweep_step_seconds);
        void SetEnvelopeStartVolume(double envelope_start_volume);
        void SetEnvelopeDirection(EnvelopeDirection direction);
        void SetEnvelopeStepLength(double envelope_step_seconds);
        void SetOnChannelOn(std::function<void()> callback);
        void SetOnChannelOff(std::function<void()> callback);

        void RestartSound();

        float NextSample();

        void DebugLog();

    private:
        double m_FrequencyHz;
        double m_DutyCycle;
        bool m_CounterModeEnabled;
        double m_SoundLengthSeconds;
        bool m_SweepModeEnabled;
        double m_SweepDirection;
        int m_SweepShiftFrequencyExponent;
        double m_SweepStepLengthSeconds;
        bool m_EnvelopeModeEnabled;
        double m_EnvelopeDirection;
        double m_EnvelopeStartVolume;
        double m_EnvelopeStepLengthSeconds;
        int m_HarmonicsCount;
        double m_Coefficients[MaxHarmonicsCount];
        double m_Phase;
        double m_SoundLengthTimerSeconds;
        bool m_ChannelIsPlaying;
        std::function<void()> m_OnChannelOn;
        std::function<void()> m_OnChannelOff;

        void RegenerateCoefficients();
    };

    class NoiseGenerator
    {
    public:
        NoiseGenerator();
        ~NoiseGenerator() = default;

        void SetFrequency(double frequency_hz);
        void SetCounterModeEnabled(bool is_enabled);
        void SetSoundLength(double sound_length_seconds);
        void SetEnvelopeStartVolume(double envelope_start_volume);
        void SetEnvelopeDirection(EnvelopeDirection direction);
        void SetEnvelopeStepLength(double envelope_step_seconds);

        void RestartSound();

        float NextSample();

        void DebugLog();

    private:
        double m_FrequencyHz;
        bool m_CounterModeEnabled;
        double m_SoundLengthSeconds;
        bool m_EnvelopeModeEnabled;
        double m_EnvelopeDirection;
        double m_EnvelopeStartVolume;
        double m_EnvelopeStepLengthSeconds;
        double m_Phase;
        double m_Signal;
        double m_SoundLengthTimerSeconds;
    };

    class WaveformGenerator
    {
    public:
        WaveformGenerator();
        ~WaveformGenerator() = default;

        void SetFrequency(double frequency_hz);
        void SetOutputLevel(double level);
        void SetCounterModeEnabled(bool is_enabled);
        void SetSoundLength(double sound_length_seconds);
        void SetWaveRamLocation(byte *wave_buffer);

        void Enable();
        void Disable();
        void RestartSound();

        float NextSample();

        void DebugLog();

    private:
        bool m_Enabled;
        double m_FrequencyHz;
        bool m_CounterModeEnabled;
        double m_SoundLengthSeconds;
        double m_OutputLevel;
        double m_Phase;
        double m_SoundLengthTimerSeconds;
        byte *m_WaveBuffer;
    };

private:
    byte m_Channel1Sweep;
    byte m_Channel1SoundLength;
    byte m_Channel1VolumeEnvelope;
    byte m_Channel1FrequencyLo;
    byte m_Channel1FrequencyHi;

    byte m_Channel2SoundLength;
    byte m_Channel2VolumeEnvelope;
    byte m_Channel2FrequencyLo;
    byte m_Channel2FrequencyHi;

    byte m_Channel3SoundOnOff;
    byte m_Channel3SoundLength;
    byte m_Channel3SelectOutputLevel;
    byte m_Channel3FrequencyLo;
    byte m_Channel3FrequencyHi;
    byte m_WavePatternRAM[0x0F + 1];

    byte m_Channel4SoundLength;
    byte m_Channel4VolumeEnvelope;
    byte m_Channel4PolynomialCounter;
    byte m_Channel4Counter;

    byte m_ChannelControlOnOffVolume;
    byte m_OutputTerminal;

    byte m_SoundOnOff;

    // Synthesis
    AdditiveSquareWaveGenerator m_Channel1SoundGenerator;
    AdditiveSquareWaveGenerator m_Channel2SoundGenerator;
    WaveformGenerator m_Channel3SoundGenerator;
    NoiseGenerator m_Channel4SoundGenerator;

    // Update flags
    bool m_Channel1RequiresUpdate;
    bool m_Channel2RequiresUpdate;
    bool m_Channel3RequiresUpdate;
    bool m_Channel4RequiresUpdate;

    // Output
    bool m_Initialized;
    SDL_AudioDeviceID m_AudioDevice;
    double m_AudioFrameRemainder;
    Buffer m_OutputBuffer;
};
