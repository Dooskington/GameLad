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

// The 512 Hz frame sequencer period, expressed in T-cycles
// (4194304 Hz / 512 Hz).
#define FRAME_SEQUENCER_PERIOD 8192

namespace
{
    // Duty position advances 0->7 on every 8th step of the frequency timer
    // (see SquareChannel::StepFrequencyTimer()). These bit patterns are
    // read LSB-first (bit N == position N) and must match real hardware's
    // phase, not just its duty percentage/period, since obscure-behavior
    // ROMs (e.g. Gambatte's ch1_duty*_pattern_pos* suite) sample the
    // waveform at a specific absolute step. Cross-checked against
    // gambatte-core's duty_unit.cpp reference table
    // (0x7EE18180, i.e. { 0x80, 0x81, 0xE1, 0x7E } per duty index) rather
    // than derived independently, since duty 1 and 3 are palindromes that
    // hide a reversed-bit-order bug for those two but not duty 0/2.
    const byte DutyTable[4] = { 0x80, 0x81, 0xE1, 0x7E };
    const byte WaveShiftTable[4] = { 4, 0, 1, 2 };
    const int NoiseDivisorTable[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };
}

// ---------------------------------------------------------------------------
// SquareChannel (Channel 1 / Channel 2)
// ---------------------------------------------------------------------------
SquareChannel::SquareChannel(bool hasSweep) :
    m_HasSweep(hasSweep)
{
    Reset();
}

void SquareChannel::Reset()
{
    m_SweepPeriod = 0x00;
    m_SweepNegate = false;
    m_SweepShift = 0x00;

    m_Duty = 0x00;
    m_LengthLoad = 0x00;

    m_EnvelopeInitialVolume = 0x00;
    m_EnvelopeIncrease = false;
    m_EnvelopePeriod = 0x00;

    m_Frequency = 0x0000;
    m_LengthEnabled = false;

    m_Enabled = false;
    m_FrameSequencerStep = 0x00;
    m_FrequencyTimer = 0;
    m_DutyPosition = 0x00;
    m_DutyStepped = false;
    m_LengthCounter = 0;
    m_EnvelopeTimer = 0x00;
    m_CurrentVolume = 0x00;

    m_ShadowFrequency = 0x0000;
    m_SweepTimer = 0;
    m_SweepEnabled = false;
    m_SweepNegateUsedSinceTrigger = false;
}

void SquareChannel::PowerOff(bool clearLength)
{
    if (clearLength)
    {
        m_LengthCounter = 0;
    }

    // On DMG, the length counter survives an APU power-off and remains writable
    // while powered down (see blargg dmg_sound "08-len ctr during power").
    m_SweepPeriod = 0x00;
    m_SweepNegate = false;
    m_SweepShift = 0x00;

    m_Duty = 0x00;

    m_EnvelopeInitialVolume = 0x00;
    m_EnvelopeIncrease = false;
    m_EnvelopePeriod = 0x00;

    m_Frequency = 0x0000;
    m_LengthEnabled = false;

    m_Enabled = false;
    m_DutyPosition = 0x00;
    m_DutyStepped = false;
    m_EnvelopeTimer = 0x00;
    m_CurrentVolume = 0x00;

    m_SweepEnabled = false;
    m_SweepNegateUsedSinceTrigger = false;
}

void SquareChannel::WriteSweep(byte value)
{
    if (!m_HasSweep)
    {
        return;
    }

    bool newNegate = ISBITSET(value, 3);

    // Obscure behavior: clearing the negate bit after a sweep calculation
    // has used it (since the last trigger) disables the channel outright.
    if (m_SweepNegateUsedSinceTrigger && m_SweepNegate && !newNegate)
    {
        m_Enabled = false;
    }

    m_SweepPeriod = (value >> 4) & 0x07;
    m_SweepNegate = newNegate;
    m_SweepShift = value & 0x07;
}

void SquareChannel::WriteLengthDuty(byte value)
{
    m_Duty = (value >> 6) & 0x03;
    m_LengthLoad = value & 0x3F;
    m_LengthCounter = 64 - m_LengthLoad;
}

void SquareChannel::WriteLengthOnly(byte value)
{
    // The duty bits are not latched from this write; only the length
    // counter's reload value is affected. This matters while the APU is
    // powered off: on DMG the length counter remains writable in that
    // state, but the register's other bits (and its read-back value, which
    // is fully masked/cleared while off) must not be affected by it.
    m_LengthLoad = value & 0x3F;
    m_LengthCounter = 64 - m_LengthLoad;
}

void SquareChannel::WriteEnvelope(byte value)
{
    m_EnvelopeInitialVolume = (value >> 4) & 0x0F;
    m_EnvelopeIncrease = ISBITSET(value, 3);
    m_EnvelopePeriod = value & 0x07;

    if (!DacEnabled())
    {
        m_Enabled = false;
    }
}

void SquareChannel::WriteFrequencyLo(byte value)
{
    m_Frequency = (m_Frequency & 0x0700) | value;
}

