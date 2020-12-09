#include "pch.hpp"
#include "APU.hpp"
#include <cmath>

#define AudioSampleRate 48000
#define AudioOutChannelCount 2
#define FrameSizeBytes 8 // 32 bit samples * 2 channels

// Based on 60Hz screen refresh rate
#define AudioBufferSize ((AudioSampleRate * AudioOutChannelCount)) // 1 sec TODO: This is pretty big
#define CyclesPerFrame 70224 // TODO: Can we import this somehow?
#define CyclesPerSecond 4213440 // CyclesPerFrame * 60

#define Pi 3.141592653589793
#define TwoPi 6.283185307179586

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

#define Channel1SweepTime ((m_Channel1Sweep >> 4) & 7)
#define Channel1SweepDirection ((m_Channel1Sweep >> 3) & 1)
#define Channel1SweepShiftNumber (m_Channel1Sweep & 7)
#define Channel1WavePatternDuty ((m_Channel1SoundLength >> 6) & 3)
#define Channel1SoundLength ((m_Channel1SoundLength) & 0x3F)
#define Channel1VolumeEnvelopeStart ((m_Channel1VolumeEnvelope >> 4) & 0xF)
#define Channel1VolumeEnvelopeDirection ((m_Channel1VolumeEnvelope >> 3) & 1)
#define Channel1VolumeEnvelopeSweepNumber (m_Channel1VolumeEnvelope & 7)
#define Channel1Initial ISBITSET(m_Channel1FrequencyHi, 7)
#define Channel1CounterConsecutive ISBITSET(m_Channel1FrequencyHi, 6)
#define Channel1Frequency (((m_Channel1FrequencyHi << 8) | m_Channel1FrequencyLo) & 0x7FF)

#define Channel2WavePatternDuty ((m_Channel2SoundLength >> 6) & 3)
#define Channel2SoundLength ((m_Channel2SoundLength) & 0x3F)
#define Channel2VolumeEnvelopeStart ((m_Channel2VolumeEnvelope >> 4) & 0xF)
#define Channel2VolumeEnvelopeDirection ((m_Channel2VolumeEnvelope >> 3) & 1)
#define Channel2VolumeEnvelopeSweepNumber (m_Channel2VolumeEnvelope & 7)
#define Channel2Initial ISBITSET(m_Channel2FrequencyHi, 7)
#define Channel2CounterConsecutive ISBITSET(m_Channel2FrequencyHi, 6)
#define Channel2Frequency (((m_Channel2FrequencyHi << 8) | m_Channel2FrequencyLo) & 0x7FF)

#define Channel3SoundOnOff ISBITSET(m_Channel3SoundOnOff, 7)
#define Channel3SoundLength m_Channel3SoundLength
#define Channel3SelectOutputLevel ((m_Channel3SelectOutputLevel >> 5) & 0x3)
#define Channel3Initial ISBITSET(m_Channel3FrequencyHi, 7)
#define Channel3CounterConsecutive ISBITSET(m_Channel3FrequencyHi, 6)
#define Channel3Frequency (((m_Channel3FrequencyHi << 8) | m_Channel3FrequencyLo) & 0x7FF)

#define Channel4SoundLength (m_Channel4SoundLength & 0x1F)
#define Channel4VolumeEnvelopeStart ((m_Channel4VolumeEnvelope >> 4) & 0xF)
#define Channel4VolumeEnvelopeDirection ((m_Channel4VolumeEnvelope >> 3) & 1)
#define Channel4VolumeEnvelopeSweepNumber (m_Channel4VolumeEnvelope & 7)
#define Channel4ShiftClockFrequency ((m_Channel4PolynomialCounter >> 4) & 0xF)
#define Channel4CounterStep ((m_Channel4PolynomialCounter >> 3) & 1)
#define Channel4FrequencyDivideRatio (m_Channel4PolynomialCounter & 7)
#define Channel4Initial ISBITSET(m_Channel4Counter, 7)
#define Channel4CounterConsecutive ISBITSET(m_Channel4Counter, 6)

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

