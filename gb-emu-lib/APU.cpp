#include "pch.hpp"
#include "APU.hpp"

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
        &m_SoundOnOff,
        &m_Channel1Sweep,
        &m_Channel1SoundLength,
        &m_Channel1VolumeEnvelope,
        &m_Channel1FrequencyLo,
        &m_Channel1FrequencyHi
    ),
    m_Channel2SoundGenerator(
        CHANNEL_2,
        &m_SoundOnOff,
        nullptr, // No sweep register on CH2
        &m_Channel2SoundLength,
        &m_Channel2VolumeEnvelope,
        &m_Channel2FrequencyLo,
        &m_Channel2FrequencyHi
    ),
    m_Channel3SoundGenerator(
        CHANNEL_3,
        &m_SoundOnOff,
        &m_Channel3SoundOnOff,
        &m_Channel3SoundLength,
        &m_Channel3SelectOutputLevel,
        &m_Channel3FrequencyLo,
        &m_Channel3FrequencyHi,
        m_WavePatternRAM
    ),
    m_Channel4SoundGenerator(
        CHANNEL_4,
        &m_SoundOnOff,
        &m_Channel4SoundLength,
        &m_Channel4VolumeEnvelope,
        &m_Channel4PolynomialCounter,
        &m_Channel4Counter
    ),
    m_Initialized(false),
    m_AudioDevice(0),
    m_AudioFrameRemainder(0.0),
    m_OutputBuffer(AudioBufferSize, FrameSizeBytes)
{
    memset(m_WavePatternRAM, 0x00, ARRAYSIZE(m_WavePatternRAM));

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

        // 0 = mute; 7 = max volume;
        // Output volume = OutputLevel / 7, then scale by 1/4
        so1 *= ((float)OutputLevelSO1 / 28); 
        so2 *= ((float)OutputLevelSO2 / 28);

        float f_frame[2] = {so1, so2};
        m_OutputBuffer.Put((Uint8 *)f_frame);
    }
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
        return true;
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
        m_Channel3SoundGenerator.TriggerChannelSoundOnOffRegisterUpdate();
        return true;
    case Channel3Length:
        m_Channel3SoundLength = val;
        m_Channel3SoundGenerator.TriggerSoundLengthRegisterUpdate();
        return true;
    case Channel3OutputLevel:
        m_Channel3SelectOutputLevel = val;
        m_Channel3SoundGenerator.TriggerSelectOutputLevelRegisterUpdate();
        return true;
    case Channel3FrequencyLower:
        m_Channel3FrequencyLo = val;
        m_Channel3SoundGenerator.TriggerFrequencyLoRegisterUpdate();
        return true;
    case Channel3FrequnecyHigher:
        m_Channel3FrequencyHi = val;
        m_Channel3SoundGenerator.TriggerFrequencyHiRegisterUpdate();
        return true;
    case Channel4Length:
        m_Channel4SoundLength = val;
        m_Channel4SoundGenerator.TriggerSoundLengthRegisterUpdate();
        return true;
    case Channel4VolumeEnvelope:
        m_Channel4VolumeEnvelope = val;
        m_Channel4SoundGenerator.TriggerVolumeEnvelopeRegisterUpdate();
        return true;
    case Channel4PolynomialCounter:
        m_Channel4PolynomialCounter = val;
        m_Channel4SoundGenerator.TriggerPolynomialCounterRegisterUpdate();
        return true;
    case Channel4Counter:
        m_Channel4Counter = val;
        m_Channel4SoundGenerator.TriggerCounterRegisterUpdate();
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

APU::SoundGenerator::SoundGenerator(
    const APUChannel channel,
    const byte *soundOnOffRegister)
    : m_Channel(channel),
      m_SoundOnOffRegister(soundOnOffRegister)
{
}