void SquareChannel::WriteFrequencyHi(byte value)
{
    m_Frequency = (m_Frequency & 0x00FF) | ((unsigned short)(value & 0x07) << 8);

    bool newLengthEnabled = ISBITSET(value, 6);
    bool trigger = ISBITSET(value, 7);

    // Obscure "extra length clocking" behavior: enabling the length counter
    // while it was previously disabled, at a moment where the frame
    // sequencer's next step will not itself clock length, causes an
    // immediate extra clock.
    bool nextStepWontClockLength = (m_FrameSequencerStep % 2) == 0;
    if (!m_LengthEnabled && newLengthEnabled && nextStepWontClockLength && m_LengthCounter > 0)
    {
        m_LengthCounter--;
        if (m_LengthCounter == 0 && !trigger)
        {
            m_Enabled = false;
        }
    }

    m_LengthEnabled = newLengthEnabled;

    if (trigger)
    {
        Trigger();
    }
}

byte SquareChannel::ReadSweep() const
{
    return (byte)((m_SweepPeriod << 4) | (m_SweepNegate ? 0x08 : 0x00) | m_SweepShift);
}

byte SquareChannel::ReadLengthDuty() const
{
    return (byte)(m_Duty << 6);
}

byte SquareChannel::ReadEnvelope() const
{
    return (byte)((m_EnvelopeInitialVolume << 4) | (m_EnvelopeIncrease ? 0x08 : 0x00) | m_EnvelopePeriod);
}

byte SquareChannel::ReadFrequencyHi() const
{
    return (byte)(m_LengthEnabled ? 0x40 : 0x00);
}

void SquareChannel::Trigger()
{
    m_Enabled = true;

    if (m_LengthCounter == 0)
    {
        m_LengthCounter = 64;
        if (m_LengthEnabled && (m_FrameSequencerStep % 2) == 0)
        {
            m_LengthCounter--;
        }
    }

    m_FrequencyTimer = (2048 - m_Frequency) * 4;
    m_EnvelopeTimer = m_EnvelopePeriod;
    if (m_FrameSequencerStep == 6)
    {
        // Obscure behavior: if a trigger coincides with the DIV-APU step
        // immediately before the one that clocks the volume envelope, the
        // envelope's timer is reloaded with one greater than it would have
        // been (i.e. the upcoming envelope clock is effectively "eaten").
        m_EnvelopeTimer++;
    }
    m_CurrentVolume = m_EnvelopeInitialVolume;

    if (m_HasSweep)
    {
        m_ShadowFrequency = m_Frequency;
        m_SweepTimer = (m_SweepPeriod != 0) ? m_SweepPeriod : 8;
        m_SweepEnabled = (m_SweepPeriod != 0) || (m_SweepShift != 0);
        m_SweepNegateUsedSinceTrigger = false;

        if (m_SweepShift != 0)
        {
            if (m_SweepNegate)
            {
                m_SweepNegateUsedSinceTrigger = true;
            }

            bool overflow = false;
            CalculateSweepFrequency(overflow);
            if (overflow)
            {
                m_Enabled = false;
            }
        }
    }

    if (!DacEnabled())
    {
        m_Enabled = false;
    }
}

unsigned short SquareChannel::CalculateSweepFrequency(bool& overflow) const
{
    unsigned short delta = m_ShadowFrequency >> m_SweepShift;
    unsigned short newFrequency = m_SweepNegate
        ? (unsigned short)(m_ShadowFrequency - delta)
        : (unsigned short)(m_ShadowFrequency + delta);

    overflow = newFrequency > 2047;
    return newFrequency;
}

void SquareChannel::StepFrequencyTimer(unsigned long cycles)
{
    if (!m_Enabled)
    {
        return;
    }

    m_FrequencyTimer -= (int)cycles;
    while (m_FrequencyTimer <= 0)
    {
        int period = (2048 - m_Frequency) * 4;
        if (period <= 0)
        {
            period = 4;
        }

        m_FrequencyTimer += period;
        m_DutyPosition = (m_DutyPosition + 1) & 0x07;
        m_DutyStepped = true;
    }
}

void SquareChannel::ClockLength()
{
    if (m_LengthEnabled && m_LengthCounter > 0)
    {
        m_LengthCounter--;
        if (m_LengthCounter == 0)
        {
            m_Enabled = false;
        }
    }
}

void SquareChannel::ClockEnvelope()
{
    if (m_EnvelopePeriod == 0)
    {
        return;
    }

    if (m_EnvelopeTimer > 0)
    {
        m_EnvelopeTimer--;
    }

    if (m_EnvelopeTimer == 0)
    {
        m_EnvelopeTimer = m_EnvelopePeriod;

        if (m_EnvelopeIncrease && m_CurrentVolume < 15)
        {
            m_CurrentVolume++;
        }
        else if (!m_EnvelopeIncrease && m_CurrentVolume > 0)
        {
            m_CurrentVolume--;
        }
    }
}

void SquareChannel::ClockSweep()
{
    if (!m_HasSweep)
    {
        return;
    }

    if (m_SweepTimer > 0)
    {
        m_SweepTimer--;
    }

    if (m_SweepTimer != 0)
    {
        return;
    }

    m_SweepTimer = (m_SweepPeriod != 0) ? m_SweepPeriod : 8;

    if (!m_SweepEnabled || m_SweepPeriod == 0)
    {
        return;
    }

    if (m_SweepNegate)
    {
        m_SweepNegateUsedSinceTrigger = true;
    }

    bool overflow = false;
    unsigned short newFrequency = CalculateSweepFrequency(overflow);

    if (overflow)
    {
        m_Enabled = false;
        return;
    }

    if (m_SweepShift != 0)
    {
        m_ShadowFrequency = newFrequency;
        m_Frequency = newFrequency & 0x07FF;

        // Hardware performs a second overflow check with the newly written
        // shadow frequency, without writing the result back.
        bool overflow2 = false;
        CalculateSweepFrequency(overflow2);
        if (overflow2)
        {
            m_Enabled = false;
        }
    }
}

