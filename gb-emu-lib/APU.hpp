#pragma once

#include <mutex>

// FF10 - NR10 - Channel 1 Sweep register (R / W)
// FF11 - NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
// FF12 - NR12 - Channel 1 Volume Envelope (R/W)
// FF13 - NR13 - Channel 1 Frequency lo (Write Only)
// FF14 - NR14 - Channel 1 Frequency hi (R/W)
// FF16 - NR21 - Channel 2 Sound Length/Wave Pattern Duty (R/W)
// FF17 - NR22 - Channel 2 Volume Envelope (R/W)
// FF18 - NR23 - Channel 2 Frequency lo data (W)
// FF19 - NR24 - Channel 2 Frequency hi data (R/W)
// FF1A - NR30 - Channel 3 Sound on/off (R/W)
// FF1B - NR31 - Channel 3 Sound Length
// FF1C - NR32 - Channel 3 Select output level (R/W)
// FF1D - NR33 - Channel 3 Frequency's lower data (W)
// FF1E - NR34 - Channel 3 Frequency's higher data (R/W)
// FF20 - NR41 - Channel 4 Sound Length (R/W)
// FF21 - NR42 - Channel 4 Volume Envelope (R/W)
// FF22 - NR43 - Channel 4 Polynomial Counter (R/W)
// FF23 - NR44 - Channel 4 Counter/consecutive; Inital (R/W)
// FF24 - NR50 - Channel control / ON-OFF / Volume (R/W)
// FF25 - NR51 - Selection of Sound output terminal (R/W)
// FF26 - NR52 - Sound on/off
#define Channel1Sweep 0xFF10
#define Channel1LengthWavePatternDuty 0xFF11
#define Channel1VolumeEnvelope 0xFF12
#define Channel1FrequencyLo 0xFF13
#define Channel1FrequencyHi 0xFF14
#define Channel2LengthWavePatternDuty 0xFF16
#define Channel2VolumeEnvelope 0xFF17
#define Channel2FrequnecyLo 0xFF18
#define Channel2FrequencyHi 0xFF19
#define Channel3OnOff 0xFF1A
#define Channel3Length 0xFF1B
#define Channel3OutputLevel 0xFF1C
#define Channel3FrequencyLower 0xFF1D
#define Channel3FrequnecyHigher 0xFF1E
#define Channel4Length 0xFF20
#define Channel4VolumeEnvelope 0xFF21
#define Channel4PolynomialCounter 0xFF22
#define Channel4Counter 0xFF23
#define ChannelControl 0xFF24
#define OutputTerminalSelection 0xFF25
#define SoundOnOff 0xFF26

#define OutputLevelSO1 (m_ChannelControlOnOffVolume & 0x7)
#define OutputLevelSO2 ((m_ChannelControlOnOffVolume >> 4) & 0x7)

#define OutputChannel1ToSO1 ISBITSET(m_OutputTerminal, 0)
#define OutputChannel2ToSO1 ISBITSET(m_OutputTerminal, 1)
#define OutputChannel3ToSO1 ISBITSET(m_OutputTerminal, 2)
#define OutputChannel4ToSO1 ISBITSET(m_OutputTerminal, 3)
#define OutputChannel1ToSO2 ISBITSET(m_OutputTerminal, 4)
#define OutputChannel2ToSO2 ISBITSET(m_OutputTerminal, 5)
#define OutputChannel3ToSO2 ISBITSET(m_OutputTerminal, 6)
#define OutputChannel4ToSO2 ISBITSET(m_OutputTerminal, 7)

#define AudioSampleRate 48000
#define AudioOutChannelCount 2
#define FrameSizeBytes 8              // 32 bit samples * 2 channels
#define AudioDeviceBufferSize 2048    // 2 KB
#define InternalAudioBufferSize 32768 // 32 KB

#define CyclesPerSecond 4213440 // CyclesPerFrame * 60Hz refresh

