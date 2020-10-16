#include "pch.hpp"
#include "APU.hpp"
#include <cmath>

#define SILENCE 0x80
#define HIGH 0xFF
#define LOW 0x00

#define PI 3.141592653589793
#define TWO_PI 6.283185307179586

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

#define CHANNEL1 0
#define CHANNEL2 1
#define CHANNEL3 2
#define CHANNEL4 3

#define Channel1SweepTime ((m_Channel1Sweep >> 4) & 7)
#define Channel1SweepDirection ((m_Channel1Sweep >> 3) & 1)
#define Channel1SweepNumber (m_Channel1Sweep & 7)
#define Channel1WavePatternDuty ((m_Channel1SoundLength >> 6) & 3)
#define Channel1SoundLength ((m_Channel1SoundLength) & 0x3F)
#define Channel1VolumeEnvelopeStart ((m_Channel1VolumeEnvelope >> 4) & 0xF)
#define Channel1VolumeEnvelopeDirection ((m_Channel1VolumeEnvelope >> 3) & 1)
#define Channel1VolumeEnvelopeSweepNumber (m_Channel1VolumeEnvelope & 7)
#define Channel1Initial ISBITSET(m_Channel1FrequencyHi, 7)
#define Channel1CounterConsecutive ((m_Channel1FrequencyHi >> 6) & 1)
#define Channel1Frequency (((m_Channel1FrequencyHi << 8) | m_Channel1FrequencyLo) & 0x7FF)

#define Channel2WavePatternDuty ((m_Channel2SoundLength >> 6) & 3)
#define Channel2SoundLength ((m_Channel2SoundLength) & 0x3F)
#define Channel2VolumeEnvelopeStart ((m_Channel2VolumeEnvelope >> 4) & 0xF)
#define Channel2VolumeEnvelopeDirection ((m_Channel2VolumeEnvelope >> 3) & 1)
#define Channel2VolumeEnvelopeSweepNumber (m_Channel2VolumeEnvelope & 7)
#define Channel2Initial ISBITSET(m_Channel2FrequencyHi, 7)
#define Channel2CounterConsecutive ((m_Channel2FrequencyHi >> 6) & 1)
#define Channel2Frequency (((m_Channel2FrequencyHi << 8) | m_Channel2FrequencyLo) & 0x7FF)

#define Channel3SoundOnOff ISBITSET(m_Channel3SoundOnOff, 7)
#define Channel3SoundLength m_Channel3SoundLength
#define Channel3SelectOutputLevel ((m_Channel3SelectOutputLevel >> 5) & 0x3)
#define Channel3Initial ISBITSET(m_Channel3FrequencyHi, 7)
#define Channel3CounterConsecutive ((m_Channel3FrequencyHi >> 6) & 1)
#define Channel3Frequency (((m_Channel3FrequencyHi << 8) | m_Channel3FrequencyLo) & 0x7FF)

#define Channel4SoundLength (m_Channel4SoundLength & 0x1F)
#define Channel4VolumeEnvelopeStart ((m_Channel4VolumeEnvelope >> 4) & 0xF)
#define Channel4VolumeEnvelopeDirection ((m_Channel4VolumeEnvelope >> 3) & 1)
#define Channel4VolumeEnvelopeSweepNumber (m_Channel4VolumeEnvelope & 7)
#define Channel4ShiftClockFrequency ((m_Channel4PolynomialCounter >> 4) & 0xF)
#define Channel4CounterStep ((m_Channel4PolynomialCounter >> 3) & 1)
#define Channel4FrequencyDivideRatio (m_Channel4PolynomialCounter & 7)
#define Channel4Initial ISBITSET(m_Channel4Counter, 7)
#define Channel4CounterConsecutive ((m_Channel4Counter >> 6) & 1)

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

void Channel1CallbackStatic(void* pUserdata, Uint8* pStream, int length)
{
    reinterpret_cast<APU*>(pUserdata)->Channel1Callback(
        pStream,
        length);
}

void Channel2CallbackStatic(void* pUserdata, Uint8* pStream, int length)
{
    reinterpret_cast<APU*>(pUserdata)->Channel2Callback(
        pStream,
        length);
}

void Channel3CallbackStatic(void* pUserdata, Uint8* pStream, int length)
{
    reinterpret_cast<APU*>(pUserdata)->Channel3Callback(
        pStream,
        length);
}

void Channel4CallbackStatic(void* pUserdata, Uint8* pStream, int length)
{
    reinterpret_cast<APU*>(pUserdata)->Channel4Callback(
        pStream,
        length);
}

APU::APU() :
    m_Channel1Sweep(0x00),
    m_Channel1SoundLength(0x00),
    m_Channel1VolumeEnvelope(0x00),
    m_Channel1FrequencyLo(0x00),
    m_Channel1FrequencyHi(0x00),
    m_Channel2SoundLength(0x00),
    m_Channel2VolumeEnvelope(0x00),
    m_Channel2FrequencyLo(0x00),
    m_Channel2FrequencyHi(0x00),
    m_Channel3SoundOnOff(0x00),
    m_Channel3SoundLength(0x00),
    m_Channel3SelectOutputLevel(0x00),
    m_Channel3FrequencyLo(0x00),
    m_Channel3FrequencyHi(0x00),
    m_Channel4SoundLength(0x00),
    m_Channel4VolumeEnvelope(0x00),
    m_Channel4PolynomialCounter(0x00),
    m_Channel4Counter(0x00),
    m_ChannelControlOnOffVolume(0x00),
    m_OutputTerminal(0x00),
    m_SoundOnOff(0x00),
    m_Channel1SoundGenerator(),
    m_Channel2SoundGenerator(),
    m_Channel3SoundGenerator(),
    m_Channel4SoundGenerator(),
    m_OutputBuffer(AudioBufferSize, FrameSizeBytes),
    m_AudioFrameRemainder(0.0)
{
    memset(m_Initialized, false, ARRAYSIZE(m_Initialized));
    memset(m_DeviceChannel, 0, ARRAYSIZE(m_DeviceChannel));
    memset(m_WavePatternRAM, 0x00, ARRAYSIZE(m_WavePatternRAM));
    
    m_Channel3SoundGenerator.SetWaveRamLocation(m_WavePatternRAM);

    if (SDL_Init(SDL_INIT_AUDIO))
    {
        Logger::LogError("[SDL] Failed to initialize: %s", SDL_GetError());
    }
    else
    {
        LoadChannel(CHANNEL1, Channel1CallbackStatic);
        LoadChannel(CHANNEL2, Channel2CallbackStatic);
        LoadChannel(CHANNEL3, Channel3CallbackStatic);
        LoadChannel(CHANNEL4, Channel4CallbackStatic);
    }
}

