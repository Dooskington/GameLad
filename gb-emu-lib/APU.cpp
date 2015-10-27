#include "PCH.hpp"
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

APU::APU() :
    m_SoundOnOffRegister(0x00)
{
    Logger::Log("APU created.");
}

APU::~APU()
{
    Logger::Log("APU destroyed.");
}

// IMemoryUnit
byte APU::ReadByte(const ushort& address)
{
    switch (address)
    {
    case Channel1Sweep:
    case Channel1LengthWavePatternDuty:
    case Channel1VolumeEnvelope:
    case Channel1FrequencyLo:
    case Channel1FrequencyHi:
    case Channel2LengthWavePatternDuty:
    case Channel2VolumeEnvelope:
    case Channel2FrequnecyLo:
    case Channel2FrequencyHi:
    case Channel3OnOff:
    case Channel3Length:
    case Channel3OutputLevel:
    case Channel3FrequencyLower:
    case Channel3FrequnecyHigher:
    case Channel4Length:
    case Channel4VolumeEnvelope:
    case Channel4PolynomialCounter:
    case Channel4Counter:
    case ChannelControl:
    case OutputTerminalSelection:
        // TODO: NYI
        return 0x00;
    case SoundOnOff:
        return m_SoundOnOffRegister;
    default:
        Logger::Log("APU::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool APU::WriteByte(const ushort& address, const byte val)
{
    switch (address)
    {
    case Channel1Sweep:
    case Channel1LengthWavePatternDuty:
    case Channel1VolumeEnvelope:
    case Channel1FrequencyLo:
    case Channel1FrequencyHi:
    case Channel2LengthWavePatternDuty:
    case Channel2VolumeEnvelope:
    case Channel2FrequnecyLo:
    case Channel2FrequencyHi:
    case Channel3OnOff:
    case Channel3Length:
    case Channel3OutputLevel:
    case Channel3FrequencyLower:
    case Channel3FrequnecyHigher:
    case Channel4Length:
    case Channel4VolumeEnvelope:
    case Channel4PolynomialCounter:
    case Channel4Counter:
    case ChannelControl:
    case OutputTerminalSelection:
        // TODO: NYI
        return true;
    case SoundOnOff:
        m_SoundOnOffRegister = val;
        return true;
    default:
        Logger::Log("APU::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