float APU::SoundGenerator::NextSample() {
    float sample = 0.0;

    if (!m_Enabled) {
        return sample;
    }

    bool is_sound_length_expired = m_CounterModeEnabled && m_SoundLengthTimerSeconds > m_SoundLengthSeconds;

    if (is_sound_length_expired && !m_SoundLengthExpired)
    {
        // Sound length has expired since the previous sample - reset 
        // the sound on/off flag to indicate the sound no longer on
        m_SoundLengthExpired = true;
        ResetSoundOnOffFlag();
    }

    if (!m_SoundLengthExpired)
    {
        // Skip generating the sample if the sound is not playing
        sample = NextWaveformSample();
    }

    double volume = m_EnvelopeStartVolume;
    if (m_EnvelopeModeEnabled)
    {
        // GB apparently has 16 "levels" of volume due to the value being stored as a 4 bit number.
        // Emulate this through applying a 1/16 change in volume per step.
        int step_number = (int)(m_SoundLengthTimerSeconds / m_EnvelopeStepLengthSeconds);
        volume += m_EnvelopeDirection * ((double)step_number / 16.0);
        if (volume < 0.0) { // Clamp volume between 0 and 1
            volume = 0.0;
        } else if (volume > 1.0) {
            volume = 1.0;
        }
    }
    sample *= volume;

    if (m_SweepModeEnabled)
    {
        // The new frequency during the sweep function is calculated through applying
        // a bit shift to the binary data stored in the two frequency registers. In order
        // to apply this computation we need the actual value from the registers themsevles.
        // Note: Only Channel 1 (square wave) supports sweep.
        uint adjusted_frequency = m_FrequencyRegisterData;
        int step_number = m_SoundLengthTimerSeconds / m_SweepStepLengthSeconds;

        // Apply the sweep algorithm to determine the new frequency:
        // X(t) = X(t-1) +/- X(t-1)/2^n
        for (int i = 0; i < step_number; i++)
        {
            if (m_SweepDirection > 0) {
                adjusted_frequency += (adjusted_frequency >> m_SweepShiftFrequencyExponent);
            } else {
                adjusted_frequency -= (adjusted_frequency >> m_SweepShiftFrequencyExponent);
            }
        }

        // Clamp adjusted frequency between 0 and 2047
        if (adjusted_frequency < 0) {
            adjusted_frequency = 0;
        } else if (adjusted_frequency > 2047) {
            adjusted_frequency = 2047;
        }

        UpdateFrequency(adjusted_frequency);
    }

    // Adjust the phase based on the duration of 1 audio sample and wrap.
    m_Phase += (TwoPi * m_FrequencyHz) / (double)AudioSampleRate;
    while (m_Phase >= TwoPi)
    {
        m_Phase -= TwoPi;
    }

    // Increase the timer for sound length based on the duration of 1 audio sample
    m_SoundLengthTimerSeconds += 1.0 / (double)AudioSampleRate;

    return sample;
}

void APU::SoundGenerator::RestartSound()
{
    m_SoundLengthExpired = false;
    m_SoundLengthTimerSeconds = 0.0;
    SetSoundOnOffFlag();
}