byte SquareChannel::Amplitude() const
{
    if (!m_Enabled || !DacEnabled())
    {
        return 0;
    }

    if (!m_DutyStepped)
    {
        // Obscure behavior: the duty step counter doesn't begin advancing
        // (or output anything but a flat digital 0) until it has been
        // clocked at least once since the APU was last powered on.
        return 0;
    }

    byte bit = (DutyTable[m_Duty] >> m_DutyPosition) & 0x01;
    return bit ? m_CurrentVolume : 0;
}

bool SquareChannel::DacEnabled() const
{
    return (m_EnvelopeInitialVolume != 0) || m_EnvelopeIncrease;
}

// ---------------------------------------------------------------------------
// WaveChannel (Channel 3)
// ---------------------------------------------------------------------------
WaveChannel::WaveChannel()
{
    m_CGBWaveBehavior = false;
    Reset();
}

void WaveChannel::Reset()
{
    m_DacEnabled = false;
    m_LengthLoad = 0x00;
    m_VolumeCode = 0x00;
    m_Frequency = 0x0000;
    m_LengthEnabled = false;

    m_Enabled = false;
    m_FrameSequencerStep = 0x00;
    m_FrequencyTimer = 0;
    m_LengthCounter = 0;
    m_WavePosition = 0x00;
    m_LastReadByte = 0x00;
    m_LastReadIndex = 0x00;
    m_CycleCounter = 0;
    m_LastReadTime = (unsigned long)-1; // never unlocked until a real fetch happens

    memset(m_WaveRAM, 0x00, sizeof(m_WaveRAM));
}

void WaveChannel::PowerOff(bool clearLength)
{
    if (clearLength)
    {
        m_LengthCounter = 0;
    }

    // Wave RAM and the length counter survive an APU power-off on DMG.
    m_DacEnabled = false;
    m_VolumeCode = 0x00;
    m_Frequency = 0x0000;
    m_LengthEnabled = false;
    m_Enabled = false;
}

void WaveChannel::OnPowerOn()
{
    // Real hardware's internal sample buffer is cleared only when the APU
    // itself powers on, so CH3 emits a "digital 0" the first time it plays
    // after power-up. Re-triggering the channel deliberately does NOT clear
    // this buffer (see Trigger()/Amplitude()).
    m_LastReadByte = 0x00;
    m_LastReadIndex = 0x00;
    m_LastReadTime = (unsigned long)-1; // wave RAM locked until the next real fetch
}

void WaveChannel::WriteDacEnable(byte value)
{
    m_DacEnabled = ISBITSET(value, 7);
    if (!m_DacEnabled)
    {
        m_Enabled = false;
    }
}

void WaveChannel::WriteLength(byte value)
{
    m_LengthLoad = value;
    m_LengthCounter = 256 - m_LengthLoad;
}

void WaveChannel::WriteVolume(byte value)
{
    m_VolumeCode = (value >> 5) & 0x03;
}

void WaveChannel::WriteFrequencyLo(byte value)
{
    m_Frequency = (m_Frequency & 0x0700) | value;
}

void WaveChannel::WriteFrequencyHi(byte value)
{
    m_Frequency = (m_Frequency & 0x00FF) | ((unsigned short)(value & 0x07) << 8);

    bool newLengthEnabled = ISBITSET(value, 6);
    bool trigger = ISBITSET(value, 7);

    bool nextStepWontClockLength = (m_FrameSequencerStep % 2) == 0;
    if (!m_LengthEnabled && newLengthEnabled && nextStepWontClockLength && m_LengthCounter > 0)
    {
        m_LengthCounter--;
        if (m_LengthCounter == 0 && !trigger)
        {
            m_Enabled = false;
        }
    }

    m_LengthEnabled = newLengthEnabled;

    if (trigger)
    {
        Trigger();
    }
}

byte WaveChannel::ReadDacEnable() const
{
    return (byte)(m_DacEnabled ? 0x80 : 0x00);
}

byte WaveChannel::ReadVolume() const
{
    return (byte)(m_VolumeCode << 5);
}

byte WaveChannel::ReadFrequencyHi() const
{
    return (byte)(m_LengthEnabled ? 0x40 : 0x00);
}

bool WaveChannel::IsWaveRamAccessible() const
{
    if (m_LastReadTime == (unsigned long)-1)
    {
        return false;
    }

    return (m_CycleCounter - m_LastReadTime) < 2;
}

byte WaveChannel::ReadWaveRAM(const ushort& address) const
{
    byte index = (byte)(address - 0xFF30);

    if (m_Enabled)
    {
        if (m_CGBWaveBehavior)
        {
            return m_WaveRAM[m_WavePosition >> 1];
        }

        if (IsWaveRamAccessible())
        {
            return m_WaveRAM[m_WavePosition >> 1];
        }

        return 0xFF;
    }

    return m_WaveRAM[index];
}

bool WaveChannel::WriteWaveRAM(const ushort& address, byte value)
{
    byte index = (byte)(address - 0xFF30);

    if (m_Enabled)
    {
        // CGB always aliases an active access to the current fetch byte. DMG
        // retains its two-T-cycle write aperture.
        if (m_CGBWaveBehavior || IsWaveRamAccessible())
        {
            m_WaveRAM[m_WavePosition >> 1] = value;
        }
    }
    else
    {
        m_WaveRAM[index] = value;
    }

    return true;
}

