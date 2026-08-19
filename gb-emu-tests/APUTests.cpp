#include "stdafx.h"

#include <cmath>
#include <APU.hpp>

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

// Channel indices, matching APU::IsChannelEnabled/GetChannelAmplitude.
#define CH1 0
#define CH2 1
#define CH3 2
#define CH4 3

TEST_CLASS(APUTests)
{
public:
    // NR52 (Sound on/off): bits 4-6 always read back as 1, and powering the
    // APU down zeroes NR50/NR51 and disables all channels while leaving them
    // writable again only once powered back on.
    TEST_METHOD(NR52PowerOnOffTest)
    {
        APU apu;

        // Powered off by default (matches real hardware boot state expectation
        // that games explicitly power on the APU before use).
        Assert::AreEqual(0x70, (int)apu.ReadByte(SoundOnOff));

        // Register writes other than length counters are ignored while off.
        apu.WriteByte(ChannelControl, 0x77);
        Assert::AreEqual(0x00, (int)apu.ReadByte(ChannelControl));

        // Power on.
        apu.WriteByte(SoundOnOff, 0x80);
        Assert::AreEqual(0xF0, (int)apu.ReadByte(SoundOnOff));

        apu.WriteByte(ChannelControl, 0x77);
        Assert::AreEqual(0x77, (int)apu.ReadByte(ChannelControl));

        // Power off again - NR50 is cleared.
        apu.WriteByte(SoundOnOff, 0x00);
        Assert::AreEqual(0x00, (int)apu.ReadByte(ChannelControl));
        Assert::AreEqual(0x70, (int)apu.ReadByte(SoundOnOff));
    }

    // Wave RAM is always addressable regardless of APU power state, and its
    // contents survive an APU power-off (DMG-specific quirk).
    TEST_METHOD(WaveRAMSurvivesPowerOffTest)
    {
        APU apu;

        for (int i = 0; i < 0x10; i++)
        {
            apu.WriteByte((ushort)(0xFF30 + i), (byte)(0x11 * i));
        }

        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(SoundOnOff, 0x00);

        for (int i = 0; i < 0x10; i++)
        {
            Assert::AreEqual(0x11 * i, (int)apu.ReadByte((ushort)(0xFF30 + i)));
        }
    }

    // Length counters (NRx1) remain writable even while the APU is powered
    // off (blargg dmg_sound "08-len ctr during power").
    TEST_METHOD(LengthCounterWritableWhilePoweredOffTest)
    {
        APU apu;

        // Channel 3, length load 255 -> counter should become 1. Length
        // registers remain writable even while the APU is powered off.
        apu.WriteByte(Channel3Length, 0xFF);      // NR31 = 255 -> length counter = 1

        apu.WriteByte(SoundOnOff, 0x80);          // power on
        apu.WriteByte(Channel3OnOff, 0x80);       // DAC on
        apu.WriteByte(Channel3FrequnecyHigher, 0xC0); // trigger + length enable, freq = 0

        Assert::IsTrue(apu.IsChannelEnabled(CH3));

        // One length clock (256 Hz -> occurs at the first 8192-cycle frame
        // sequencer tick) should exhaust the length counter and disable the
        // channel.
        apu.Step(8192);
        Assert::IsFalse(apu.IsChannelEnabled(CH3));
    }

    // While the APU is off, writes to NR11/NR21 must only affect the length
    // counter's reload value; the duty bits (and hence the register's
    // masked read-back value, which is fully cleared while off) must not be
    // latched until the next write while powered on (blargg dmg_sound
    // "01-registers" test 6, "When off, should ignore writes to registers").
    TEST_METHOD(DutyBitsIgnoredWhilePoweredOffTest)
    {
        APU apu;

        // Powered off by default. Write duty=3, length=63 - should be fully
        // masked (0x3F) on read back, since duty bits are not writable off.
        apu.WriteByte(Channel1LengthWavePatternDuty, 0xFF);
        Assert::AreEqual(0x3F, (int)apu.ReadByte(Channel1LengthWavePatternDuty));

        // But the length counter reload did take effect: length load 63
        // means only 1 clock is needed to expire it once running.
        apu.WriteByte(SoundOnOff, 0x80);          // power on
        apu.WriteByte(Channel1VolumeEnvelope, 0xF0); // DAC on
        apu.WriteByte(Channel1FrequencyHi, 0xC0);    // trigger + length enable
        Assert::IsTrue(apu.IsChannelEnabled(CH1));

        apu.Step(8192); // one length clock
        Assert::IsFalse(apu.IsChannelEnabled(CH1));
    }

    // A channel cannot be triggered while its DAC is disabled (envelope
    // volume == 0 and envelope direction == decrease).
    TEST_METHOD(DacDisabledPreventsTriggerTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);

        apu.WriteByte(Channel1VolumeEnvelope, 0x00); // volume 0, decrease -> DAC off
        apu.WriteByte(Channel1FrequencyHi, 0x80);    // trigger

        Assert::IsFalse(apu.IsChannelEnabled(CH1));
    }

    // Writing NRx2 such that the DAC becomes disabled immediately silences
    // an already-running channel (blargg dmg_sound "03-trigger").
    TEST_METHOD(NRx2WriteDisablesActiveChannelTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);

        apu.WriteByte(Channel1VolumeEnvelope, 0x80); // volume 8, DAC on
        apu.WriteByte(Channel1FrequencyHi, 0x80);    // trigger
        Assert::IsTrue(apu.IsChannelEnabled(CH1));

        apu.WriteByte(Channel1VolumeEnvelope, 0x00); // DAC off
        Assert::IsFalse(apu.IsChannelEnabled(CH1));
    }

    // The frame sequencer resets so that its first clock (8192 cycles after
    // power-on) is step 0, and it then advances one step every 8192 cycles.
    TEST_METHOD(FrameSequencerCadenceTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);

        Assert::AreEqual(7, (int)apu.GetFrameSequencerStep());

        for (int expectedStep = 0; expectedStep < 16; expectedStep++)
        {
            apu.Step(8192);
            Assert::AreEqual(expectedStep % 8, (int)apu.GetFrameSequencerStep());
        }
    }

    // Channel 1's sweep unit performs an overflow check immediately at
    // trigger time (before any frame-sequencer clock) and disables the
    // channel if the shifted frequency would exceed the 11-bit range.
    TEST_METHOD(SweepOverflowDisablesChannelOnTriggerTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);

        apu.WriteByte(Channel1Sweep, 0x11);          // period 1, add, shift 1
        apu.WriteByte(Channel1VolumeEnvelope, 0x80); // DAC on
        apu.WriteByte(Channel1FrequencyLo, 0xFF);    // frequency = 0x7FF (max 11-bit value)
        apu.WriteByte(Channel1FrequencyHi, 0x87);    // trigger, freq hi bits = 0x07

        // shadow(0x7FF) + (0x7FF >> 1) = 0x7FF + 0x3FF = 0xBFE > 0x7FF -> overflow
        Assert::IsFalse(apu.IsChannelEnabled(CH1));
    }

    // Channel 3 plays back the currently-addressed wave RAM nibble, shifted
    // per NR32's volume code, immediately upon trigger.
    // Obscure behavior: (re)triggering CH3 does not reload its internal
    // sample buffer from wave RAM. The first sample played after a trigger
    // is whatever was in the buffer beforehand (0, since the APU was just
    // powered on) - wave RAM byte 0's high nibble is only heard once the
    // waveform naturally loops back around to position 0.
    TEST_METHOD(WaveChannelAmplitudeTest)
    {
        APU apu;

        apu.WriteByte(0xFF30, 0xA5); // first wave RAM byte, played first on trigger

        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(Channel3OnOff, 0x80);        // DAC on
        apu.WriteByte(Channel3OutputLevel, 0x20);  // volume code 1 -> 100% (no shift)
        apu.WriteByte(Channel3FrequencyLower, 0x00);
        apu.WriteByte(Channel3FrequnecyHigher, 0x80); // trigger, freq = 0

        Assert::IsTrue(apu.IsChannelEnabled(CH3));

        // Sample buffer is still the post-power-on 0 right after the trigger.
        Assert::AreEqual(0x00, (int)apu.GetChannelAmplitude(CH3));

        // The first fetch has a six-T-cycle trigger-pipeline delay beyond the
        // normal 4096-T period.
        apu.Step(4096);
        Assert::AreEqual(0x00, (int)apu.GetChannelAmplitude(CH3));
        apu.Step(6);
        Assert::AreEqual(0x05, (int)apu.GetChannelAmplitude(CH3));
    }

    TEST_METHOD(WaveChannelShortPeriodTest)
    {
        const int periods[] = { 2, 4, 6 };
        for (int period : periods)
        {
            WaveChannel channel;
            channel.OnPowerOn();
            channel.WriteWaveRAM(0xFF30, 0x12);
            channel.WriteWaveRAM(0xFF31, 0x34);
            channel.WriteDacEnable(0x80);
            channel.WriteVolume(0x20);

            const int frequency = 2048 - (period / 2);
            channel.WriteFrequencyLo(static_cast<byte>(frequency));
            channel.WriteFrequencyHi(
                static_cast<byte>(0x80 | ((frequency >> 8) & 0x07)));

            channel.StepFrequencyTimer(period + 5);
            Assert::AreEqual(0x00, (int)channel.Amplitude());
            channel.StepFrequencyTimer(1);
            Assert::AreEqual(0x02, (int)channel.Amplitude());

            channel.StepFrequencyTimer(period);
            Assert::AreEqual(0x03, (int)channel.Amplitude());
        }
    }

    TEST_METHOD(WaveRamTwoCycleApertureTest)
    {
        WaveChannel channel;
        channel.OnPowerOn();
        channel.WriteWaveRAM(0xFF30, 0x12);
        channel.WriteWaveRAM(0xFF31, 0x34);
        channel.WriteDacEnable(0x80);
        channel.WriteVolume(0x20);
        channel.WriteFrequencyLo(0xFD);
        channel.WriteFrequencyHi(0x87);

        channel.StepFrequencyTimer(12);
        Assert::AreEqual(0x12, (int)channel.ReadWaveRAM(0xFF30));
        channel.StepFrequencyTimer(1);
        Assert::AreEqual(0x12, (int)channel.ReadWaveRAM(0xFF30));
        channel.StepFrequencyTimer(1);
        Assert::AreEqual(0xFF, (int)channel.ReadWaveRAM(0xFF30));
    }

    TEST_METHOD(WaveChannelRetriggerOverlapTest)
    {
        WaveChannel channel;
        channel.OnPowerOn();
        channel.WriteWaveRAM(0xFF30, 0x12);
        channel.WriteWaveRAM(0xFF31, 0x34);
        channel.WriteDacEnable(0x80);
        channel.WriteFrequencyLo(0xFF);
        channel.WriteFrequencyHi(0x87);

        channel.StepFrequencyTimer(8);
        channel.WriteFrequencyHi(0x87);
        channel.WriteDacEnable(0x00);
        Assert::AreEqual(0x34, (int)channel.ReadWaveRAM(0xFF30));
    }

    // Obscure behavior, continued: retriggering CH3 while it's already
    // running does not refresh the sample buffer from wave RAM either, even
    // if wave RAM was rewritten in the meantime - the stale buffer keeps
    // playing until the channel naturally advances again.
    TEST_METHOD(WaveChannelRetriggerDoesNotRefreshBufferTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(Channel3OutputLevel, 0x20); // volume code 1 -> 100%
        apu.WriteByte(Channel3FrequencyLower, 0x00);

        apu.WriteByte(0xFF30, 0x30); // byte 0: high nibble 0x3, low nibble 0x0
        apu.WriteByte(Channel3OnOff, 0x80);           // DAC on
        apu.WriteByte(Channel3FrequnecyHigher, 0x80); // trigger #1, freq = 0

        // The first period includes the six-T-cycle trigger-pipeline delay,
        // then buffers wave RAM byte 0 (0x30) and lands on its low nibble.
        apu.Step(4102);
        Assert::AreEqual(0x00, (int)apu.GetChannelAmplitude(CH3));

        // Rewrite byte 0 to something clearly distinguishable, then
        // retrigger. If the buffer were (incorrectly) refreshed on trigger,
        // amplitude would become 0xF (the new byte's high nibble); real
        // hardware instead keeps emitting the old buffered byte's high
        // nibble (0x3) until the waveform advances again.
        apu.WriteByte(0xFF30, 0xF0);
        apu.WriteByte(Channel3FrequnecyHigher, 0x80); // trigger #2

        Assert::AreEqual(0x03, (int)apu.GetChannelAmplitude(CH3));
    }

    // Obscure behavior: a pulse channel's duty step counter doesn't begin
    // advancing/outputting anything but silence until it has been clocked
    // at least once since the APU was last powered on - even though the
    // duty waveform's bit 0 (position 0) may itself be "on".
    TEST_METHOD(DutyStepSilentUntilFirstAdvanceTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(Channel1LengthWavePatternDuty, 0xC0); // duty 3 (0x7E): bit 1 set
        apu.WriteByte(Channel1VolumeEnvelope, 0x80);        // volume 8, DAC on
        apu.WriteByte(Channel1FrequencyHi, 0x80);           // trigger, freq = 0

        Assert::AreEqual(0, (int)apu.GetChannelAmplitude(CH1));

        // One full period (8192 cycles) advances the duty position to 1 and
        // clears the "not yet stepped" flag; duty 3's pattern has bit 1 set,
        // so the channel now outputs its current volume normally.
        apu.Step(8192);
        Assert::AreEqual(8, (int)apu.GetChannelAmplitude(CH1));
    }

    // Obscure behavior: if a channel is triggered exactly when the next
    // DIV-APU step will clock the volume envelope, the envelope timer is
    // reloaded with one greater than usual, effectively absorbing that
    // upcoming clock rather than acting on it immediately.
    TEST_METHOD(EnvelopeTimerReloadQuirkOnTriggerTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(Channel1LengthWavePatternDuty, 0xC0); // duty 3 (0x7E), bit 1 set

        // Advance the frame sequencer to step 6, so the next clock (step 7)
        // is the one that clocks the envelope.
        apu.Step(8192 * 7);
        Assert::AreEqual(6, (int)apu.GetFrameSequencerStep());

        apu.WriteByte(Channel1VolumeEnvelope, 0x19); // initial volume 1, increase, period 1
        apu.WriteByte(Channel1FrequencyHi, 0x80);    // trigger, freq = 0

        // This step both clocks the frame sequencer to step 7 (an envelope
        // clock) and advances the duty position to 1 (clearing the "not yet
        // stepped" silence above), letting amplitude directly reflect the
        // current volume. The quirk means this clock is absorbed by the
        // extra timer tick, so volume has NOT yet incremented.
        apu.Step(8192);
        Assert::AreEqual(1, (int)apu.GetChannelAmplitude(CH1));

        // The next envelope clock (8 more frame-sequencer steps later) is a
        // "normal" one and does increment the volume, proving the quirk only
        // delays the very first clock rather than breaking the envelope.
        apu.Step(8192 * 8);
        Assert::AreEqual(2, (int)apu.GetChannelAmplitude(CH1));
    }

    // Obscure behavior: a noise channel clock shift of 14 or 15 means the
    // LFSR receives no clocks at all, regardless of how long the channel
    // runs.
    TEST_METHOD(NoiseChannelClockShift14NoClocksTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);

        apu.WriteByte(Channel4VolumeEnvelope, 0x80);    // volume 8, DAC on
        apu.WriteByte(Channel4PolynomialCounter, 0xE0); // clock shift 14, 15-bit, divisor 0
        apu.WriteByte(Channel4Counter, 0x80);           // trigger

        Assert::IsTrue(apu.IsChannelEnabled(CH4));

        byte before = apu.GetChannelAmplitude(CH4);

        // Stepping far more cycles than any plausible period would normally
        // require leaves the output completely unchanged.
        apu.Step(4194304);
        Assert::AreEqual((int)before, (int)apu.GetChannelAmplitude(CH4));
    }

    // Volume code 0 (mute) silences the wave channel's output entirely,
    // regardless of wave RAM contents.
    TEST_METHOD(WaveChannelMuteVolumeTest)
    {
        APU apu;

        apu.WriteByte(0xFF30, 0xFF);

        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(Channel3OnOff, 0x80);
        apu.WriteByte(Channel3OutputLevel, 0x00); // volume code 0 -> mute
        apu.WriteByte(Channel3FrequnecyHigher, 0x80);

        Assert::AreEqual(0, (int)apu.GetChannelAmplitude(CH3));
    }

    // The noise channel's LFSR starts at 0x7FFF on trigger (bit 0 clear ->
    // amplitude reflects the envelope's initial volume), and evolves
    // deterministically as StepFrequencyTimer runs.
    TEST_METHOD(NoiseChannelTriggerAmplitudeTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80);

        apu.WriteByte(Channel4VolumeEnvelope, 0x80); // volume 8, DAC on
        apu.WriteByte(Channel4PolynomialCounter, 0x00); // shift 0, 15-bit, divisor code 0 -> period 8
        apu.WriteByte(Channel4Counter, 0x80); // trigger

        Assert::IsTrue(apu.IsChannelEnabled(CH4));

        // LFSR bit 0 starts clear (0x7FFF has bit 0 set... wait: 0x7FFF & 1 == 1)
        // so the very first output is silence (amplitude 0) until the LFSR
        // is first clocked.
        Assert::AreEqual(0, (int)apu.GetChannelAmplitude(CH4));

        // Stepping one full period (8 cycles) clocks the LFSR once:
        // bit0 ^ bit1 of 0x7FFF = 1 ^ 1 = 0, so the new bit15 is 0 and the
        // shifted LFSR's bit0 becomes ((0x7FFF >> 1) & 1) = 1, still silent.
        apu.Step(8);
        Assert::AreEqual(0, (int)apu.GetChannelAmplitude(CH4));
    }

    // NR51 routes each channel independently to the left/right mixer inputs,
    // and NR50 scales the resulting per-side volume - verified end-to-end via
    // the deterministic, thread-independent sample stream.
    TEST_METHOD(PanningAndVolumeMixingTest)
    {
        APU apu;

        // 1:1 sample rate so Step() produces one sample per T-cycle once the
        // internal accumulator has drained, keeping the math simple/exact.
        apu.SetSampleRate(4194304);

        apu.WriteByte(SoundOnOff, 0x80);
        apu.WriteByte(Channel1LengthWavePatternDuty, 0x40); // duty 1 (0x81): bit 0 set
        apu.WriteByte(Channel1VolumeEnvelope, 0x80); // volume 8, DAC on
        apu.WriteByte(Channel1FrequencyHi, 0x80);    // trigger, freq = 0 (long period, duty position stays put)

        apu.WriteByte(OutputTerminalSelection, 0x10); // channel 1 -> left (SO2) only
        apu.WriteByte(ChannelControl, 0x70);           // left volume = 7 (max), right volume = 0, Vin off

        // Obscure behavior: the duty step counter's very first advance after
        // a trigger is forced to silence (see SquareChannel::Amplitude()).
        // Prime it by stepping exactly one full duty cycle (8 steps of the
        // 8192-cycle period), which returns the position to 0 - matching
        // duty 1's (0x81) bit 0 "on" - while leaving the "stepped" flag
        // set, so the capture window below behaves as a steady tone.
        apu.Step(8192 * 8);

        apu.ClearSampleBuffer();
        apu.Step(300); // drain the initial accumulator and generate several samples

        Assert::IsTrue(apu.GetPendingSampleCount() > 0);

        APU::Sample samples[300];
        size_t count = apu.ConsumeSamples(samples, 300);
        Assert::IsTrue(count > 0);

        // Channel 1 duty 1 (0x81) bit 0 is set, so amplitude == current
        // volume (8). Analog DAC value = 1.0 - (8 / 7.5). Left volume factor
        // = (7 + 1) / 8.0 = 1.0. Only one channel contributes to the left
        // sum, which is then divided by 4 (four mixer inputs).
        //
        // This is asserted against the raw mixer output: the sample stream
        // itself is DC-blocked (see DCOffsetIsRemovedFromOutputTest), which
        // by design removes exactly this kind of steady offset.
        float expectedAnalog = 1.0f - (8.0f / 7.5f);
        float expectedLeft = (expectedAnalog / 4.0f) * 1.0f;

        APU::Sample mixed = apu.GetMixerOutput();
        Assert::IsTrue(std::fabs(mixed.Left - expectedLeft) < 0.0001f);
        Assert::IsTrue(mixed.Right == 0.0f);
    }

    // An enabled-but-idle DAC sits at a rail, so the raw mixer output carries
    // a large constant offset. Hardware strips it with the output coupling
    // capacitor; without that the audible signal rides at full scale and
    // routing changes turn into full-scale steps (audible as static).
    TEST_METHOD(DCOffsetIsRemovedFromOutputTest)
    {
        APU apu;
        apu.SetSampleRate(44100);

        apu.WriteByte(SoundOnOff, 0x80);
        // DAC on but the channel is never triggered: digital output stays 0,
        // which the DAC drives to a rail. This is the real source of the
        // offset - it is pure DC, with no audio content to preserve.
        apu.WriteByte(Channel1VolumeEnvelope, 0x80);
        apu.WriteByte(OutputTerminalSelection, 0x11); // channel 1 -> both sides
        apu.WriteByte(ChannelControl, 0x77);          // full volume both sides

        // The raw mixer output is a large steady offset...
        apu.Step(4194304 / 60);
        APU::Sample mixed = apu.GetMixerOutput();
        Assert::IsTrue(std::fabs(mixed.Left) > 0.2f);

        // ...which the output stage decays away. Give the filter a second to
        // settle, then inspect the samples produced after that.
        apu.Step(4194304);
        apu.ClearSampleBuffer();
        apu.Step(4194304 / 60);

        APU::Sample samples[64];
        size_t count = apu.ConsumeSamples(samples, 64);
        Assert::IsTrue(count > 0);

        for (size_t i = 0; i < count; i++)
        {
            Assert::IsTrue(std::fabs(samples[i].Left) < 0.01f);
            Assert::IsTrue(std::fabs(samples[i].Right) < 0.01f);
        }

        // The mixer stage itself is untouched by the filter.
        Assert::IsTrue(std::fabs(apu.GetMixerOutput().Left) > 0.2f);
    }

    // No audio is produced while the APU is powered off, regardless of any
    // channel register contents left over from before power-off.
    TEST_METHOD(SilentWhenPoweredOffTest)
    {
        APU apu;
        apu.SetSampleRate(4194304);

        apu.ClearSampleBuffer();
        apu.Step(300);

        APU::Sample samples[300];
        size_t count = apu.ConsumeSamples(samples, 300);
        Assert::IsTrue(count > 0);

        for (size_t i = 0; i < count; i++)
        {
            Assert::IsTrue(samples[i].Left == 0.0f);
            Assert::IsTrue(samples[i].Right == 0.0f);
        }
    }

    // Write-only bits read back as 1 (standard DMG APU register masks),
    // verified here for NR11/NR21 (length is write-only, duty is readable).
    TEST_METHOD(RegisterReadMasksTest)
    {
        APU apu;
        apu.WriteByte(SoundOnOff, 0x80); // Power on: duty bits are only latched while powered.

        // Duty = 2, length load = 5 -> length bits always read back as 1.
        apu.WriteByte(Channel1LengthWavePatternDuty, 0x85);
        Assert::AreEqual(0xBF, (int)apu.ReadByte(Channel1LengthWavePatternDuty));

        // NR13/NR23/NR31/NR33 are write-only and always read back as 0xFF.
        apu.WriteByte(Channel1FrequencyLo, 0x42);
        Assert::AreEqual(0xFF, (int)apu.ReadByte(Channel1FrequencyLo));
    }
};