APU::~APU()
{
    for (int index = CHANNEL1; index <= CHANNEL4; index++)
    {
        if (m_Initialized[index])
        {
            SDL_PauseAudioDevice(m_DeviceChannel[index], 1);
            SDL_CloseAudioDevice(m_DeviceChannel[index]);
        }
    }

    SDL_Quit();
}

void APU::Step(unsigned long cycles)
{
    // CHANNEL 1
    byte NewChannel1SweepTime = Channel1SweepTime;
    byte NewChannel1SweepDirection = Channel1SweepDirection;
    byte NewChannel1SweepNumber = Channel1SweepNumber;
    byte NewChannel1WavePatternDuty = Channel1WavePatternDuty;
    byte NewChannel1SoundLength = Channel1SoundLength;
    byte NewChannel1VolumeEnvelopeStart = Channel1VolumeEnvelopeStart;
    byte NewChannel1VolumeEnvelopeDirection = Channel1VolumeEnvelopeDirection;
    byte NewChannel1VolumeEnvelopeSweepNumber = Channel1VolumeEnvelopeSweepNumber;
    byte NewChannel1Initial = Channel1Initial;
    byte NewChannel1CounterConsecutive = Channel1CounterConsecutive;
    int NewChannel1Frequency = Channel1Frequency;

    if (
        NewChannel1SweepTime != PrevChannel1SweepTime ||
        NewChannel1SweepDirection != PrevChannel1SweepDirection ||
        NewChannel1SweepNumber != PrevChannel1SweepNumber ||
        NewChannel1WavePatternDuty != PrevChannel1WavePatternDuty ||
        NewChannel1SoundLength != PrevChannel1SoundLength ||
        NewChannel1VolumeEnvelopeStart != PrevChannel1VolumeEnvelopeStart ||
        NewChannel1VolumeEnvelopeDirection != PrevChannel1VolumeEnvelopeDirection ||
        NewChannel1VolumeEnvelopeSweepNumber != PrevChannel1VolumeEnvelopeSweepNumber ||
        NewChannel1Initial != PrevChannel1Initial ||
        NewChannel1CounterConsecutive != PrevChannel1CounterConsecutive ||
        NewChannel1Frequency != PrevChannel1Frequency
    ) {
        // Debug: Print audio registers whenever a value change occurs
        // printf("CHANNEL 1 -- SWPTIME:0x%01x SWEDIR:0x%01x SWPNUM:0x%01x DUTY:0x%01x LEN:0x%02x ENVST:0x%01x ENVDIR:0x%01x ENVNUM:0x%01x INIT:0x%01x CC:0x%01x FREQ:0x%03x\n",
        //     NewChannel1SweepTime,
        //     NewChannel1SweepDirection,
        //     NewChannel1SweepNumber,
        //     NewChannel1WavePatternDuty,
        //     NewChannel1SoundLength,
        //     NewChannel1VolumeEnvelopeStart,
        //     NewChannel1VolumeEnvelopeDirection,
        //     NewChannel1VolumeEnvelopeSweepNumber,
        //     NewChannel1Initial,
        //     NewChannel1CounterConsecutive,
        //     NewChannel1Frequency);

        PrevChannel1SweepTime = NewChannel1SweepTime;
        PrevChannel1SweepDirection = NewChannel1SweepDirection;
        PrevChannel1SweepNumber = NewChannel1SweepNumber;
        PrevChannel1WavePatternDuty = NewChannel1WavePatternDuty;
        PrevChannel1SoundLength = NewChannel1SoundLength;
        PrevChannel1VolumeEnvelopeStart = NewChannel1VolumeEnvelopeStart;
        PrevChannel1VolumeEnvelopeDirection = NewChannel1VolumeEnvelopeDirection;
        PrevChannel1VolumeEnvelopeSweepNumber = NewChannel1VolumeEnvelopeSweepNumber;
        PrevChannel1Initial = NewChannel1Initial;
        PrevChannel1CounterConsecutive = NewChannel1CounterConsecutive;
        PrevChannel1Frequency = NewChannel1Frequency;

        // Do anything that requires recaculation based on these values

        if (Channel1Initial) {
            m_Channel1SoundGenerator.RestartSound();
        }
        
        // Counter/Consecutive modes
        m_Channel1SoundGenerator.SetCounterModeEnabled(Channel1CounterConsecutive);

        // Sound Length = (64-t1)*(1/256) seconds
        m_Channel1SoundGenerator.SetSoundLength((64.0 - (double)Channel1SoundLength) / 256.0);

        // Envelope start volume. 4 bits of precision = 16 volume levels.
        m_Channel1SoundGenerator.SetEnvelopeStartVolume((double)Channel1VolumeEnvelopeStart / 16.0);

        // Length of an envelope step = n * (1/64)
        m_Channel1SoundGenerator.SetEnvelopeStep((double)Channel1VolumeEnvelopeSweepNumber / 64.0);

        // Envelope direction: 0 = Decrease, 1 = Increase
        m_Channel1SoundGenerator.SetEnvelopeDirection(Channel1VolumeEnvelopeDirection ? UP : DOWN);

        // For x = the value in the frequency register, the actual frequency
        // in Hz is 131072/(2048-x) Hz
        m_Channel1SoundGenerator.SetFrequency(131072.0 / (double)(2048 - Channel1Frequency));

        // Wave Duty
        // 00: 12.5%
        // 01: 25%
        // 10: 50%
        // 11: 75%
        switch (Channel1WavePatternDuty) {
            case 0:
                m_Channel1SoundGenerator.SetDutyCycle(0.125);
                break;
            case 1:
                m_Channel1SoundGenerator.SetDutyCycle(0.25);
                break;
            case 2:
                m_Channel1SoundGenerator.SetDutyCycle(0.5);
                break;
            case 3:
                m_Channel1SoundGenerator.SetDutyCycle(0.75);
                break;
            default:
                Logger::LogError("Invalid duty cycle %x", Channel1WavePatternDuty);
                assert(false);
        }

        // m_Channel1SoundGenerator.DebugLog();
    }

    // CHANNEL 2
    byte NewChannel2WavePatternDuty = Channel2WavePatternDuty;
    byte NewChannel2SoundLength = Channel2SoundLength;
    byte NewChannel2VolumeEnvelopeStart = Channel2VolumeEnvelopeStart;
    byte NewChannel2VolumeEnvelopeDirection = Channel2VolumeEnvelopeDirection;
    byte NewChannel2VolumeEnvelopeSweepNumber = Channel2VolumeEnvelopeSweepNumber;
    byte NewChannel2Initial = Channel2Initial;
    byte NewChannel2CounterConsecutive = Channel2CounterConsecutive;
    int NewChannel2Frequency = Channel2Frequency;

    if (
        NewChannel2WavePatternDuty != PrevChannel2WavePatternDuty ||
        NewChannel2SoundLength != PrevChannel2SoundLength ||
        NewChannel2VolumeEnvelopeStart != PrevChannel2VolumeEnvelopeStart ||
        NewChannel2VolumeEnvelopeDirection != PrevChannel2VolumeEnvelopeDirection ||
        NewChannel2VolumeEnvelopeSweepNumber != PrevChannel2VolumeEnvelopeSweepNumber ||
        NewChannel2Initial != PrevChannel2Initial ||
        NewChannel2CounterConsecutive != PrevChannel2CounterConsecutive ||
        NewChannel2Frequency != PrevChannel2Frequency
    ) {
        // Debug: Print audio registers whenever a value change occurs
        // printf("CHANNEL 2 -- DUTY:0x%01x LEN:0x%02x ENVST:0x%01x ENVDIR:0x%01x ENVNUM:0x%01x INIT:0x%01x CC:0x%01x FREQ:0x%03x\n",
        //     NewChannel2WavePatternDuty,
        //     NewChannel2SoundLength,
        //     NewChannel2VolumeEnvelopeStart,
        //     NewChannel2VolumeEnvelopeDirection,
        //     NewChannel2VolumeEnvelopeSweepNumber,
        //     NewChannel2Initial,
        //     NewChannel2CounterConsecutive,
        //     NewChannel2Frequency);

        PrevChannel2WavePatternDuty = NewChannel2WavePatternDuty;
        PrevChannel2SoundLength = NewChannel2SoundLength;
        PrevChannel2VolumeEnvelopeStart = NewChannel2VolumeEnvelopeStart;
        PrevChannel2VolumeEnvelopeDirection = NewChannel2VolumeEnvelopeDirection;
        PrevChannel2VolumeEnvelopeSweepNumber = NewChannel2VolumeEnvelopeSweepNumber;
        PrevChannel2Initial = NewChannel2Initial;
        PrevChannel2CounterConsecutive = NewChannel2CounterConsecutive;
        PrevChannel2Frequency = NewChannel2Frequency;

        // Do anything that requires recaculation based on these values

        if (Channel2Initial) {
            m_Channel2SoundGenerator.RestartSound();
        }
        
        // Counter/Consecutive modes
        m_Channel2SoundGenerator.SetCounterModeEnabled(Channel2CounterConsecutive);

        // Sound Length = (64-t1)*(1/256) seconds
        m_Channel2SoundGenerator.SetSoundLength((64.0 - (double)Channel2SoundLength) / 256.0);

        // Envelope start volume. 4 bits of precision = 16 volume levels.
        m_Channel2SoundGenerator.SetEnvelopeStartVolume((double)Channel2VolumeEnvelopeStart / 16.0);

        // Length of an envelope step = n * (1/64)
        m_Channel2SoundGenerator.SetEnvelopeStep((double)Channel2VolumeEnvelopeSweepNumber / 64.0);

        // Envelope direction: 0 = Decrease, 1 = Increase
        m_Channel2SoundGenerator.SetEnvelopeDirection(Channel2VolumeEnvelopeDirection ? UP : DOWN);

        // For x = the value in the frequency register, the actual frequency
        // in Hz is 131072/(2048-x) Hz
        m_Channel2SoundGenerator.SetFrequency(131072.0 / (double)(2048 - Channel2Frequency));

        // Wave Duty
        // 00: 12.5%
        // 01: 25%
        // 10: 50%
        // 11: 75%
        switch (Channel2WavePatternDuty) {
            case 0:
                m_Channel2SoundGenerator.SetDutyCycle(0.125);
                break;
            case 1:
                m_Channel2SoundGenerator.SetDutyCycle(0.25);
                break;
            case 2:
                m_Channel2SoundGenerator.SetDutyCycle(0.5);
                break;
            case 3:
                m_Channel2SoundGenerator.SetDutyCycle(0.75);
                break;
            default:
                Logger::LogError("Invalid duty cycle %x", Channel2WavePatternDuty);
                assert(false);
        }

        // m_Channel2SoundGenerator.DebugLog();
    }

    // CHANNEL 3
    byte NewChannel3SoundOnOff = Channel3SoundOnOff;
    byte NewChannel3SoundLength = Channel3SoundLength;
    byte NewChannel3SelectOutputLevel = Channel3SelectOutputLevel;
    byte NewChannel3Initial = Channel3Initial;
    byte NewChannel3CounterConsecutive = Channel3CounterConsecutive;
    int NewChannel3Frequency = Channel3Frequency;
    bool IsWaveRamUpdate = false;

    for (int i = 0; i < 0x0F + 1; i++) {
        if (m_WavePatternRAM[i] != PrevWavePatternRAM[i]) {
            IsWaveRamUpdate = true;
            break;
        }
    }

    if (NewChannel3SoundOnOff != PrevChannel3SoundOnOff ||
        NewChannel3SoundLength != PrevChannel3SoundLength ||
        NewChannel3SelectOutputLevel != PrevChannel3SelectOutputLevel ||
        NewChannel3Initial != PrevChannel3Initial ||
        NewChannel3CounterConsecutive != PrevChannel3CounterConsecutive ||
        NewChannel3Frequency != PrevChannel3Frequency ||
        IsWaveRamUpdate) {

        // Debug: Print audio registers whenever a value change occurs
        // printf("CHANNEL 3 -- ON/OFF:0x%02x LEN:0x%01x LVL:0x%01x INIT:0x%01x CC:0x%01x FREQ:0x%03x DATA:0x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x\n",
        //     NewChannel3SoundOnOff,
        //     NewChannel3SoundLength,
        //     NewChannel3SelectOutputLevel,
        //     NewChannel3Initial,
        //     NewChannel3CounterConsecutive,
        //     NewChannel3Frequency,
        //     (m_WavePatternRAM[0] >> 4) & 0xF,
        //     m_WavePatternRAM[0] & 0xF,
        //     (m_WavePatternRAM[1] >> 4) & 0xF,
        //     m_WavePatternRAM[1] & 0xF,
        //     (m_WavePatternRAM[2] >> 4) & 0xF,
        //     m_WavePatternRAM[2] & 0xF,
        //     (m_WavePatternRAM[3] >> 4) & 0xF,
        //     m_WavePatternRAM[3] & 0xF,
        //     (m_WavePatternRAM[4] >> 4) & 0xF,
        //     m_WavePatternRAM[4] & 0xF,
        //     (m_WavePatternRAM[5] >> 4) & 0xF,
        //     m_WavePatternRAM[5] & 0xF,
        //     (m_WavePatternRAM[6] >> 4) & 0xF,
        //     m_WavePatternRAM[6] & 0xF,
        //     (m_WavePatternRAM[7] >> 4) & 0xF,
        //     m_WavePatternRAM[7] & 0xF,
        //     (m_WavePatternRAM[8] >> 4) & 0xF,
        //     m_WavePatternRAM[8] & 0xF,
        //     (m_WavePatternRAM[9] >> 4) & 0xF,
        //     m_WavePatternRAM[9] & 0xF,
        //     (m_WavePatternRAM[10] >> 4) & 0xF,
        //     m_WavePatternRAM[10] & 0xF,
        //     (m_WavePatternRAM[11] >> 4) & 0xF,
        //     m_WavePatternRAM[11] & 0xF,
        //     (m_WavePatternRAM[12] >> 4) & 0xF,
        //     m_WavePatternRAM[12] & 0xF,
        //     (m_WavePatternRAM[13] >> 4) & 0xF,
        //     m_WavePatternRAM[13] & 0xF,
        //     (m_WavePatternRAM[14] >> 4) & 0xF,
        //     m_WavePatternRAM[14] & 0xF,
        //     (m_WavePatternRAM[15] >> 4) & 0xF,
        //     m_WavePatternRAM[15] & 0xF);

        PrevChannel3SoundOnOff = NewChannel3SoundOnOff;
        PrevChannel3SoundLength = NewChannel3SoundLength;
        PrevChannel3SelectOutputLevel = NewChannel3SelectOutputLevel;
        PrevChannel3Initial = NewChannel3Initial;
        PrevChannel3CounterConsecutive = NewChannel3CounterConsecutive;
        PrevChannel3Frequency = NewChannel3Frequency;
        for (int i = 0; i < 0x0F + 1; i++) {
            PrevWavePatternRAM[i] = m_WavePatternRAM[i];
        }

        // Do anything that requires recaculation based on these values

        if (Channel3SoundOnOff) {
            m_Channel3SoundGenerator.Enable();
        } else {
            m_Channel3SoundGenerator.Disable();
        }

        if (Channel3Initial) {
            m_Channel3SoundGenerator.RestartSound();
        }

        // Counter/Consecutive modes
        m_Channel3SoundGenerator.SetCounterModeEnabled(Channel3CounterConsecutive);

        // Sound Length = (256-t1)*(1/256) seconds
        m_Channel3SoundGenerator.SetSoundLength((256.0 - (double)Channel3SoundLength) / 256.0);

        // Sound output level
        switch (Channel3SelectOutputLevel) {
            case 0:
                m_Channel3SoundGenerator.SetOutputLevel(0.0);
                break;
            case 1:
                m_Channel3SoundGenerator.SetOutputLevel(1.0);
                break;
            case 2:
                m_Channel3SoundGenerator.SetOutputLevel(0.5);
                break;
            case 3:
                m_Channel3SoundGenerator.SetOutputLevel(0.25);
                break;
            default:
                Logger::LogError("Invalid output level %x", Channel3SelectOutputLevel);
                assert(false);
        }

        // 65536/(2048-x) Hz
        m_Channel3SoundGenerator.SetFrequency(65536.0 / (2048.0 - Channel3Frequency));
                
    }       

    // CHANNEL 4
    byte NewChannel4SoundLength = Channel4SoundLength;
    byte NewChannel4VolumeEnvelopeStart = Channel4VolumeEnvelopeStart;
    byte NewChannel4VolumeEnvelopeDirection = Channel4VolumeEnvelopeDirection;
    byte NewChannel4VolumeEnvelopeSweepNumber = Channel4VolumeEnvelopeSweepNumber;
    byte NewChannel4ShiftClockFrequency = Channel4ShiftClockFrequency;
    byte NewChannel4CounterStep = Channel4CounterStep;
    byte NewChannel4FrequencyDivideRatio = Channel4FrequencyDivideRatio;
    byte NewChannel4Initial = Channel4Initial;
    byte NewChannel4CounterConsecutive = Channel4CounterConsecutive;

    if (
        NewChannel4SoundLength != PrevChannel4SoundLength ||
        NewChannel4VolumeEnvelopeStart != PrevChannel4VolumeEnvelopeStart ||
        NewChannel4VolumeEnvelopeDirection != PrevChannel4VolumeEnvelopeDirection ||
        NewChannel4VolumeEnvelopeSweepNumber != PrevChannel4VolumeEnvelopeSweepNumber ||
        NewChannel4ShiftClockFrequency != PrevChannel4ShiftClockFrequency ||
        NewChannel4CounterStep != PrevChannel4CounterStep ||
        NewChannel4FrequencyDivideRatio != PrevChannel4FrequencyDivideRatio ||
        NewChannel4Initial != PrevChannel4Initial ||
        NewChannel4CounterConsecutive != PrevChannel4CounterConsecutive
    ) {
        // Debug: Print audio registers whenever a value change occurs
        // printf("CHANNEL 4 -- LEN:0x%02x ENVST:0x%01x ENVDIR:0x%01x ENVNUM:0x%01x SHIFT:0x%01x STEP:0x%01x RATIO:0x%01x INIT:0x%01x CC:0x%01x\n",
            // NewChannel4SoundLength,
            // NewChannel4VolumeEnvelopeStart,
            // NewChannel4VolumeEnvelopeDirection,
            // NewChannel4VolumeEnvelopeSweepNumber,
            // NewChannel4ShiftClockFrequency,
            // NewChannel4CounterStep,
            // NewChannel4FrequencyDivideRatio,
            // NewChannel4Initial,
            // NewChannel4CounterConsecutive);

        PrevChannel4SoundLength = NewChannel4SoundLength;
        PrevChannel4VolumeEnvelopeStart = NewChannel4VolumeEnvelopeStart;
        PrevChannel4VolumeEnvelopeDirection = NewChannel4VolumeEnvelopeDirection;
        PrevChannel4VolumeEnvelopeSweepNumber = NewChannel4VolumeEnvelopeSweepNumber;
        PrevChannel4ShiftClockFrequency = NewChannel4ShiftClockFrequency;
        PrevChannel4CounterStep = NewChannel4CounterStep;
        PrevChannel4FrequencyDivideRatio = NewChannel4FrequencyDivideRatio;
        PrevChannel4Initial = NewChannel4Initial;
        PrevChannel4CounterConsecutive = NewChannel4CounterConsecutive;

        // Do anything that requires recaculation based on these values

        if (Channel4Initial) {
            m_Channel4SoundGenerator.RestartSound();
        }

        // Counter/Consecutive modes
        m_Channel4SoundGenerator.SetCounterModeEnabled(Channel4CounterConsecutive);

        // Sound Length = (64-t1)*(1/256) seconds
        m_Channel4SoundGenerator.SetSoundLength((64.0 - (double)Channel4SoundLength) / 256.0);

        // Envelope start volume. 4 bits of precision = 16 volume levels.
        m_Channel4SoundGenerator.SetEnvelopeStartVolume((double)Channel4VolumeEnvelopeStart / 16.0);

        // Length of an envelope step = n * (1/64)
        m_Channel4SoundGenerator.SetEnvelopeStep((double)Channel4VolumeEnvelopeSweepNumber / 64.0);

        // Envelope direction: 0 = Decrease, 1 = Increase
        m_Channel4SoundGenerator.SetEnvelopeDirection(Channel4VolumeEnvelopeDirection ? UP : DOWN);

        // 524288 Hz / r / 2^(s+1)
        double divide_ratio = Channel4FrequencyDivideRatio == 0 ? 0.5 : (double)Channel4FrequencyDivideRatio;
        m_Channel4SoundGenerator.SetFrequency(524288.0 / divide_ratio / (double)(2 << Channel4ShiftClockFrequency));
    }

    // Calculate the number of audio frames to generate for the elapsed CPU cycle count
    double sample_count = m_AudioFrameRemainder + (((double)AudioSampleRate / (double)CyclesPerSecond) * (double)cycles);
    double int_part = 0.0;
    m_AudioFrameRemainder = modf(sample_count, &int_part);

    for (int i = 0; i < int_part; i++) {
        float ch1_sample = m_Channel1SoundGenerator.NextSample();
        float ch2_sample = m_Channel2SoundGenerator.NextSample();
        float ch3_sample = m_Channel3SoundGenerator.NextSample();
        float ch4_sample = m_Channel4SoundGenerator.NextSample();

        // Left channel "SO1"
        float so1 = 0.0;
        if (OutputChannel1ToSO1) so1 += ch1_sample;
        if (OutputChannel2ToSO1) so1 += ch2_sample;
        if (OutputChannel3ToSO1) so1 += ch3_sample;
        if (OutputChannel4ToSO1) so1 += ch4_sample;
        so1 *= ((float)OutputLevelSO1 / 7); // 0 = mute; 7 = max volume

        // Right channel "SO2"
        float so2 = 0.0;
        if (OutputChannel1ToSO2) so2 += ch1_sample;
        if (OutputChannel2ToSO2) so2 += ch2_sample;
        if (OutputChannel3ToSO2) so2 += ch3_sample;
        if (OutputChannel4ToSO2) so2 += ch4_sample;
        so2 *= ((float)OutputLevelSO2 / 7); // 0 = mute; 7 = max volume

        float f_frame[2] = {so1, so2};
        m_OutputBuffer.Put((Uint8*) f_frame);
    }
}