void WaveChannel::Trigger()
{
    // A DMG retrigger that overlaps the upcoming two-T wave-RAM fetch aperture
    // corrupts the first four bytes before resetting the channel position.
    if (!m_CGBWaveBehavior && m_Enabled && m_DacEnabled && m_FrequencyTimer <= 2)
    {
        byte pos = (byte)(((m_WavePosition + 1) & 0x1F) >> 1);
        if (pos < 4)
        {
            m_WaveRAM[0] = m_WaveRAM[pos];
        }
        else
        {
            byte block = (byte)(pos & ~0x03);
            memcpy(m_WaveRAM, m_WaveRAM + block, 4);
        }
    }

    m_Enabled = true;

    if (m_LengthCounter == 0)
    {
        m_LengthCounter = 256;
        if (m_LengthEnabled && (m_FrameSequencerStep % 2) == 0)
        {
            m_LengthCounter--;
        }
    }

    // CH3's restart signal traverses the frequency-counter and wave-RAM
    // pipeline before the first fetch. Subsequent fetches use the normal
    // period, but the first one occurs six T-cycles later.
    m_FrequencyTimer = ((2048 - m_Frequency) * 2) + 6;

    // Obscure behavior: retriggering does NOT load wave RAM byte 0 into the
    // sample buffer the way it "should". The position resets to 0, but the
    // buffer (m_LastReadByte/m_LastReadIndex) is left untouched, so the
    // first sample played after a trigger is whatever was last buffered
    // before it; wave RAM byte 0's high nibble is only heard once the
    // waveform loops back around to position 0 naturally. m_LastReadTime is
    // deliberately left untouched too - it stays "in the past" relative to
    // m_CycleCounter until the next real fetch, correctly reproducing the
    // "wave RAM is locked immediately after a retrigger" behavior with no
    // extra bookkeeping.
    m_WavePosition = 0x00;

    if (!m_DacEnabled)
    {
        m_Enabled = false;
    }
}

void WaveChannel::StepFrequencyTimer(unsigned long cycles)
{
    m_CycleCounter += cycles;

    if (!m_Enabled)
    {
        return;
    }

    m_FrequencyTimer -= (int)cycles;

    bool fetched = false;
    int period = 0;
    while (m_FrequencyTimer <= 0)
    {
        period = (2048 - m_Frequency) * 2;
        if (period <= 0)
        {
            period = 2;
        }

        m_FrequencyTimer += period;
        m_WavePosition = (m_WavePosition + 1) & 0x1F;
        fetched = true;
    }

    if (fetched)
    {
        m_LastReadIndex = m_WavePosition >> 1;
        m_LastReadByte = m_WaveRAM[m_LastReadIndex];

        // The most recent fetch is "now" (this exact T-cycle) only if it
        // landed exactly on the last cycle consumed by this call, i.e. the
        // countdown was reloaded to a full, unconsumed period with nothing
        // left over. Otherwise the fetch happened earlier within this batch
        // and is already in the past by the time any register access
        // following this call could observe it. (A cruder "whole batch is
        // unlocked" approximation was tried and measured worse: it lost a
        // Gambatte gambatte_hex case with no compensating gain elsewhere.)
        if (m_FrequencyTimer == period)
        {
            m_LastReadTime = m_CycleCounter;
        }
    }
}

void WaveChannel::ClockLength()
{
    if (m_LengthEnabled && m_LengthCounter > 0)
    {
        m_LengthCounter--;
        if (m_LengthCounter == 0)
        {
            m_Enabled = false;
        }
    }
}

byte WaveChannel::Amplitude() const
{
    if (!m_Enabled || !m_DacEnabled)
    {
        return 0;
    }

    // Output comes from the internal sample buffer, not a live wave RAM
    // read - see Trigger()'s comment for why these can briefly disagree
    // right after a retrigger.
    byte sample = (m_WavePosition & 0x01) ? (m_LastReadByte & 0x0F) : (byte)(m_LastReadByte >> 4);

    return (byte)(sample >> WaveShiftTable[m_VolumeCode]);
}

// ---------------------------------------------------------------------------
// NoiseChannel (Channel 4)
// ---------------------------------------------------------------------------
NoiseChannel::NoiseChannel()
{
    Reset();
}

void NoiseChannel::Reset()
{
    m_LengthLoad = 0x00;

    m_EnvelopeInitialVolume = 0x00;
    m_EnvelopeIncrease = false;
    m_EnvelopePeriod = 0x00;

    m_ClockShift = 0x00;
    m_WidthMode7Bit = false;
    m_DivisorCode = 0x00;

    m_LengthEnabled = false;

    m_Enabled = false;
    m_FrameSequencerStep = 0x00;
    m_FrequencyTimer = NoiseDivisorTable[0];
    m_LengthCounter = 0;
    m_EnvelopeTimer = 0x00;
    m_CurrentVolume = 0x00;
    m_LFSR = 0x7FFF;
}

