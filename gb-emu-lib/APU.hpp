#pragma once

#include <SDL2/SDL.h>
#include <mutex>

#define AudioSampleRate 48000
#define AudioOutChannelCount 2
#define FrameSizeBytes 8 // 32 bit samples * 2 channels

// Based on 60Hz screen refresh rate
#define AudioBufferSize ((AudioSampleRate * AudioOutChannelCount)) // 1 sec TODO: This is pretty big
#define CyclesPerFrame 70224 // TODO: Can we import this?
#define CyclesPerSecond 4213440 // CyclesPerFrame * 60

#define MaxHarmonicsCount 52

class APU : public IMemoryUnit
{
public:
    APU();
    ~APU();

    void Step(unsigned long cycles);
    void Channel1Callback(Uint8* pStream, int length);
    void Channel2Callback(Uint8* pStream, int length);
    void Channel3Callback(Uint8* pStream, int length);
    void Channel4Callback(Uint8* pStream, int length);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    void LoadChannel(int index, SDL_AudioCallback callback);

    typedef enum {
        UP, DOWN
    } EnvelopeDirection;

    // Ring buffer for streaming audio data
    class Buffer {
        public:
            Buffer(size_t element_count, size_t element_size);
            ~Buffer();

            void Reset();
            void Put(Uint8* element);
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

    class AdditiveSquareWaveGenerator {
        public:
            AdditiveSquareWaveGenerator();
            ~AdditiveSquareWaveGenerator() = default;

            void SetFrequency(double frequency_hz);
            void SetDutyCycle(double duty_cycle);
            void SetCounterModeEnabled(bool is_enabled);
            void SetSoundLength(double sound_length_seconds);
            void SetEnvelopeStartVolume(double envelope_start_volume);
            void SetEnvelopeDirection(EnvelopeDirection direction);
            void SetEnvelopeStep(double envelope_step_seconds);
            
            void RestartSound();
            
            float NextSample();

            void DebugLog();

        private:
            double m_FrequencyHz;
            double m_DutyCycle;
            bool m_CounterModeEnabled;
            double m_SoundLengthSeconds;
            bool m_EnvelopeModeEnabled;
            double m_EnvelopeDirection;
            double m_EnvelopeStartVolume;
            double m_EnvelopeStepLengthSeconds; 
            int m_HarmonicsCount;
            double m_Coefficients[MaxHarmonicsCount];
            double m_Phase;
            double m_SoundLengthTimerSeconds;

            void RegenerateCoefficients();
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
    byte m_Channel3FreuqencyLo;
    byte m_Channel3FreuqencyHi;
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

    // Output
    bool m_Initialized[4];
    SDL_AudioDeviceID m_DeviceChannel[4];
    double m_AudioFrameRemainder;
    Buffer m_Channel1Buffer;

    // For testing...
    byte PrevChannel1SweepTime;
    byte PrevChannel1SweepDirection;
    byte PrevChannel1SweepNumber;
    byte PrevChannel1WavePatternDuty;
    byte PrevChannel1SoundLength;
    byte PrevChannel1VolumeEnvelopeStart;
    byte PrevChannel1VolumeEnvelopeDirection;
    byte PrevChannel1VolumeEnvelopeSweepNumber;
    byte PrevChannel1Initial;
    byte PrevChannel1CounterConsecutive;
    int PrevChannel1Frequency;
};
