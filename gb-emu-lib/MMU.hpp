#pragma once

class MMU : public IMMU
{
public:
    MMU();
    ~MMU();

    void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit);
    unsigned short ReadUShort(const ushort& address);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

    ushort GetIE();
    ushort GetIF();
    void SetIF(const byte& val);

private:
    byte ReadByteInternal(const ushort& address);
    bool WriteByteInternal(const ushort& address, const byte val);

private:
    // Booting
    byte m_isBooting;

    // Memory
    IMemoryUnit* m_memoryUnits[0xFFFF + 1];
    byte m_bank0[0x0FFF + 1];   // 4k work RAM Bank 0
    byte m_bank1[0x0FFF + 1];   // 4k work RAM Bank 1
    byte m_HRAM[0x007E + 1];    // HRAM

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
    // TODO: TESTS FOR INTERRUPTS!!!!
};