void NoiseChannel::PowerOff(bool clearLength)
{
    if (clearLength)
    {
        m_LengthCounter = 0;
    }

    m_EnvelopeInitialVolume = 0x00;
    m_EnvelopeIncrease = false;
    m_EnvelopePeriod = 0x00;

    m_ClockShift = 0x00;
    m_WidthMode7Bit = false;
    m_DivisorCode = 0x00;

    m_LengthEnabled = false;
    m_Enabled = false;
    m_EnvelopeTimer = 0x00;
    m_CurrentVolume = 0x00;
}

void NoiseChannel::WriteLength(byte value)
{
    m_LengthLoad = value & 0x3F;
    m_LengthCounter = 64 - m_LengthLoad;
}

void NoiseChannel::WriteEnvelope(byte value)
{
    m_EnvelopeInitialVolume = (value >> 4) & 0x0F;
    m_EnvelopeIncrease = ISBITSET(value, 3);
    m_EnvelopePeriod = value & 0x07;

    if (!DacEnabled())
    {
        m_Enabled = false;
    }
}

void NoiseChannel::WritePolynomial(byte value)
{
    m_ClockShift = (value >> 4) & 0x0F;
    m_WidthMode7Bit = ISBITSET(value, 3);
    m_DivisorCode = value & 0x07;
}

void NoiseChannel::WriteCounter(byte value)
{
    bool newLengthEnabled = ISBITSET(value, 6);
    bool trigger = ISBITSET(value, 7);

    bool nextStepWontClockLength = (m_FrameSequencerStep % 2) == 0;
    if (!m_LengthEnabled && newLengthEnabled && nextStepWontClockLength && m_LengthCounter > 0)
    {
        m_LengthCounter--;
        if (m_LengthCounter == 0 && !trigger)
        {
            m_Enabled = false;
        }
    }

    m_LengthEnabled = newLengthEnabled;

    if (trigger)
    {
        Trigger();
    }
}

byte NoiseChannel::ReadEnvelope() const
{
    return (byte)((m_EnvelopeInitialVolume << 4) | (m_EnvelopeIncrease ? 0x08 : 0x00) | m_EnvelopePeriod);
}

byte NoiseChannel::ReadPolynomial() const
{
    return (byte)((m_ClockShift << 4) | (m_WidthMode7Bit ? 0x08 : 0x00) | m_DivisorCode);
}

byte NoiseChannel::ReadCounter() const
{
    return (byte)(m_LengthEnabled ? 0x40 : 0x00);
}

void NoiseChannel::Trigger()
{
    m_Enabled = true;

    if (m_LengthCounter == 0)
    {
        m_LengthCounter = 64;
        if (m_LengthEnabled && (m_FrameSequencerStep % 2) == 0)
        {
            m_LengthCounter--;
        }
    }

    m_FrequencyTimer = NoiseDivisorTable[m_DivisorCode] << m_ClockShift;
    m_EnvelopeTimer = m_EnvelopePeriod;
    if (m_FrameSequencerStep == 6)
    {
        // See SquareChannel::Trigger() for an explanation of this quirk.
        m_EnvelopeTimer++;
    }
    m_CurrentVolume = m_EnvelopeInitialVolume;
    m_LFSR = 0x7FFF;

    if (!DacEnabled())
    {
        m_Enabled = false;
    }
}

void NoiseChannel::ClockLength()
{
    if (m_LengthEnabled && m_LengthCounter > 0)
    {
        m_LengthCounter--;
        if (m_LengthCounter == 0)
        {
            m_Enabled = false;
        }
    }
}

void NoiseChannel::ClockEnvelope()
{
    if (m_EnvelopePeriod == 0)
    {
        return;
    }

    if (m_EnvelopeTimer > 0)
    {
        m_EnvelopeTimer--;
    }

    if (m_EnvelopeTimer == 0)
    {
        m_EnvelopeTimer = m_EnvelopePeriod;

        if (m_EnvelopeIncrease && m_CurrentVolume < 15)
        {
            m_CurrentVolume++;
        }
        else if (!m_EnvelopeIncrease && m_CurrentVolume > 0)
        {
            m_CurrentVolume--;
        }
    }
}

void NoiseChannel::StepFrequencyTimer(unsigned long cycles)
{
    if (!m_Enabled)
    {
        return;
    }

    if (m_ClockShift >= 14)
    {
        // Obscure behavior: a clock shift of 14 or 15 results in the LFSR
        // receiving no clocks at all (the real hardware's internal divider
        // can't represent a shift that large), so the channel just holds
        // its last output.
        return;
    }

    m_FrequencyTimer -= (int)cycles;
    while (m_FrequencyTimer <= 0)
    {
        int period = NoiseDivisorTable[m_DivisorCode] << m_ClockShift;
        if (period <= 0)
        {
            period = 8;
        }

        m_FrequencyTimer += period;

        unsigned short xorResult = (m_LFSR & 0x01) ^ ((m_LFSR >> 1) & 0x01);
        m_LFSR = (m_LFSR >> 1) | (xorResult << 14);

        if (m_WidthMode7Bit)
        {
            m_LFSR = (unsigned short)((m_LFSR & ~0x0040) | (xorResult << 6));
        }
    }
}

byte NoiseChannel::Amplitude() const
{
    if (!m_Enabled || !DacEnabled())
    {
        return 0;
    }

    return (~m_LFSR & 0x01) ? m_CurrentVolume : (byte)0;
}

bool NoiseChannel::DacEnabled() const
{
    return (m_EnvelopeInitialVolume != 0) || m_EnvelopeIncrease;
}