void APU::Channel1Callback(Uint8* pStream, int length)
{
    SDL_memset(pStream, 0x00, length);

    if (!ISBITSET(m_SoundOnOff, 7))
        return;

    for (int index = 0; index < length; index += FrameSizeBytes)
    {
        Uint8* frame = m_OutputBuffer.Get();
        for (int i = 0; i < FrameSizeBytes; i++) 
        {
            pStream[index + i] = frame[i];
        }
    }
}

void APU::Channel2Callback(Uint8* pStream, int length)
{
    SDL_memset(pStream, 0x00, length);

    if (!ISBITSET(m_SoundOnOff, 7))
        return;

    /*int v = 0;
    for (int index = 0; index < length; index++)
    {
        pStream[index] = (Uint8)(0xFF * std::sin(v * 2 * M_PI / 44100));
        v += 400;
    }*/
}

void APU::Channel3Callback(Uint8* pStream, int length)
{
    SDL_memset(pStream, 0x00, length);

    if (!ISBITSET(m_SoundOnOff, 7))
        return;

    /*int v = 0;
    for (int index = 0; index < length; index++)
    {
        pStream[index] = (Uint8)(0xFF * std::sin(v * 2 * M_PI / 44100));
        v += 500;
    }*/
}

void APU::Channel4Callback(Uint8* pStream, int length)
{
    SDL_memset(pStream, 0x00, length);

    if (!ISBITSET(m_SoundOnOff, 7))
        return;

    /*int v = 0;
    for (int index = 0; index < length; index++)
    {
        pStream[index] = (Uint8)(0xFF * std::sin(v * 2 * M_PI / 44100));
        v += 600;
    }*/
}

