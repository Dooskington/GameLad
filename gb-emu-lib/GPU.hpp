#pragma once

class GPU : public IMemoryUnit
{
public:
    GPU(IMMU* pMMU);
    ~GPU();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    void LaunchDMATransfer(const byte address);

private:
    IMMU* m_MMU;
    byte m_VRAM[0x1FFF + 1];
    byte m_OAM[0x009F + 1];
    
    byte m_LCDControl;
    byte m_LCDControllerStatus;
    byte m_ScrollY;
    byte m_ScrollX;
    byte m_LCDControllerYCoordinate;
    byte m_LYCompare;
    byte m_WindowYPosition;
    byte m_WindowXPositionMinus7;
    byte m_BGPaletteData;
    byte m_ObjectPalette0Data;
    byte m_ObjectPalette1Data;
};