// ---------------------------------------------------------------------------
// APU
// ---------------------------------------------------------------------------
APU::APU() :
    m_Channel1(true),
    m_Channel2(false),
    m_NR50(0x00),
    m_NR51(0x00),
    m_Powered(false),
    m_mode(GameBoyMode::DMG),
    m_FrameSequencerCounter(FRAME_SEQUENCER_PERIOD),
    m_FrameSequencerStep(0x07),
    m_FrameSequencerSelfClocked(true),
    m_SampleRate(44100),
    m_CyclesPerSample(4194304.0 / 44100.0),
    m_SampleCycleAccumulator(4194304.0 / 44100.0),
    m_SampleReadIndex(0)
{
    m_SampleBuffer.reserve(4096);
}

APU::~APU()
{
}

void APU::SetSampleRate(unsigned int sampleRate)
{
    m_SampleRate = (sampleRate == 0) ? 44100 : sampleRate;
    m_CyclesPerSample = 4194304.0 / (double)m_SampleRate;
}

void APU::Step(unsigned long cycles)
{
    // Each channel's frequency divider free-runs in the T-cycle domain
    // regardless of the frame sequencer; StepFrequencyTimer() itself is a
    // no-op while a channel is disabled.
    m_Channel1.StepFrequencyTimer(cycles);
    m_Channel2.StepFrequencyTimer(cycles);
    m_Channel3.StepFrequencyTimer(cycles);
    m_Channel4.StepFrequencyTimer(cycles);

    if (m_Powered && m_FrameSequencerSelfClocked)
    {
        m_FrameSequencerCounter -= (long)cycles;
        while (m_FrameSequencerCounter <= 0)
        {
            m_FrameSequencerCounter += FRAME_SEQUENCER_PERIOD;
            ClockFrameSequencer();
        }
    }

    m_SampleCycleAccumulator -= (double)cycles;
    while (m_SampleCycleAccumulator <= 0.0)
    {
        m_SampleCycleAccumulator += m_CyclesPerSample;
        GenerateSample();
    }
}

void APU::ClockFrameSequencerEdge()
{
    // Real Timer-driven edges only matter while the APU is actually
    // powered - matches the same m_Powered gate the internal free-running
    // counter uses, and mirrors real hardware where a powered-off APU's
    // frame sequencer is held reset regardless of DIV activity.
    if (m_Powered)
    {
        ClockFrameSequencer();
    }
}

void APU::ClockFrameSequencer()
{
    m_FrameSequencerStep = (m_FrameSequencerStep + 1) & 0x07;

    m_Channel1.SetFrameSequencerStep(m_FrameSequencerStep);
    m_Channel2.SetFrameSequencerStep(m_FrameSequencerStep);
    m_Channel3.SetFrameSequencerStep(m_FrameSequencerStep);
    m_Channel4.SetFrameSequencerStep(m_FrameSequencerStep);

    // Step:  0    1    2    3    4    5    6    7
    // Length: X         X         X         X
    // Sweep:            X                   X
    // Envelope:                                   X
    bool clockLength = (m_FrameSequencerStep % 2) == 0;
    bool clockSweep = (m_FrameSequencerStep == 2) || (m_FrameSequencerStep == 6);
    bool clockEnvelope = (m_FrameSequencerStep == 7);

    if (clockLength)
    {
        m_Channel1.ClockLength();
        m_Channel2.ClockLength();
        m_Channel3.ClockLength();
        m_Channel4.ClockLength();
    }

    if (clockSweep)
    {
        m_Channel1.ClockSweep();
    }

    if (clockEnvelope)
    {
        m_Channel1.ClockEnvelope();
        m_Channel2.ClockEnvelope();
        m_Channel4.ClockEnvelope();
    }
}

void APU::GenerateSample()
{
    if (m_SampleBuffer.size() >= MaxBufferedSamples)
    {
        // Drop samples rather than growing unbounded if nobody is consuming
        // them (e.g. a test harness that never calls ConsumeSamples()).
        return;
    }

    Sample sample = { 0.0f, 0.0f };

    if (m_Powered)
    {
        byte ch1 = m_Channel1.Amplitude();
        byte ch2 = m_Channel2.Amplitude();
        byte ch3 = m_Channel3.Amplitude();
        byte ch4 = m_Channel4.Amplitude();

        // Digital-to-analog conversion. This is a linear approximation of
        // the documented DMG DAC transfer function (0 -> +1, 15 -> -1); it
        // is not calibrated to any specific chip revision's analog output,
        // which is an acknowledged fidelity gap (see audit caveats).
        float analog1 = m_Channel1.DacEnabled() ? (1.0f - ((float)ch1 / 7.5f)) : 0.0f;
        float analog2 = m_Channel2.DacEnabled() ? (1.0f - ((float)ch2 / 7.5f)) : 0.0f;
        float analog3 = m_Channel3.DacEnabled() ? (1.0f - ((float)ch3 / 7.5f)) : 0.0f;
        float analog4 = m_Channel4.DacEnabled() ? (1.0f - ((float)ch4 / 7.5f)) : 0.0f;

        float left = 0.0f;
        float right = 0.0f;

        // NR51 routes each channel independently to the left ("SO2", bits
        // 4-7) and right ("SO1", bits 0-3) mixer inputs.
        if (ISBITSET(m_NR51, 4)) left += analog1;
        if (ISBITSET(m_NR51, 5)) left += analog2;
        if (ISBITSET(m_NR51, 6)) left += analog3;
        if (ISBITSET(m_NR51, 7)) left += analog4;

        if (ISBITSET(m_NR51, 0)) right += analog1;
        if (ISBITSET(m_NR51, 1)) right += analog2;
        if (ISBITSET(m_NR51, 2)) right += analog3;
        if (ISBITSET(m_NR51, 3)) right += analog4;

        byte leftVolume = (byte)(((m_NR50 >> 4) & 0x07) + 1);
        byte rightVolume = (byte)((m_NR50 & 0x07) + 1);

        sample.Left = (left / 4.0f) * ((float)leftVolume / 8.0f);
        sample.Right = (right / 4.0f) * ((float)rightVolume / 8.0f);
    }

    m_SampleBuffer.push_back(sample);
}

