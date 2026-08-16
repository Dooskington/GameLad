#pragma once

class IMMU
{
public:
    virtual ~IMMU() {}
    virtual void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit) = 0;
    virtual unsigned short ReadUShort(const ushort& address) = 0;
    virtual bool LoadBootROM(const char* bootROMPath) = 0;

    virtual byte Read(const ushort& address) = 0;
    virtual bool Write(const ushort& address, const byte val) = 0;

    // Hardware model. Pushed down once at ROM-load time so no component has to
    // re-derive the mode from the cartridge header.
    virtual void SetGameBoyMode(GameBoyMode mode) = 0;
    virtual GameBoyMode GetGameBoyMode() const = 0;

    /*
        KEY1 (0xFF4D) speed switching.

        The MMU owns KEY1 because it is the register file; the CPU owns the
        switch itself because STOP is what actually performs it. IsSpeedSwitchArmed()
        reports bit 0 (prepare), CompleteSpeedSwitch() flips bit 7 (current speed)
        and clears the prepare bit, matching what the hardware does when STOP is
        executed with the prepare bit set.
    */
    virtual bool IsSpeedSwitchArmed() const = 0;
    virtual void CompleteSpeedSwitch() = 0;
    virtual bool IsDoubleSpeed() const = 0;
};
