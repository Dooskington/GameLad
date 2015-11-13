#include "pch.hpp"
#include "APU.hpp"

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
    m_Channel3FreuqencyLo(0x00),
    m_Channel3FreuqencyHi(0x00),
    m_Channel4SoundLength(0x00),
    m_Channel4VolumeEnvelope(0x00),
    m_Channel4PolynomialCounter(0x00),
    m_Channel4Counter(0x00),
    m_ChannelControlOnOffVolume(0x00),
    m_OutputTerminal(0x00),
    m_SoundOnOff(0x00)
{
    memset(m_Initialized, false, ARRAYSIZE(m_Initialized));
    memset(m_DeviceChannel, 0, ARRAYSIZE(m_DeviceChannel));
    memset(m_WavePatternRAM, 0x00, ARRAYSIZE(m_WavePatternRAM));

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
    // TODO: Create audio here based on cycles, etc.
}

void APU::Channel1Callback(Uint8* pStream, int length)
{
    SDL_memset(pStream, 0x00, length);

    if (!ISBITSET(m_SoundOnOff, 7))
        return;

    /*int v = 0;
    for (int index = 0; index < length; index++)
    {
        pStream[index] = (Uint8)(0xFF * std::sin(v * 2 * M_PI / 44100));
        v += 300;
    }*/
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
        return m_Channel3FreuqencyLo;
    case Channel3FrequnecyHigher:
        return m_Channel3FreuqencyHi;
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
        m_Channel3FreuqencyLo = val;
        return true;
    case Channel3FrequnecyHigher:
        m_Channel3FreuqencyHi = val;
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
    // Audio is currently disabled
    /*
    SDL_AudioSpec want, have;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = 48000;
    want.format = AUDIO_U8;
    want.channels = 2;
    want.samples = 1024 * 2;
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
    */
}