void AudioDeviceCallbackStatic(void* pUserdata, Uint8* pStream, int length)
{
    reinterpret_cast<APU*>(pUserdata)->AudioDeviceCallback(
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
    m_Channel1SoundGenerator(
        CHANNEL_1,
        &m_Channel1Sweep,
        &m_Channel1SoundLength,
        &m_Channel1VolumeEnvelope,
        &m_Channel1FrequencyLo,
        &m_Channel1FrequencyHi,
        &m_SoundOnOff
    ),
    m_Channel2SoundGenerator(
        CHANNEL_2,
        nullptr, // No sweep register on CH2
        &m_Channel2SoundLength,
        &m_Channel2VolumeEnvelope,
        &m_Channel2FrequencyLo,
        &m_Channel2FrequencyHi,
        &m_SoundOnOff
    ),
    m_Channel3SoundGenerator(),
    m_Channel4SoundGenerator(),
    m_Channel1RequiresUpdate(false),
    m_Channel2RequiresUpdate(false),
    m_Channel3RequiresUpdate(false),
    m_Channel4RequiresUpdate(false),
    m_Initialized(false),
    m_AudioDevice(0),
    m_AudioFrameRemainder(0.0),
    m_OutputBuffer(AudioBufferSize, FrameSizeBytes)
{
    memset(m_WavePatternRAM, 0x00, ARRAYSIZE(m_WavePatternRAM));

    m_Channel3SoundGenerator.SetWaveRamLocation(m_WavePatternRAM);

    if (SDL_Init(SDL_INIT_AUDIO))
    {
        Logger::LogError("[SDL] Failed to initialize: %s", SDL_GetError());
    }
    else
    {
        LoadAudioDevice(AudioDeviceCallbackStatic);
    }
}

APU::~APU()
{
    if (m_Initialized)
    {
        SDL_PauseAudioDevice(m_AudioDevice, 1);
        SDL_CloseAudioDevice(m_AudioDevice);
    }

    SDL_Quit();
}

void APU::Step(unsigned long cycles)
{
    if (m_Channel3RequiresUpdate) UpdateChannel3Generator();
    if (m_Channel4RequiresUpdate) UpdateChannel4Generator();

    // Calculate the number of audio frames to generate for the elapsed CPU cycle count
    double sample_count = m_AudioFrameRemainder + (((double)AudioSampleRate / (double)CyclesPerSecond) * (double)cycles);
    double int_part = 0.0;
    m_AudioFrameRemainder = modf(sample_count, &int_part);

    // Compute the next audio sample(s)
    for (int i = 0; i < int_part; i++)
    {
        float ch1_sample = m_Channel1SoundGenerator.NextSample();
        float ch2_sample = m_Channel2SoundGenerator.NextSample();
        float ch3_sample = m_Channel3SoundGenerator.NextSample();
        float ch4_sample = m_Channel4SoundGenerator.NextSample();

        // Left channel "SO1"
        float so1 = 0.0;
        if (OutputChannel1ToSO1)
            so1 += ch1_sample;
        if (OutputChannel2ToSO1)
            so1 += ch2_sample;
        if (OutputChannel3ToSO1)
            so1 += ch3_sample;
        if (OutputChannel4ToSO1)
            so1 += ch4_sample;
        so1 *= ((float)OutputLevelSO1 / 7); // 0 = mute; 7 = max volume

        // Right channel "SO2"
        float so2 = 0.0;
        if (OutputChannel1ToSO2)
            so2 += ch1_sample;
        if (OutputChannel2ToSO2)
            so2 += ch2_sample;
        if (OutputChannel3ToSO2)
            so2 += ch3_sample;
        if (OutputChannel4ToSO2)
            so2 += ch4_sample;
        so2 *= ((float)OutputLevelSO2 / 7); // 0 = mute; 7 = max volume

        float f_frame[2] = {so1, so2};
        m_OutputBuffer.Put((Uint8 *)f_frame);
    }
}

void APU::UpdateChannel3Generator()
{
    if (Channel3SoundOnOff)
    {
        m_Channel3SoundGenerator.Enable();
    }
    else
    {
        m_Channel3SoundGenerator.Disable();
    }

    if (Channel3Initial)
    {
        m_Channel3SoundGenerator.RestartSound();
    }

    // Counter/Consecutive modes
    m_Channel3SoundGenerator.SetCounterModeEnabled(Channel3CounterConsecutive);

    // Sound Length = (256-t1)*(1/256) seconds
    m_Channel3SoundGenerator.SetSoundLength((256.0 - (double)Channel3SoundLength) / 256.0);

    // Sound output level
    switch (Channel3SelectOutputLevel)
    {
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
    
    m_Channel3RequiresUpdate = false;

    Logger::Log("CHANNEL 3 -- ON/OFF:0x%02x LEN:0x%01x LVL:0x%01x INIT:0x%01x CC:0x%01x FREQ:0x%03x DATA:0x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
        Channel3SoundOnOff,
        Channel3SoundLength,
        Channel3SelectOutputLevel,
        Channel3Initial,
        Channel3CounterConsecutive,
        Channel3Frequency,
        (m_WavePatternRAM[0] >> 4) & 0xF,
        m_WavePatternRAM[0] & 0xF,
        (m_WavePatternRAM[1] >> 4) & 0xF,
        m_WavePatternRAM[1] & 0xF,
        (m_WavePatternRAM[2] >> 4) & 0xF,
        m_WavePatternRAM[2] & 0xF,
        (m_WavePatternRAM[3] >> 4) & 0xF,
        m_WavePatternRAM[3] & 0xF,
        (m_WavePatternRAM[4] >> 4) & 0xF,
        m_WavePatternRAM[4] & 0xF,
        (m_WavePatternRAM[5] >> 4) & 0xF,
        m_WavePatternRAM[5] & 0xF,
        (m_WavePatternRAM[6] >> 4) & 0xF,
        m_WavePatternRAM[6] & 0xF,
        (m_WavePatternRAM[7] >> 4) & 0xF,
        m_WavePatternRAM[7] & 0xF,
        (m_WavePatternRAM[8] >> 4) & 0xF,
        m_WavePatternRAM[8] & 0xF,
        (m_WavePatternRAM[9] >> 4) & 0xF,
        m_WavePatternRAM[9] & 0xF,
        (m_WavePatternRAM[10] >> 4) & 0xF,
        m_WavePatternRAM[10] & 0xF,
        (m_WavePatternRAM[11] >> 4) & 0xF,
        m_WavePatternRAM[11] & 0xF,
        (m_WavePatternRAM[12] >> 4) & 0xF,
        m_WavePatternRAM[12] & 0xF,
        (m_WavePatternRAM[13] >> 4) & 0xF,
        m_WavePatternRAM[13] & 0xF,
        (m_WavePatternRAM[14] >> 4) & 0xF,
        m_WavePatternRAM[14] & 0xF,
        (m_WavePatternRAM[15] >> 4) & 0xF,
        m_WavePatternRAM[15] & 0xF);
}

void APU::UpdateChannel4Generator()
{
    if (Channel4Initial)
    {
        m_Channel4SoundGenerator.RestartSound();
    }

    // Counter/Consecutive modes
    m_Channel4SoundGenerator.SetCounterModeEnabled(Channel4CounterConsecutive);

    // Sound Length = (64-t1)*(1/256) seconds
    m_Channel4SoundGenerator.SetSoundLength((64.0 - (double)Channel4SoundLength) / 256.0);

    // Envelope start volume. 4 bits of precision = 16 volume levels.
    m_Channel4SoundGenerator.SetEnvelopeStartVolume((double)Channel4VolumeEnvelopeStart / 16.0);

    // Length of an envelope step = n * (1/64)
    m_Channel4SoundGenerator.SetEnvelopeStepLength((double)Channel4VolumeEnvelopeSweepNumber / 64.0);

    // Envelope direction: 0 = Decrease, 1 = Increase
    m_Channel4SoundGenerator.SetEnvelopeDirection(Channel4VolumeEnvelopeDirection ? UP : DOWN);

    // 524288 Hz / r / 2^(s+1)
    double divide_ratio = Channel4FrequencyDivideRatio == 0 ? 0.5 : (double)Channel4FrequencyDivideRatio;
    m_Channel4SoundGenerator.SetFrequency(524288.0 / divide_ratio / (double)(2 << Channel4ShiftClockFrequency));

    m_Channel4RequiresUpdate = false;

    Logger::Log("CHANNEL 4 -- LEN:0x%02x ENVST:0x%01x ENVDIR:0x%01x ENVNUM:0x%01x SHIFT:0x%01x STEP:0x%01x RATIO:0x%01x INIT:0x%01x CC:0x%01x",
    Channel4SoundLength,
    Channel4VolumeEnvelopeStart,
    Channel4VolumeEnvelopeDirection,
    Channel4VolumeEnvelopeSweepNumber,
    Channel4ShiftClockFrequency,
    Channel4CounterStep,
    Channel4FrequencyDivideRatio,
    Channel4Initial,
    Channel4CounterConsecutive);
}

void APU::AudioDeviceCallback(Uint8* pStream, int length)
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

        // TODO: remove
        m_Channel3RequiresUpdate = true;

        return true;
    }

    if ((address >= 0xFF10) && (address <= 0xFF14))
    {
        // TODO: remove
        m_Channel1RequiresUpdate = true;
    }

    if ((address >= 0xFF16) && (address <= 0xFF19))
    {
        // TODO: remove
        m_Channel2RequiresUpdate = true;
    }

    if ((address >= 0xFF1A) && (address <= 0xFF1E))
    {
        // TODO: remove
        m_Channel3RequiresUpdate = true;
    }

    if ((address >= 0xFF20) && (address <= 0xFF23))
    {
        // TODO: remove
        m_Channel4RequiresUpdate = true;
    }

    switch (address)
    {
    case Channel1Sweep:
        m_Channel1Sweep = val;
        m_Channel1SoundGenerator.TriggerSweepRegisterUpdate();
        return true;
    case Channel1LengthWavePatternDuty:
        m_Channel1SoundLength = val;
        m_Channel1SoundGenerator.TriggerSoundLengthRegisterUpdate();
        return true;
    case Channel1VolumeEnvelope:
        m_Channel1VolumeEnvelope = val;
        m_Channel1SoundGenerator.TriggerVolumeEnvelopeRegisterUpdate();
        return true;
    case Channel1FrequencyLo:
        m_Channel1FrequencyLo = val;
        m_Channel1SoundGenerator.TriggerFrequencyLoRegisterUpdate();
        return true;
    case Channel1FrequencyHi:
        m_Channel1FrequencyHi = val;
        m_Channel1SoundGenerator.TriggerFrequencyHiRegisterUpdate();

        // TODO: remove
        m_Channel1Reset = Channel1Initial;

        return true;
    case Channel2LengthWavePatternDuty:
        m_Channel2SoundLength = val;
        m_Channel2SoundGenerator.TriggerSoundLengthRegisterUpdate();
        return true;
    case Channel2VolumeEnvelope:
        m_Channel2VolumeEnvelope = val;
        m_Channel2SoundGenerator.TriggerVolumeEnvelopeRegisterUpdate();
        return true;
    case Channel2FrequnecyLo:
        m_Channel2FrequencyLo = val;
        m_Channel2SoundGenerator.TriggerFrequencyLoRegisterUpdate();
        return true;
    case Channel2FrequencyHi:
        m_Channel2FrequencyHi = val;
        m_Channel2SoundGenerator.TriggerFrequencyHiRegisterUpdate();
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

void APU::LoadAudioDevice(SDL_AudioCallback callback)
{
    SDL_AudioSpec want, have;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = AudioSampleRate;
    want.format = AUDIO_F32;
    want.channels = AudioOutChannelCount;
    want.samples = 1024 * AudioOutChannelCount;
    want.callback = callback;
    want.userdata = this;

    m_AudioDevice = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (m_AudioDevice == 0)
    {
        Logger::Log("[SDL] Failed to open audio device - %s", SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(m_AudioDevice, 0);
    m_Initialized = true;
}

APU::RegisterAwareSquareWaveGenerator::RegisterAwareSquareWaveGenerator(
    const APUChannel channel,
    const byte *sweepRegister,
    const byte *soundLengthRegister,
    const byte *volumeEnvelopeRegister,
    const byte *frequencyLoRegister,
    const byte *frequencyHiRegister,
    const byte *soundOnOffRegister)
    : m_Channel(channel),
      m_SweepRegister(sweepRegister),
      m_SoundLengthRegister(soundLengthRegister),
      m_VolumeEnvelopeRegister(volumeEnvelopeRegister),
      m_FrequencyLoRegister(frequencyLoRegister),
      m_FrequencyHiRegister(frequencyHiRegister),
      m_SoundOnOffRegister(soundOnOffRegister),
      m_FrequencyHz(1.0),
      m_DutyCycle(50.0),
      m_CounterModeEnabled(false),
      m_SoundLengthSeconds(0.0),
      m_SweepModeEnabled(false),
      m_SweepDirection(1.0),
      m_SweepShiftFrequencyExponent(0),
      m_EnvelopeModeEnabled(false),
      m_EnvelopeDirection(1.0),
      m_EnvelopeStartVolume(0.0),
      m_EnvelopeStepLengthSeconds(0.0),
      m_HarmonicsCount(0),
      m_Phase(0.0),
      m_SoundLengthTimerSeconds(0.0),
      m_ChannelIsPlaying(false)
{
    memset(m_Coefficients, 0.0, ARRAYSIZE(m_Coefficients));
}

void APU::RegisterAwareSquareWaveGenerator::TriggerSweepRegisterUpdate()
{
    byte sweep_register = *m_SweepRegister;
    
    // Bit 2-0 Sweep shift number
    // Shift value to calculate frequency step amount
    m_SweepShiftFrequencyExponent = sweep_register & 7;

    // Bit 3 - Sweep Direction - 0 = Up, 1 = Down
    m_SweepDirection = ISBITSET(sweep_register, 3) ? -1.0 : 1.0;

    // Bit 6-4 - Sweep Time - When sweep time is zero, disable sweep mode.
    // Length of a sweep step = n * (1/128)
    byte sweep_time = (sweep_register >> 4) & 7;
    m_SweepStepLengthSeconds = (double)sweep_time / 128.0;
    m_SweepModeEnabled = sweep_time != 0;

    // TODO: I believe the sweep function has it's own timer separate from the sound length timer - add that.
}

void APU::RegisterAwareSquareWaveGenerator::TriggerSoundLengthRegisterUpdate()
{
    byte sound_length_register = *m_SoundLengthRegister;

    // Bit 5-0 - Sound Length
    // Sound Length = (64-n)*(1/256) seconds
    byte sound_length = sound_length_register & 0x3F;
    m_SoundLengthSeconds = (64.0 - (double)sound_length) / 256.0;

    // Bit 7-6 - Wave Pattern Duty
    // 00: 12.5%
    // 01: 25%
    // 10: 50%
    // 11: 75%
    byte wave_pattern_duty = (sound_length_register >> 6) & 3;
    switch (wave_pattern_duty)
    {
    case 0:
        m_DutyCycle = 0.125;
        break;
    case 1:
        m_DutyCycle = 0.25;
        break;
    case 2:
        m_DutyCycle = 0.5;
        break;
    case 3:
        m_DutyCycle = 0.75;
        break;
    default:
        Logger::LogError("Invalid duty cycle %x", wave_pattern_duty);
        assert(false);
    }

    // Duty cycle update requires update of coefficients
    RegenerateCoefficients();
}

void APU::RegisterAwareSquareWaveGenerator::TriggerVolumeEnvelopeRegisterUpdate()
{
    byte volume_envelope_register = *m_VolumeEnvelopeRegister;

    // Bit 2-0 - Envelope step length number - When 0, disable envelope mode
    // Length of an envelope step = n * (1/64)
    byte envelope_step_length_number = volume_envelope_register & 7;
    m_EnvelopeStepLengthSeconds = (double)envelope_step_length_number / 64.0;
    m_EnvelopeModeEnabled = envelope_step_length_number != 0;

    // Bit 3 - Envelope direction - 0 = Decrease, 1 = Increase
    m_EnvelopeDirection = ISBITSET(volume_envelope_register, 3) ? 1.0 : -1.0;

    // Bit 7-4 - Initial volume envelope - When 0, sound is muted
    // Envelope start volume. 4 bits of precision = 16 volume levels.
    byte initial_envelope_volume = (volume_envelope_register >> 4) & 0xF;
    m_EnvelopeStartVolume = (double)initial_envelope_volume / 16.0;
}

void APU::RegisterAwareSquareWaveGenerator::TriggerFrequencyLoRegisterUpdate()
{
    // Bit 7-0 - Low 8 bits of the frequency data
    UpdateFrequency();
}

void APU::RegisterAwareSquareWaveGenerator::TriggerFrequencyHiRegisterUpdate()
{
    // Bit 2-0 - High 3 bits of the frequency data
    UpdateFrequency();

    byte frequency_hi_register = *m_FrequencyHiRegister;

    // Bit 6 - Counter/consecutive selection
    m_CounterModeEnabled = ISBITSET(frequency_hi_register, 6);

    // Bit 7 - Initial
    if (ISBITSET(frequency_hi_register, 7))
    {
        RestartSound();
    }
}

void APU::RegisterAwareSquareWaveGenerator::RestartSound()
{
    m_ChannelIsPlaying = true;

    // Reset the sound length timer
    m_SoundLengthTimerSeconds = 0.0;

    // Set sound on/off flag for this register
    byte sound_on_off_register = *m_SoundOnOffRegister;
    switch(m_Channel) {
        case CHANNEL_1:
            sound_on_off_register |= 0b0001;
            break;
        case CHANNEL_2:
            sound_on_off_register |= 0b0010;
            break;
        case CHANNEL_3:
            sound_on_off_register |= 0b0100;
            break;
        case CHANNEL_4:
            sound_on_off_register |= 0b1000;
    }
}

void APU::RegisterAwareSquareWaveGenerator::UpdateFrequency()
{
    byte frequency_lo_register = *m_FrequencyLoRegister;
    byte frequency_hi_register = *m_FrequencyHiRegister;

    // Frequency data (11 bits total)
    // Low 8 bits in Frequency Lo register
    // High 3 bits in Frequency Hi register bits 2-0
    // For x = the value in the frequency registers, the actual frequency
    // in Hz is 131072/(2048-x) Hz
    m_FrequencyHz = (131072.0 / (double)(2048 - (((frequency_hi_register << 8) | frequency_lo_register) & 0x7FF)));

    if (m_FrequencyHz <= 0)
    {
        Logger::LogError("Invalid Frequency %f", m_FrequencyHz);
        assert(false);
    }

    RegenerateCoefficients();
}

void APU::RegisterAwareSquareWaveGenerator::RegenerateCoefficients()
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
        m_Coefficients[i] = (sin(i * m_DutyCycle * Pi) * 2) / (i * Pi);
    }
}

float APU::RegisterAwareSquareWaveGenerator::NextSample()
{
    bool is_sound_playing = !m_CounterModeEnabled || m_SoundLengthTimerSeconds < m_SoundLengthSeconds;
    bool is_sound_enabled = m_EnvelopeStartVolume != 0.0 && is_sound_playing;

    if (!is_sound_playing && m_ChannelIsPlaying)
    {
        // sound has stopped playing
        m_ChannelIsPlaying = false;

        // Reset sound on/off flag for this register
        byte sound_on_off_register = *m_SoundOnOffRegister;
        switch(m_Channel) {
            case CHANNEL_1:
                sound_on_off_register &= ~0b0001;
                break;
            case CHANNEL_2:
                sound_on_off_register &= ~0b0010;
                break;
            case CHANNEL_3:
                sound_on_off_register &= ~0b0100;
                break;
            case CHANNEL_4:
                sound_on_off_register &= ~0b1000;
        }
    }

    float sample = 0.0;

    if (is_sound_enabled)
    {
        for (int j = 0; j < m_HarmonicsCount; j++)
        {
            sample += m_Coefficients[j] * cos(j * m_Phase);
        }
    }

    double volume = m_EnvelopeStartVolume;
    if (m_EnvelopeModeEnabled)
    {
        int step_number = (int)(m_SoundLengthTimerSeconds / m_EnvelopeStepLengthSeconds);
        volume += m_EnvelopeDirection * ((double)step_number / 16.0);
        if (volume < 0.0)
            volume = 0.0;
        if (volume > 1.0)
            volume = 1.0;
    }
    sample *= volume;

    double frequency = m_FrequencyHz;
    if (m_SweepModeEnabled)
    {
        byte frequency_lo_register = *m_FrequencyLoRegister;
        byte frequency_hi_register = *m_FrequencyHiRegister;
        uint frequency_shadow = (((frequency_hi_register << 8) | frequency_lo_register) & 0x7FF);
        int step_number = m_SoundLengthTimerSeconds / m_SweepStepLengthSeconds;

        // uint new_frequency = frequency_shadow;
        for (int i = 0; i < step_number; i++)
        {
            frequency_shadow -= (frequency_shadow >> m_SweepShiftFrequencyExponent);
        }

        // Recalculate the frequency - TODO: figure out how to de-duplicate this
        m_FrequencyHz = (131072.0 / (double)(2048 - frequency_shadow));

        if (m_FrequencyHz <= 0)
        {
            Logger::LogError("Invalid Frequency %f", m_FrequencyHz);
            assert(false);
        }

        RegenerateCoefficients();
    }

    // TODO - Remove
    assert(frequency > 0.0);
    assert(frequency < 999999999999999.0);

    m_Phase += (TwoPi * frequency) / (double)AudioSampleRate;
    while (m_Phase >= TwoPi)
    {
        m_Phase -= TwoPi;
    }

    if (m_Phase < 0 || m_Phase >= TwoPi)
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
}

void APU::NoiseGenerator::DebugLog()
{
}

void APU::NoiseGenerator::SetFrequency(double frequency_hz)
{
    m_FrequencyHz = frequency_hz;

    if (m_FrequencyHz <= 0)
    {
        Logger::LogError("Invalid Frequency %f", m_FrequencyHz);
        assert(false);
    }
}

void APU::NoiseGenerator::SetCounterModeEnabled(bool is_enabled)
{
    m_CounterModeEnabled = is_enabled;
}

void APU::NoiseGenerator::SetSoundLength(double sound_length_seconds)
{
    m_SoundLengthSeconds = sound_length_seconds;
}

void APU::NoiseGenerator::SetEnvelopeStartVolume(double envelope_start_volume)
{
    m_EnvelopeStartVolume = envelope_start_volume;
}

void APU::NoiseGenerator::SetEnvelopeDirection(EnvelopeDirection direction)
{
    m_EnvelopeDirection = direction == UP ? 1.0 : -1.0;
}

void APU::NoiseGenerator::SetEnvelopeStepLength(double envelope_step_seconds)
{
    m_EnvelopeStepLengthSeconds = envelope_step_seconds;
    m_EnvelopeModeEnabled = !(envelope_step_seconds == 0.0);
}

void APU::NoiseGenerator::RestartSound()
{
    m_SoundLengthTimerSeconds = 0.0;
}

float APU::NoiseGenerator::NextSample()
{
    float sample = 0.0;

    bool sound_enabled = m_EnvelopeStartVolume != 0.0 && (!m_CounterModeEnabled || m_SoundLengthTimerSeconds < m_SoundLengthSeconds);

    if (m_Phase >= 1.0)
    {
        m_Phase -= 1.0;
        float r = rand() / (RAND_MAX + 1.0);
        m_Signal = r > 0.5 ? 0.5 : -0.5;
    }

    if (sound_enabled)
    {
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

    if (m_OutputLevel > 1.0 || m_OutputLevel < 0.0)
    {
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

void APU::WaveformGenerator::SetWaveRamLocation(byte *wave_buffer)
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

    if (m_Enabled)
    {
        bool sound_enabled = m_OutputLevel != 0.0 && (!m_CounterModeEnabled || m_SoundLengthTimerSeconds < m_SoundLengthSeconds);

        if (sound_enabled)
        {
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

        if (m_Phase >= 1.0)
        {
            m_Phase -= 1.0;
        }
    }

    m_SoundLengthTimerSeconds += 1.0 / (double)AudioSampleRate;

    return sample;
}

APU::Buffer::Buffer(size_t element_count, size_t element_size)
{
    m_ElementCount = element_count + 1;
    m_ElementSize = element_size;
    m_BufferSize = m_ElementCount * m_ElementSize;
    m_Bytes = (Uint8 *)malloc(m_BufferSize * sizeof(Uint8));
    if (m_Bytes == nullptr)
    {
        // TODO handle error
    }
    m_DefaultBytes = (Uint8 *)malloc(m_ElementSize * sizeof(Uint8));
    if (m_DefaultBytes == nullptr)
    {
        // TODO handle error
    }
    memset(m_DefaultBytes, 0, m_ElementSize);
    Reset();
}

APU::Buffer::~Buffer()
{
    free(m_Bytes);
    free(m_DefaultBytes);
}

void APU::Buffer::Reset()
{
    m_ReadIndex = 0;
    m_WriteIndex = 0;
    memset(m_Bytes, 0, m_BufferSize * sizeof(Uint8));
}

void APU::Buffer::Put(Uint8 *element)
{
    memcpy(&m_Bytes[m_WriteIndex], element, m_ElementSize);
    AdvanceWriteIndex();
    if (m_WriteIndex == m_ReadIndex)
    {
        AdvanceReadIndex();
    }
}

Uint8 *APU::Buffer::Get()
{
    Uint8 *data;
    if (m_WriteIndex == m_ReadIndex)
    {
        data = m_DefaultBytes;
    }
    else
    {
        data = &m_Bytes[m_ReadIndex];
        AdvanceReadIndex();
    }
    return data;
}

void APU::Buffer::AdvanceReadIndex()
{
    mutex.lock();
    m_ReadIndex = (m_ReadIndex + m_ElementSize) % m_BufferSize;
    mutex.unlock();
}

void APU::Buffer::AdvanceWriteIndex()
{
    m_WriteIndex = (m_WriteIndex + m_ElementSize) % m_BufferSize;
}