size_t APU::GetPendingSampleCount() const
{
    return m_SampleBuffer.size() - m_SampleReadIndex;
}

size_t APU::ConsumeSamples(Sample* pBuffer, size_t maxCount)
{
    size_t available = GetPendingSampleCount();
    size_t count = (maxCount < available) ? maxCount : available;

    if (pBuffer != nullptr)
    {
        for (size_t i = 0; i < count; i++)
        {
            pBuffer[i] = m_SampleBuffer[m_SampleReadIndex + i];
        }
    }

    m_SampleReadIndex += count;

    if (m_SampleReadIndex == m_SampleBuffer.size())
    {
        m_SampleBuffer.clear();
        m_SampleReadIndex = 0;
    }

    return count;
}

void APU::ClearSampleBuffer()
{
    m_SampleBuffer.clear();
    m_SampleReadIndex = 0;
}

void APU::PowerOn()
{
    m_Powered = true;

    // The frame sequencer resets so that the next clock produced is step 0,
    // which includes a length clock.
    m_FrameSequencerStep = 0x07;
    m_FrameSequencerCounter = FRAME_SEQUENCER_PERIOD;

    m_Channel1.SetFrameSequencerStep(m_FrameSequencerStep);
    m_Channel2.SetFrameSequencerStep(m_FrameSequencerStep);
    m_Channel3.SetFrameSequencerStep(m_FrameSequencerStep);
    m_Channel4.SetFrameSequencerStep(m_FrameSequencerStep);

    m_Channel3.OnPowerOn();
}

void APU::PowerOff()
{
    m_Powered = false;

    const bool clearLength = IsCGBHardware(m_mode);
    m_Channel1.PowerOff(clearLength);
    m_Channel2.PowerOff(clearLength);
    m_Channel3.PowerOff(clearLength);
    m_Channel4.PowerOff(clearLength);

    m_NR50 = 0x00;
    m_NR51 = 0x00;
}

bool APU::IsChannelEnabled(int channel) const
{
    switch (channel)
    {
    case 0:
        return m_Channel1.IsEnabled();
    case 1:
        return m_Channel2.IsEnabled();
    case 2:
        return m_Channel3.IsEnabled();
    case 3:
        return m_Channel4.IsEnabled();
    default:
        return false;
    }
}

byte APU::GetChannelAmplitude(int channel) const
{
    switch (channel)
    {
    case 0:
        return m_Channel1.Amplitude();
    case 1:
        return m_Channel2.Amplitude();
    case 2:
        return m_Channel3.Amplitude();
    case 3:
        return m_Channel4.Amplitude();
    default:
        return 0;
    }
}

// IMemoryUnit
byte APU::ReadByte(const ushort& address)
{
    if ((address >= 0xFF30) && (address <= 0xFF3F))
    {
        return m_Channel3.ReadWaveRAM(address);
    }

    /*
        PCM12 / PCM34 - CGB-only read-only taps on the raw 4-bit channel
        outputs, before panning and master volume. Low nibble is the lower
        numbered channel of each pair.
    */
    if ((address == 0xFF76) || (address == 0xFF77))
    {
        if (!IsCGBFeatureMode(m_mode))
        {
            return 0xFF;
        }

        const int lowChannel = (address == 0xFF76) ? 0 : 2;
        const byte low = static_cast<byte>(GetChannelAmplitude(lowChannel) & 0x0F);
        const byte high = static_cast<byte>(GetChannelAmplitude(lowChannel + 1) & 0x0F);
        return static_cast<byte>(low | (high << 4));
    }

    switch (address)
    {
    case Channel1Sweep:
        return m_Channel1.ReadSweep() | 0x80;
    case Channel1LengthWavePatternDuty:
        return m_Channel1.ReadLengthDuty() | 0x3F;
    case Channel1VolumeEnvelope:
        return m_Channel1.ReadEnvelope();
    case Channel1FrequencyLo:
        // Write only
        return 0xFF;
    case Channel1FrequencyHi:
        return m_Channel1.ReadFrequencyHi() | 0xBF;
    case Channel2LengthWavePatternDuty:
        return m_Channel2.ReadLengthDuty() | 0x3F;
    case Channel2VolumeEnvelope:
        return m_Channel2.ReadEnvelope();
    case Channel2FrequnecyLo:
        // Write only
        return 0xFF;
    case Channel2FrequencyHi:
        return m_Channel2.ReadFrequencyHi() | 0xBF;
    case Channel3OnOff:
        return m_Channel3.ReadDacEnable() | 0x7F;
    case Channel3Length:
        // Write only
        return 0xFF;
    case Channel3OutputLevel:
        return m_Channel3.ReadVolume() | 0x9F;
    case Channel3FrequencyLower:
        // Write only
        return 0xFF;
    case Channel3FrequnecyHigher:
        return m_Channel3.ReadFrequencyHi() | 0xBF;
    case Channel4Length:
        // Write only
        return 0xFF;
    case Channel4VolumeEnvelope:
        return m_Channel4.ReadEnvelope();
    case Channel4PolynomialCounter:
        return m_Channel4.ReadPolynomial();
    case Channel4Counter:
        return m_Channel4.ReadCounter() | 0xBF;
    case ChannelControl:
        return m_NR50;
    case OutputTerminalSelection:
        return m_NR51;
    case SoundOnOff:
        {
            byte value = (byte)(m_Powered ? 0x80 : 0x00);
            value |= 0x70; // Bits 4-6 are unused and always read back as 1.

            if (m_Channel1.IsEnabled()) value |= 0x01;
            if (m_Channel2.IsEnabled()) value |= 0x02;
            if (m_Channel3.IsEnabled()) value |= 0x04;
            if (m_Channel4.IsEnabled()) value |= 0x08;

            return value;
        }
    default:
        // Unmapped registers in the FF10-FF3F range (FF15, FF1F, FF27-FF2F)
        // read back as 0xFF on real hardware.
        return 0xFF;
    }
}