// IMemoryUnit
byte APU::ReadByte(const ushort& address)
{
    if ((address >= 0xFF30) && (address <= 0xFF3F))
    {
        return m_WavePatternRAM[address - 0xFF30];
    }

    switch (address)
    {
    case Channel1Sweep:
        return m_Channel1Sweep;
    case Channel1LengthWavePatternDuty:
        return m_Channel1SoundLength;
    case Channel1VolumeEnvelope:
        return m_Channel1VolumeEnvelope;
    case Channel1FrequencyLo:
        // Technically write only
        return m_Channel1FrequencyLo;
    case Channel1FrequencyHi:
        return m_Channel1FrequencyHi;
    case Channel2LengthWavePatternDuty:
        return m_Channel2SoundLength;
    case Channel2VolumeEnvelope:
        return m_Channel2VolumeEnvelope;
    case Channel2FrequnecyLo:
        // Technically write only
        return m_Channel2FrequencyLo;
    case Channel2FrequencyHi:
        return m_Channel2FrequencyHi;
    case Channel3OnOff:
        return m_Channel3SoundOnOff;
    case Channel3Length:
        return m_Channel3SoundLength;
    case Channel3OutputLevel:
        return m_Channel3SelectOutputLevel;
    case Channel3FrequencyLower:
        // Technically write only
        return m_Channel3FrequencyLo;
    case Channel3FrequnecyHigher:
        return m_Channel3FrequencyHi;
    case Channel4Length:
        return m_Channel4SoundLength;
    case Channel4VolumeEnvelope:
        return m_Channel4VolumeEnvelope;
    case Channel4PolynomialCounter:
        return m_Channel4PolynomialCounter;
    case Channel4Counter:
        return m_Channel4Counter;
    case ChannelControl:
        return m_ChannelControlOnOffVolume;
    case OutputTerminalSelection:
        return m_OutputTerminal;
    case SoundOnOff:
        return m_SoundOnOff;
    default:
        Logger::Log("APU::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool APU::WriteByte(const ushort& address, const byte val)
{
    if ((address >= 0xFF30) && (address <= 0xFF3F))
    {
        m_WavePatternRAM[address - 0xFF30] = val;
        return true;
    }

    switch (address)
    {
    case Channel1Sweep:
        m_Channel1Sweep = val;
        return true;
    case Channel1LengthWavePatternDuty:
        m_Channel1SoundLength = val;
        return true;
    case Channel1VolumeEnvelope:
        m_Channel1VolumeEnvelope = val;
        return true;
    case Channel1FrequencyLo:
        m_Channel1FrequencyLo = val;
        return true;
    case Channel1FrequencyHi:
        m_Channel1FrequencyHi = val;
        return true;
    case Channel2LengthWavePatternDuty:
        m_Channel2SoundLength = val;
        return true;
    case Channel2VolumeEnvelope:
        m_Channel2VolumeEnvelope = val;
        return true;
    case Channel2FrequnecyLo:
        m_Channel2FrequencyLo = val;
        return true;
    case Channel2FrequencyHi:
        m_Channel2FrequencyHi = val;
        return true;
    case Channel3OnOff:
        m_Channel3SoundOnOff = val;
        return true;
    case Channel3Length:
        m_Channel3SoundLength = val;
        return true;
    case Channel3OutputLevel:
        m_Channel3SelectOutputLevel = val;
        return true;
    case Channel3FrequencyLower:
        m_Channel3FrequencyLo = val;
        return true;
    case Channel3FrequnecyHigher:
        m_Channel3FrequencyHi = val;
        return true;
    case Channel4Length:
        m_Channel4SoundLength = val;
        return true;
    case Channel4VolumeEnvelope:
        m_Channel4VolumeEnvelope = val;
        return true;
    case Channel4PolynomialCounter:
        m_Channel4PolynomialCounter = val;
        return true;
    case Channel4Counter:
        m_Channel4Counter = val;
        return true;
    case ChannelControl:
        m_ChannelControlOnOffVolume = val;
        return true;
    case OutputTerminalSelection:
        m_OutputTerminal = val;
        return true;
    case SoundOnOff:
        /*
        Bit 7 - All sound on/off  (0: stop all sound circuits) (Read/Write)
        Bit 3 - Sound 4 ON flag (Read Only)
        Bit 2 - Sound 3 ON flag (Read Only)
        Bit 1 - Sound 2 ON flag (Read Only)
        Bit 0 - Sound 1 ON flag (Read Only)
        */
        m_SoundOnOff = val & 0x80;
        return true;
    default:
        Logger::Log("APU::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}

void APU::LoadChannel(int index, SDL_AudioCallback callback)
{
    SDL_AudioSpec want, have;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = AudioSampleRate;
    want.format = AUDIO_F32;
    want.channels = AudioOutChannelCount;
    want.samples = 1024 * AudioOutChannelCount;
    want.callback = callback;
    want.userdata = this;

    m_DeviceChannel[index] = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (m_DeviceChannel[index] == 0)
    {
        Logger::Log("[SDL] Failed to open audio device for channel %d - %s", index + 1, SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(m_DeviceChannel[index], 0);
    m_Initialized[index] = true;
}

APU::AdditiveSquareWaveGenerator::AdditiveSquareWaveGenerator() : 
    m_FrequencyHz(1.0),
    m_DutyCycle(50.0),
    m_CounterModeEnabled(false),
    m_SoundLengthSeconds(0.0),
    m_EnvelopeModeEnabled(false),
    m_EnvelopeDirection(1.0),
    m_EnvelopeStartVolume(0.0),
    m_EnvelopeStepLengthSeconds(0.0),
    m_HarmonicsCount(0),
    m_Phase(0.0),
    m_SoundLengthTimerSeconds(0.0)
{
    memset(m_Coefficients, 0.0, ARRAYSIZE(m_Coefficients));
}

void APU::AdditiveSquareWaveGenerator::DebugLog() {
    Logger::Log("Freq %f Duty %f Phase %f TimeRemain %f EnvStVol %f",
        m_FrequencyHz,
        m_DutyCycle,
        m_Phase,
        (m_SoundLengthSeconds - m_SoundLengthTimerSeconds),
        m_EnvelopeStartVolume);
}

void APU::AdditiveSquareWaveGenerator::RegenerateCoefficients()
{
    // Keep the upper harmonic below the Nyquist frequency
    m_HarmonicsCount = AudioSampleRate / (m_FrequencyHz * 2);
    if (m_HarmonicsCount > MaxHarmonicsCount)
    {
        m_HarmonicsCount = MaxHarmonicsCount;
    }

    // Generate the coefficients for each harmonic
    m_Coefficients[0] = m_DutyCycle - 0.5;
    for (int i = 1; i < m_HarmonicsCount; i++)
    {
        m_Coefficients[i] = (sin(i * m_DutyCycle * PI) * 2) / (i * PI);
    }
}

void APU::AdditiveSquareWaveGenerator::SetFrequency(double frequency_hz)
{
    m_FrequencyHz = frequency_hz;

    if (m_FrequencyHz <= 0)
    {
        Logger::LogError("Invalid Frequency %f", m_FrequencyHz);
        assert(false);
    }

    RegenerateCoefficients();
}

void APU::AdditiveSquareWaveGenerator::SetDutyCycle(double duty_cycle)
{
    m_DutyCycle = duty_cycle;

    if (m_DutyCycle < 0.0 || m_DutyCycle > 1.0)
    {
        Logger::LogError("Invalid Duty Cycle %f", m_DutyCycle);
        assert(false);
    }

    RegenerateCoefficients();
}

void APU::AdditiveSquareWaveGenerator::SetCounterModeEnabled(bool is_enabled)
{
    m_CounterModeEnabled = is_enabled;
}

void APU::AdditiveSquareWaveGenerator::SetSoundLength(double sound_length_seconds)
{
    m_SoundLengthSeconds = sound_length_seconds;
}

void APU::AdditiveSquareWaveGenerator::SetEnvelopeStartVolume(double envelope_start_volume)
{
    m_EnvelopeStartVolume = envelope_start_volume;
}

void APU::AdditiveSquareWaveGenerator::SetEnvelopeDirection(EnvelopeDirection direction)
{
    m_EnvelopeDirection = direction == UP ? 1.0 : -1.0;
}

void APU::AdditiveSquareWaveGenerator::SetEnvelopeStep(double envelope_step_seconds)
{
    m_EnvelopeStepLengthSeconds = envelope_step_seconds;
    m_EnvelopeModeEnabled = !(envelope_step_seconds == 0.0);
}

void APU::AdditiveSquareWaveGenerator::RestartSound()
{
    m_SoundLengthTimerSeconds = 0.0;
}

float APU::AdditiveSquareWaveGenerator::NextSample()
{
    float sample = 0.0;

    bool sound_enabled = m_EnvelopeStartVolume != 0.0 && (!m_CounterModeEnabled || m_SoundLengthTimerSeconds < m_SoundLengthSeconds);

    if (sound_enabled)
    {
        for (int j = 0; j < m_HarmonicsCount; j++)
        {
            sample += m_Coefficients[j] * cos(j * m_Phase);
        }

        if (m_EnvelopeModeEnabled)
        {
            int step_number = m_SoundLengthTimerSeconds / m_EnvelopeStepLengthSeconds;
            double volume = m_EnvelopeStartVolume + (m_EnvelopeDirection * ((double)step_number / 16.0));
            if (volume < 0.0)
                volume = 0.0;
            if (volume > 1.0)
                volume = 1.0;
            sample *= volume;
        }
    }

    m_Phase += (TWO_PI * m_FrequencyHz) / (double)AudioSampleRate;
    while (m_Phase >= TWO_PI)
    {
        m_Phase -= TWO_PI;
    }

    if (m_Phase < 0 || m_Phase >= TWO_PI)
    {
        Logger::LogError("Invalid phase %f", m_Phase);
        assert(false);
    }

    m_SoundLengthTimerSeconds += 1.0 / (double)AudioSampleRate;

    return sample;
}

APU::NoiseGenerator::NoiseGenerator() : 
    m_FrequencyHz(1.0),
    m_CounterModeEnabled(false),
    m_SoundLengthSeconds(0.0),
    m_EnvelopeModeEnabled(false),
    m_EnvelopeDirection(1.0),
    m_EnvelopeStartVolume(0.0),
    m_EnvelopeStepLengthSeconds(0.0),
    m_Phase(0.0),
    m_Signal(0.0),
    m_SoundLengthTimerSeconds(0.0)
{
    // TODO
}

void APU::NoiseGenerator::DebugLog() {
    // TODO
}

void APU::NoiseGenerator::SetFrequency(double frequency_hz) {
    m_FrequencyHz = frequency_hz;    
    
    if (m_FrequencyHz <= 0)
    {
        Logger::LogError("Invalid Frequency %f", m_FrequencyHz);
        assert(false);
    }
}

void APU::NoiseGenerator::SetCounterModeEnabled(bool is_enabled) {
    m_CounterModeEnabled = is_enabled;
}

void APU::NoiseGenerator::SetSoundLength(double sound_length_seconds) {
    m_SoundLengthSeconds = sound_length_seconds;
}

void APU::NoiseGenerator::SetEnvelopeStartVolume(double envelope_start_volume) {
    m_EnvelopeStartVolume = envelope_start_volume;
}

void APU::NoiseGenerator::SetEnvelopeDirection(EnvelopeDirection direction) {
    m_EnvelopeDirection = direction == UP ? 1.0 : -1.0;
}

void APU::NoiseGenerator::SetEnvelopeStep(double envelope_step_seconds) {
    m_EnvelopeStepLengthSeconds = envelope_step_seconds;
    m_EnvelopeModeEnabled = !(envelope_step_seconds == 0.0);
}

void APU::NoiseGenerator::RestartSound() {
    m_SoundLengthTimerSeconds = 0.0;
}

float APU::NoiseGenerator::NextSample() {
    float sample = 0.0;

    bool sound_enabled = m_EnvelopeStartVolume != 0.0 && (!m_CounterModeEnabled || m_SoundLengthTimerSeconds < m_SoundLengthSeconds);
    
    if (m_Phase >= 1.0) {
        m_Phase -= 1.0;
        float r = rand() / (RAND_MAX + 1.0);
        m_Signal = r > 0.5 ? 0.5 : -0.5;
    }

    if (sound_enabled) {
        sample = m_Signal;

        if (m_EnvelopeModeEnabled)
        {
            int step_number = m_SoundLengthTimerSeconds / m_EnvelopeStepLengthSeconds;
            double volume = m_EnvelopeStartVolume + (m_EnvelopeDirection * ((double)step_number / 16.0));
            if (volume < 0.0)
                volume = 0.0;
            if (volume > 1.0)
                volume = 1.0;
            sample *= volume;
        }
    }

    m_Phase += m_FrequencyHz / (double)AudioSampleRate;

    m_SoundLengthTimerSeconds += 1.0 / (double)AudioSampleRate;

    return sample;
}

APU::WaveformGenerator::WaveformGenerator() : 
    m_Enabled(true),
    m_FrequencyHz(1.0),
    m_CounterModeEnabled(false),
    m_SoundLengthSeconds(0.0),
    m_OutputLevel(1.0),
    m_Phase(0.0),
    m_SoundLengthTimerSeconds(0.0),
    m_WaveBuffer(nullptr)
{
    // TODO   
}

void APU::WaveformGenerator::DebugLog() 
{

}
    
void APU::WaveformGenerator::SetFrequency(double frequency_hz) 
{
    m_FrequencyHz = frequency_hz;    

    if (m_FrequencyHz <= 0)
    {
        Logger::LogError("Invalid Frequency %f", m_FrequencyHz);
        assert(false);
    }
}

void APU::WaveformGenerator::SetOutputLevel(double level) 
{
    m_OutputLevel = level;

    if (m_OutputLevel > 1.0 || m_OutputLevel < 0.0) {
        Logger::LogError("Invalid Output Level %f", m_OutputLevel);
        assert(false);
    }
}

void APU::WaveformGenerator::SetCounterModeEnabled(bool is_enabled) 
{
    m_CounterModeEnabled = is_enabled;
}

void APU::WaveformGenerator::SetSoundLength(double sound_length_seconds) 
{
    m_SoundLengthSeconds = sound_length_seconds;
}

void APU::WaveformGenerator::SetWaveRamLocation(byte* wave_buffer) 
{
    m_WaveBuffer = wave_buffer;
}

void APU::WaveformGenerator::Enable() 
{
    m_Enabled = true;
}

void APU::WaveformGenerator::Disable() 
{
    m_Enabled = false;
}

void APU::WaveformGenerator::RestartSound() 
{
    m_SoundLengthTimerSeconds = 0.0;
}

float APU::WaveformGenerator::NextSample() 
{
    float sample = 0.0;

    if (m_Enabled) {
        bool sound_enabled = m_OutputLevel != 0.0 && (!m_CounterModeEnabled || m_SoundLengthTimerSeconds < m_SoundLengthSeconds);
        
        if (sound_enabled) {
            // Wave is made up of 32 4-bit samples
            int wave_ram_sample_number = (int)(m_Phase * 32.0);
            int wave_ram_byte_offset = wave_ram_sample_number / 2;
            int shift = ((1 + wave_ram_sample_number) % 2) * 4; // shift 0 or 4 bits.
            int wave_ram_sample = (m_WaveBuffer[wave_ram_byte_offset] >> shift) & 0xF;
            sample = ((double)wave_ram_sample / 16.0) - 0.5;

            // TODO: Emulate bit-shift rather than scaling based sound level
            sample *= m_OutputLevel;
        }

        m_Phase += m_FrequencyHz / (double)AudioSampleRate;

        if (m_Phase >= 1.0) {
            m_Phase -= 1.0;
        }
    }

    m_SoundLengthTimerSeconds += 1.0 / (double)AudioSampleRate;

    return sample;
}

APU::Buffer::Buffer(size_t element_count, size_t element_size) {
    m_ElementCount = element_count + 1;
    m_ElementSize = element_size;
    m_BufferSize = m_ElementCount * m_ElementSize;
    m_Bytes = (Uint8*) malloc(m_BufferSize * sizeof(Uint8));
    if (m_Bytes == nullptr) {
        // TODO handle error
    }
    m_DefaultBytes = (Uint8*) malloc(m_ElementSize * sizeof(Uint8));
     if (m_DefaultBytes == nullptr) {
        // TODO handle error
    }
    memset(m_DefaultBytes, SILENCE, m_ElementSize);
    Reset();
}

APU::Buffer::~Buffer() {
    free(m_Bytes);
    free(m_DefaultBytes);
}

void APU::Buffer::Reset() {
    m_ReadIndex = 0;
    m_WriteIndex = 0;
    memset(m_Bytes, SILENCE, m_BufferSize * sizeof(Uint8));
}

void APU::Buffer::Put(Uint8* element) {
    memcpy(&m_Bytes[m_WriteIndex], element, m_ElementSize);
    AdvanceWriteIndex();
    if (m_WriteIndex == m_ReadIndex) {
        AdvanceReadIndex();
    }
}

Uint8* APU::Buffer::Get() {
    Uint8* data;
    if (m_WriteIndex == m_ReadIndex) {
        data = m_DefaultBytes;
    } else {
        data = &m_Bytes[m_ReadIndex];
        AdvanceReadIndex();
    }
    return data;
}

void APU::Buffer::AdvanceReadIndex() {
    mutex.lock();
    m_ReadIndex = (m_ReadIndex + m_ElementSize) % m_BufferSize;
    mutex.unlock();
}

void APU::Buffer::AdvanceWriteIndex() {
    m_WriteIndex = (m_WriteIndex + m_ElementSize) % m_BufferSize;
}
