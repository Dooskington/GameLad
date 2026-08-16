#pragma once

class MMU : public IMMU, IMemoryUnit
{
public:
    MMU();
    ~MMU();

    void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit);
    unsigned short ReadUShort(const ushort& address);
    bool LoadBootROM(const char* bootROMPath);

    byte Read(const ushort& address);
    bool Write(const ushort& address, const byte val);

    void SetGameBoyMode(GameBoyMode mode);
    GameBoyMode GetGameBoyMode() const { return m_mode; }

    bool IsSpeedSwitchArmed() const { return m_speedSwitchArmed; }
    void CompleteSpeedSwitch();
    bool IsDoubleSpeed() const { return m_doubleSpeed; }

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    byte ReadIORegister(const ushort& address);
    bool WriteIORegister(const ushort& address, const byte val);
    // Effective WRAM bank for 0xD000-0xDFFF. SVBK selects banks 1-7; a written
    // value of 0 selects bank 1. DMG always sees the single fixed bank 1.
    unsigned int CurrentWRAMBank() const;

private:
    // Booting
    byte m_isBooting;
    std::unique_ptr<byte[]> m_BIOS;

    // Hardware model
    GameBoyMode m_mode;

    // Memory
    IMemoryUnit* m_memoryUnits[0xFFFF + 1];
    /*
        Work RAM. A DMG has two fixed 4 KB banks; a CGB has eight, with bank 0
        fixed at 0xC000-0xCFFF and banks 1-7 switchable at 0xD000-0xDFFF via
        SVBK. Using the eight-bank array for both keeps a single code path -
        DMG simply never leaves bank 1.
    */
    byte m_WRAM[8][0x0FFF + 1];
    byte m_SVBK;                // 0xFF70, raw written value
    byte m_HRAM[0x007E + 1];    // HRAM

    // CGB speed switching (0xFF4D KEY1)
    bool m_speedSwitchArmed;
    bool m_doubleSpeed;

    /*
        CGB-only I/O that has no other owner.
        0xFF56 RP   - infrared port
        0xFF6C OPRI - object priority mode
        0xFF72-0xFF75 - undocumented but readable/writable CGB registers
    */
    byte m_RP;
    byte m_OPRI;
    byte m_undocumented72;
    byte m_undocumented73;
    byte m_undocumented74;
    byte m_undocumented75;

    /*
        Interrupts

        Bit     When 0          When 1
        0       VBlank off      VBlank on
        1       LCD stat off    LCD stat on
        2       Timer off       Timer on
        3       Serial off      Serial on
        4       Joypad off      Joypad on

        Interrupt enable - When bits are set, we care about the corresponding interrupt.
        Interrupt flags - When bits are set, an interrupt has happened.
    */
    byte m_IE; // Interrupt enable register (0xFFFF)
    byte m_IF; // Interrupt flag register (0xFF0F)
};