bool APU::WriteByte(const ushort& address, const byte val)
{
    if ((address == 0xFF76) || (address == 0xFF77))
    {
        // PCM12 / PCM34 are read-only.
        return true;
    }

    if ((address >= 0xFF30) && (address <= 0xFF3F))
    {
        return m_Channel3.WriteWaveRAM(address, val);
    }

    if (address == SoundOnOff)
    {
        /*
        Bit 7 - All sound on/off  (0: stop all sound circuits) (Read/Write)
        Bit 3 - Sound 4 ON flag (Read Only)
        Bit 2 - Sound 3 ON flag (Read Only)
        Bit 1 - Sound 2 ON flag (Read Only)
        Bit 0 - Sound 1 ON flag (Read Only)
        */
        bool powerOn = ISBITSET(val, 7);
        if (powerOn && !m_Powered)
        {
            PowerOn();
        }
        else if (!powerOn && m_Powered)
        {
            PowerOff();
        }

        return true;
    }

    // On DMG, the length counters remain writable (and keep counting down)
    // even while the APU is powered off; this must be handled before the
    // general power gate below (see blargg dmg_sound "08-len ctr during
    // power"). On CGB the hardware blocks these writes as well, which is what
    // cgb_sound's equivalent test asserts.
    if (!m_Powered)
    {
        if (IsCGBFeatureMode(m_mode))
        {
            return true;
        }

        switch (address)
        {
        case Channel1LengthWavePatternDuty:
            // Only the length field is writable while off; the duty bits
            // (and hence the register's masked read-back value) must stay
            // cleared until the next full write while powered on.
            m_Channel1.WriteLengthOnly(val);
            return true;
        case Channel2LengthWavePatternDuty:
            m_Channel2.WriteLengthOnly(val);
            return true;
        case Channel3Length:
            m_Channel3.WriteLength(val);
            return true;
        case Channel4Length:
            m_Channel4.WriteLength(val);
            return true;
        default:
            // All other register writes are ignored while powered off.
            return true;
        }
    }

    switch (address)
    {
    case Channel1LengthWavePatternDuty:
        m_Channel1.WriteLengthDuty(val);
        return true;
    case Channel2LengthWavePatternDuty:
        m_Channel2.WriteLengthDuty(val);
        return true;
    case Channel3Length:
        m_Channel3.WriteLength(val);
        return true;
    case Channel4Length:
        m_Channel4.WriteLength(val);
        return true;
    case Channel1Sweep:
        m_Channel1.WriteSweep(val);
        return true;
    case Channel1VolumeEnvelope:
        m_Channel1.WriteEnvelope(val);
        return true;
    case Channel1FrequencyLo:
        m_Channel1.WriteFrequencyLo(val);
        return true;
    case Channel1FrequencyHi:
        m_Channel1.WriteFrequencyHi(val);
        return true;
    case Channel2VolumeEnvelope:
        m_Channel2.WriteEnvelope(val);
        return true;
    case Channel2FrequnecyLo:
        m_Channel2.WriteFrequencyLo(val);
        return true;
    case Channel2FrequencyHi:
        m_Channel2.WriteFrequencyHi(val);
        return true;
    case Channel3OnOff:
        m_Channel3.WriteDacEnable(val);
        return true;
    case Channel3OutputLevel:
        m_Channel3.WriteVolume(val);
        return true;
    case Channel3FrequencyLower:
        m_Channel3.WriteFrequencyLo(val);
        return true;
    case Channel3FrequnecyHigher:
        m_Channel3.WriteFrequencyHi(val);
        return true;
    case Channel4VolumeEnvelope:
        m_Channel4.WriteEnvelope(val);
        return true;
    case Channel4PolynomialCounter:
        m_Channel4.WritePolynomial(val);
        return true;
    case Channel4Counter:
        m_Channel4.WriteCounter(val);
        return true;
    case ChannelControl:
        m_NR50 = val;
        return true;
    case OutputTerminalSelection:
        m_NR51 = val;
        return true;
    default:
        // Unmapped registers in the FF10-FF3F range are simply ignored.
        return true;
    }
}