void APU::SoundGenerator::SetSoundOnOffFlag()
{
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

void APU::SoundGenerator::ResetSoundOnOffFlag() 
{
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

APU::SquareWaveGenerator::SquareWaveGenerator(
    const APUChannel channel,
    const byte *soundOnOffRegister,
    const byte *sweepRegister,
    const byte *soundLengthRegister,
    const byte *volumeEnvelopeRegister,
    const byte *frequencyLoRegister,
    const byte *frequencyHiRegister)
    : SoundGenerator(channel, soundOnOffRegister),
      m_SweepRegister(sweepRegister),
      m_SoundLengthRegister(soundLengthRegister),
      m_VolumeEnvelopeRegister(volumeEnvelopeRegister),
      m_FrequencyLoRegister(frequencyLoRegister),
      m_FrequencyHiRegister(frequencyHiRegister)
{
    memset(m_Coefficients, 0.0, ARRAYSIZE(m_Coefficients));
}

void APU::SquareWaveGenerator::TriggerSweepRegisterUpdate()
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

void APU::SquareWaveGenerator::TriggerSoundLengthRegisterUpdate()
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

void APU::SquareWaveGenerator::TriggerVolumeEnvelopeRegisterUpdate()
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

void APU::SquareWaveGenerator::TriggerFrequencyLoRegisterUpdate()
{
    // Bit 7-0 - Low 8 bits of the frequency data
    byte frequency_lo_register = *m_FrequencyLoRegister;
    byte frequency_hi_register = *m_FrequencyHiRegister;
    uint frequency_reg_value = ((frequency_hi_register << 8) | frequency_lo_register) & 0x7FF;

    UpdateFrequency(frequency_reg_value);

    // Store the frequency value from the registers as well so that it can be
    // used by the frequency sweep functionality
    m_FrequencyRegisterData = frequency_reg_value;
}

void APU::SquareWaveGenerator::TriggerFrequencyHiRegisterUpdate()
{
    // Bit 2-0 - High 3 bits of the frequency data
    byte frequency_lo_register = *m_FrequencyLoRegister;
    byte frequency_hi_register = *m_FrequencyHiRegister;
    uint frequency_reg_value = ((frequency_hi_register << 8) | frequency_lo_register) & 0x7FF;

    UpdateFrequency(frequency_reg_value);

    // Store the frequency value from the registers as well so that it can be
    // used by the frequency sweep functionality
    m_FrequencyRegisterData = frequency_reg_value;

    // Bit 6 - Counter/consecutive selection
    m_CounterModeEnabled = ISBITSET(frequency_hi_register, 6);

    // Bit 7 - Initial
    if (ISBITSET(frequency_hi_register, 7))
    {
        RestartSound();
    }
}

float APU::SquareWaveGenerator::NextWaveformSample() {
    float sample = 0.0;
    // Build the square wave using additive synthesis
    for (int j = 0; j < m_HarmonicsCount; j++)
    {
        sample += m_Coefficients[j] * cos(j * m_Phase);
    }
    return sample;
}

void APU::SquareWaveGenerator::UpdateFrequency(uint frequencyRegValue)
{
    // Frequency data (11 bits total)
    // For x = the value in the frequency registers, the actual frequency
    // in Hz is 131072/(2048-x) Hz
    m_FrequencyHz = (131072.0 / (double)(2048 - frequencyRegValue));

    // Coefficients must be recalculated whenever the frequency is modified
    RegenerateCoefficients();
}

void APU::SquareWaveGenerator::RegenerateCoefficients()
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

APU::NoiseGenerator::NoiseGenerator(
    const APUChannel channel,
    const byte* soundOnOffRegister,
    const byte* soundLengthRegister,
    const byte* volumeEnvelopeRegister,
    const byte* polynomialCounterRegister,
    const byte* counterRegister
) :
    SoundGenerator(channel, soundOnOffRegister),
    m_SoundLengthRegister(soundLengthRegister),
    m_VolumeEnvelopeRegister(volumeEnvelopeRegister),
    m_PolynomialCounterRegister(polynomialCounterRegister),
    m_CounterRegister(counterRegister)
{
}

void APU::NoiseGenerator::TriggerSoundLengthRegisterUpdate()
{
    byte sound_length_register = *m_SoundLengthRegister;

    // Bit 5-0 - Sound Length
    // Sound Length = (64-n)*(1/256) seconds
    byte sound_length = sound_length_register & 0x3F;
    m_SoundLengthSeconds = (64.0 - (double)sound_length) / 256.0;
}

void APU::NoiseGenerator::TriggerVolumeEnvelopeRegisterUpdate()
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

void APU::NoiseGenerator::TriggerPolynomialCounterRegisterUpdate()
{
    byte polynomial_counter_register = *m_PolynomialCounterRegister;

    UpdateFrequency(polynomial_counter_register);

    // Bit 3 - Counter step (0=15 bits, 1=7 bits)
    byte counter_step = ISBITSET(polynomial_counter_register, 3) ? 7 : 15;
    // TODO - where to use this?
}

void APU::NoiseGenerator::TriggerCounterRegisterUpdate()
{
    byte counter_register = *m_CounterRegister;

    // Bit 6 - Counter/consecutive selection
    m_CounterModeEnabled = ISBITSET(counter_register, 6);

    // Bit 7 - Initial
    if (ISBITSET(counter_register, 7))
    {
        RestartSound();
    }
}

float APU::NoiseGenerator::NextWaveformSample() {
    if (m_Phase < m_PreviousSamplePhase) // A wrap-around occurred
    {
        // TODO: Model real GB behavior
        float r = rand() / (RAND_MAX + 1.0);
        m_Signal = r > 0.5 ? 0.5 : -0.5;
    }
    m_PreviousSamplePhase = m_Phase;
    return m_Signal;
}

void APU::NoiseGenerator::UpdateFrequency(uint polynomial_counter_register) {
    // Bit 2-0 - Dividing ratio of frequency (r)
    byte divide_ratio = polynomial_counter_register & 7;

    // Bit 7-4 - Shift clock frequency (s)
    byte shift_clock_frequency = (polynomial_counter_register >> 4) & 0xF;

    // 524288 Hz / r / 2^(s+1)
    // For r=0 assume r=0.5 instead
    double r = divide_ratio == 0 ? 0.5 : (double)divide_ratio;
    m_FrequencyHz = 524288.0 / r / (double)(2 << shift_clock_frequency);
}

APU::WaveformGenerator::WaveformGenerator(
    const APUChannel channel,
    const byte* soundOnOffRegister,
    const byte* channelSoundOnOffRegister,
    const byte* soundLengthRegister,
    const byte* selectOutputLevelRegister,
    const byte* frequencyLoRegister,
    const byte* frequencyHiRegister,
    const byte* waveBuffer
) :
    SoundGenerator(channel, soundOnOffRegister),
    m_ChannelSoundOnOffRegister(channelSoundOnOffRegister),
    m_SoundLengthRegister(soundLengthRegister),
    m_SelectOutputLevelRegister(selectOutputLevelRegister),
    m_FrequencyLoRegister(frequencyLoRegister),
    m_FrequencyHiRegister(frequencyHiRegister),
    m_WaveBuffer(waveBuffer)
{
}

void APU::WaveformGenerator::TriggerChannelSoundOnOffRegisterUpdate()
{
    // Bit 7 - Sound Channel 3 Off (0=Stop, 1=Playback)
    m_Enabled = ISBITSET(*m_ChannelSoundOnOffRegister, 7);
}

void APU::WaveformGenerator::TriggerSoundLengthRegisterUpdate()
{
    byte sound_length_register = *m_SoundLengthRegister;

    // Bit 7-0 - Sound length (t1: 0 - 255)
    // Sound Length = (256-t1)*(1/256) seconds 
    m_SoundLengthSeconds = (256.0 - (double)sound_length_register) / 256.0;
}

void APU::WaveformGenerator::TriggerSelectOutputLevelRegisterUpdate()
{
    // Bit 6-5 - Select output level
    byte sound_output_level = ((*m_SelectOutputLevelRegister) >> 5) & 0x3;

    // 0: Mute
    // 1: 100%
    // 2: 50%
    // 3: 25%
    switch (sound_output_level)
    {
    case 0:
        m_EnvelopeStartVolume = 0.0;
        break;
    case 1:
        m_EnvelopeStartVolume = 1.0;
        break;
    case 2:
        m_EnvelopeStartVolume = 0.5;
        break;
    case 3:
        m_EnvelopeStartVolume = 0.25;
    }
}

void APU::WaveformGenerator::TriggerFrequencyLoRegisterUpdate()
{
    // Bit 7-0 - Low 8 bits of the frequency data
    byte frequency_lo_register = *m_FrequencyLoRegister;
    byte frequency_hi_register = *m_FrequencyHiRegister;
    UpdateFrequency(((frequency_hi_register << 8) | frequency_lo_register) & 0x7FF);
}

void APU::WaveformGenerator::TriggerFrequencyHiRegisterUpdate()
{
    // Bit 2-0 - High 3 bits of the frequency data
    byte frequency_lo_register = *m_FrequencyLoRegister;
    byte frequency_hi_register = *m_FrequencyHiRegister;
    UpdateFrequency(((frequency_hi_register << 8) | frequency_lo_register) & 0x7FF);

    // Bit 6 - Counter/consecutive selection
    m_CounterModeEnabled = ISBITSET(frequency_hi_register, 6);

    // Bit 7 - Initial
    if (ISBITSET(frequency_hi_register, 7))
    {
        RestartSound();
    }
}

float APU::WaveformGenerator::NextWaveformSample() 
{
    // Wave is made up of 32 4-bit samples
    int wave_ram_sample_number = (int)((m_Phase / TwoPi) * 32.0);
    int wave_ram_byte_offset = wave_ram_sample_number / 2;
    int shift = ((1 + wave_ram_sample_number) % 2) * 4; // shift 0 or 4 bits.
    int wave_ram_sample = (m_WaveBuffer[wave_ram_byte_offset] >> shift) & 0xF;
    float sample = ((double)wave_ram_sample / 16.0) - 0.5;

    // TODO: Emulate bit-shift sound level scaling rather than relying on the volume envelope

    return sample;
}

void APU::WaveformGenerator::UpdateFrequency(uint frequencyRegValue)
{
    // Frequency data (11 bits total)
    // For x = the value in the frequency registers, the actual frequency
    // is 65536/(2048-x) Hz
    m_FrequencyHz = 65536.0 / (double)(2048 - frequencyRegValue);
}

// TODO: Move this out of the APU class
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
