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

    void UpdateChannel3Generator();

    typedef enum
    {
        UP,
        DOWN
    } EnvelopeDirection;

    typedef enum
    {
        CHANNEL_1,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4
    } APUChannel;

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

    class RegisterAwareSquareWaveGenerator
    {
    public:
        RegisterAwareSquareWaveGenerator(
            const APUChannel channel,
            const byte* sweepRegister,
            const byte* soundLengthRegister,
            const byte* volumeEnvelopeRegister,
            const byte* frequencyLoRegister,
            const byte* frequencyHiRegister,
            const byte* soundOnOffRegister
        );
        ~RegisterAwareSquareWaveGenerator() = default;

        float NextSample();
        void Reset();
        void TriggerSweepRegisterUpdate();
        void TriggerSoundLengthRegisterUpdate();
        void TriggerVolumeEnvelopeRegisterUpdate();
        void TriggerFrequencyLoRegisterUpdate();
        void TriggerFrequencyHiRegisterUpdate();

    private:
        const APUChannel m_Channel;
        const byte* m_SweepRegister;
        const byte* m_SoundLengthRegister;
        const byte* m_VolumeEnvelopeRegister;
        const byte* m_FrequencyLoRegister;
        const byte* m_FrequencyHiRegister;
        const byte* m_SoundOnOffRegister;

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

        void RegenerateCoefficients();
        void UpdateFrequency();
        void RestartSound();
    };

    class RegisterAwareNoiseGenerator
    {
    public:
        RegisterAwareNoiseGenerator(
            const APUChannel channel,
            const byte* soundLengthRegister,
            const byte* volumeEnvelopeRegister,
            const byte* polynomialCounterRegister,
            const byte* counterRegister,
            const byte* soundOnOffRegister
        );
        ~RegisterAwareNoiseGenerator() = default;

        float NextSample();
        void Reset();
        void TriggerSoundLengthRegisterUpdate();
        void TriggerVolumeEnvelopeRegisterUpdate();
        void TriggerPolynomialCounterRegisterUpdate();
        void TriggerCounterRegisterUpdate();

    private:
        const APUChannel m_Channel;
        const byte* m_SoundLengthRegister;
        const byte* m_VolumeEnvelopeRegister;
        const byte* m_PolynomialCounterRegister;
        const byte* m_CounterRegister;
        const byte* m_SoundOnOffRegister;
        
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

        void RestartSound();
    };

    // class WaveformGenerator
    // {
    // public:
    //     WaveformGenerator();
    //     ~WaveformGenerator() = default;

    //     void SetFrequency(double frequency_hz);
    //     void SetOutputLevel(double level);
    //     void SetCounterModeEnabled(bool is_enabled);
    //     void SetSoundLength(double sound_length_seconds);
    //     void SetWaveRamLocation(byte *wave_buffer);

    //     void Enable();
    //     void Disable();
    //     void RestartSound();

    //     float NextSample();

    //     void DebugLog();

    // private:
    //     bool m_Enabled;
    //     double m_FrequencyHz;
    //     bool m_CounterModeEnabled;
    //     double m_SoundLengthSeconds;
    //     double m_OutputLevel;
    //     double m_Phase;
    //     double m_SoundLengthTimerSeconds;
    //     byte *m_WaveBuffer;
    // };

    class RegisterAwareWaveformGenerator
    {
    public:
        RegisterAwareWaveformGenerator(
            const APUChannel channel,
            const byte* channelSoundOnOffRegister,
            const byte* soundLengthRegister,
            const byte* selectOutputLevelRegister,
            const byte* frequencyLoRegister,
            const byte* frequencyHiRegister,
            const byte* waveBuffer,
            const byte* soundOnOffRegister
        );
        ~RegisterAwareWaveformGenerator() = default;

        float NextSample();
        void Reset();
        void TriggerChannelSoundOnOffRegisterUpdate();
        void TriggerSoundLengthRegisterUpdate();
        void TriggerSelectOutputLevelRegisterUpdate();
        void TriggerFrequencyLoRegisterUpdate();
        void TriggerFrequencyHiRegisterUpdate();

    private:
        const APUChannel m_Channel;
        const byte* m_ChannelSoundOnOffRegister;
        const byte* m_SoundLengthRegister;
        const byte* m_SelectOutputLevelRegister;
        const byte* m_FrequencyLoRegister;
        const byte* m_FrequencyHiRegister;
        const byte* m_WaveBuffer;
        const byte* m_SoundOnOffRegister;

        bool m_Enabled;
        double m_FrequencyHz;
        bool m_CounterModeEnabled;
        double m_SoundLengthSeconds;
        double m_OutputLevel;
        double m_Phase;
        double m_SoundLengthTimerSeconds;

        void RestartSound();
        void UpdateFrequency();
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
    RegisterAwareSquareWaveGenerator m_Channel1SoundGenerator;
    RegisterAwareSquareWaveGenerator m_Channel2SoundGenerator;
    RegisterAwareWaveformGenerator m_Channel3SoundGenerator;
    RegisterAwareNoiseGenerator m_Channel4SoundGenerator;

    // Update flags
    bool m_Channel1RequiresUpdate;
    bool m_Channel2RequiresUpdate;
    bool m_Channel3RequiresUpdate;
    bool m_Channel4RequiresUpdate;

    bool m_Channel1Reset;

    // Output
    bool m_Initialized;
    SDL_AudioDeviceID m_AudioDevice;
    double m_AudioFrameRemainder;
    Buffer m_OutputBuffer;
};