#define Pi 3.141592653589793
#define TwoPi 6.283185307179586

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

    typedef enum
    {
        CHANNEL_1,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4
    } APUChannel;

    // Byte ring buffer for streaming audio data
    class Buffer
    {
    public:
        Buffer(size_t element_count, size_t element_size);
        ~Buffer();

        void Reset();
        void Put(Uint8 *element);
        Uint8* Get();

    private:
        size_t m_ElementCount;
        size_t m_ElementSize;
        size_t m_BufferSize;
        Uint8* m_Bytes;
        Uint8* m_DefaultBytes;
        size_t m_ReadIndex;
        size_t m_WriteIndex;
        std::mutex mutex;

        void AdvanceReadIndex();
        void AdvanceWriteIndex();
    };

    // Base class for sound channel sample generators
    class SoundGenerator
    {
    public:
        SoundGenerator(
            const APUChannel channel,
            const byte* soundOnOffRegister
        );
        virtual ~SoundGenerator() = default;
        float NextSample();

    protected:
        const APUChannel m_Channel;
        const byte* m_SoundOnOffRegister;

        bool m_Enabled = true;
        double m_FrequencyHz = 1.0;
        double m_Phase = 0.0;
        bool m_CounterModeEnabled = false;
        double m_SoundLengthSeconds;
        bool m_SweepModeEnabled = false;
        double m_SweepDirection;
        int m_SweepShiftFrequencyExponent;
        double m_SweepStepLengthSeconds;
        bool m_EnvelopeModeEnabled = false;
        double m_EnvelopeDirection;
        double m_EnvelopeStartVolume = 1.0;
        double m_EnvelopeStepLengthSeconds;
        double m_SoundLengthTimerSeconds = 0.0;
        bool m_SoundLengthExpired = false;
        unsigned int m_FrequencyRegisterData = 0;

        virtual float NextWaveformSample() = 0;
        virtual void UpdateFrequency(unsigned int freqencyRegValue) = 0;

        void RestartSound();
        void SetSoundOnOffFlag();
        void ResetSoundOnOffFlag();
    };

    class SquareWaveGenerator : public SoundGenerator
    {
    public:
        SquareWaveGenerator(
            const APUChannel channel,
            const byte* soundOnOffRegister,
            const byte* sweepRegister,
            const byte* soundLengthRegister,
            const byte* volumeEnvelopeRegister,
            const byte* frequencyLoRegister,
            const byte* frequencyHiRegister
        );
        ~SquareWaveGenerator() = default;

        void TriggerSweepRegisterUpdate();
        void TriggerSoundLengthRegisterUpdate();
        void TriggerVolumeEnvelopeRegisterUpdate();
        void TriggerFrequencyLoRegisterUpdate();
        void TriggerFrequencyHiRegisterUpdate();

    private:
        // Maximum number of harmonics used to generate the square wave
        static const int MaxHarmonicsCount = 52;

        const byte* m_SweepRegister;
        const byte* m_SoundLengthRegister;
        const byte* m_VolumeEnvelopeRegister;
        const byte* m_FrequencyLoRegister;
        const byte* m_FrequencyHiRegister;

        double m_DutyCycle = 0.5;
        int m_HarmonicsCount = 0;
        double m_Coefficients[MaxHarmonicsCount];

        float NextWaveformSample() override;
        void UpdateFrequency(unsigned int freqencyRegValue) override;

        void RegenerateCoefficients();
    };

    class NoiseGenerator : public SoundGenerator
    {
    public:
        NoiseGenerator(
            const APUChannel channel,
            const byte* soundOnOffRegister,
            const byte* soundLengthRegister,
            const byte* volumeEnvelopeRegister,
            const byte* polynomialCounterRegister,
            const byte* counterRegister
        );
        ~NoiseGenerator() = default;

        void TriggerSoundLengthRegisterUpdate();
        void TriggerVolumeEnvelopeRegisterUpdate();
        void TriggerPolynomialCounterRegisterUpdate();
        void TriggerCounterRegisterUpdate();

    private:
        const byte* m_SoundLengthRegister;
        const byte* m_VolumeEnvelopeRegister;
        const byte* m_PolynomialCounterRegister;
        const byte* m_CounterRegister;
        
        float m_Signal = 0.5;
        double m_PreviousSamplePhase = 0.0;

        unsigned int m_shiftRegisterMSB = 14;
        unsigned int m_shiftRegister = 0xFF;

        float NextWaveformSample() override;
        void UpdateFrequency(unsigned int freqencyRegValue) override;
    };

    class WaveformGenerator : public SoundGenerator
    {
    public:
        WaveformGenerator(
            const APUChannel channel,
            const byte* soundOnOffRegister,
            const byte* channelSoundOnOffRegister,
            const byte* soundLengthRegister,
            const byte* selectOutputLevelRegister,
            const byte* frequencyLoRegister,
            const byte* frequencyHiRegister,
            const byte* waveBuffer
        );
        ~WaveformGenerator() = default;

        void TriggerChannelSoundOnOffRegisterUpdate();
        void TriggerSoundLengthRegisterUpdate();
        void TriggerSelectOutputLevelRegisterUpdate();
        void TriggerFrequencyLoRegisterUpdate();
        void TriggerFrequencyHiRegisterUpdate();

    private:
        const byte* m_ChannelSoundOnOffRegister;
        const byte* m_SoundLengthRegister;
        const byte* m_SelectOutputLevelRegister;
        const byte* m_FrequencyLoRegister;
        const byte* m_FrequencyHiRegister;
        const byte* m_WaveBuffer;

        byte m_VolumeShift = 0;

        float NextWaveformSample() override;
        void UpdateFrequency(unsigned int freqencyRegValue) override;
    };

private:
    // Audio Registers
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
    SquareWaveGenerator m_Channel1SoundGenerator;
    SquareWaveGenerator m_Channel2SoundGenerator;
    WaveformGenerator m_Channel3SoundGenerator;
    NoiseGenerator m_Channel4SoundGenerator;

    // Output
    SDL_AudioDeviceID m_AudioDevice;
    double m_AudioFrameRemainder;
    Buffer m_OutputBuffer;
    bool m_Initialized;
};
