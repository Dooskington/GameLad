#pragma once

// FF40 - LCDC - LCD Control (R/W)
// FF41 - STAT - LCDC Status (R/W)
// FF42 - SCY - Scroll Y (R/W)
// FF43 - SCX - Scroll X (R/W)
// FF44 - LY - LCDC Y-Coordinate (R)
// FF45 - LYC - LY Compare (R/W)
// FF4A - WY - Window Y Position (R/W)
// FF4B - WX - Window X Position minus 7 (R/W)
// FF47 - BGP - BG Palette Data (R/W) - Non CGB Mode Only
// FF48 - OBP0 - Object Palette 0 Data (R/W) - Non CGB Mode Only
// FF49 - OBP1 - Object Palette 1 Data (R/W) - Non CGB Mode Only
// FF46 - DMA - DMA Transfer and Start Address (W)
#define LCDControl 0xFF40
#define LCDControllerStatus 0xFF41
#define ScrollY 0xFF42
#define ScrollX 0xFF43
#define LCDControllerYCoordinate 0xFF44
#define LYCompare 0xFF45
#define WindowYPosition 0xFF4A
#define WindowXPositionMinus7 0xFF4B
#define BGPaletteData 0xFF47
#define ObjectPalette0Data 0xFF48
#define ObjectPalette1Data 0xFF49
#define DMATransferAndStartAddress 0xFF46

#define ModeHBlank 0
#define ModeVBlank 1
#define ModeReadingOAM 2
#define ModeReadingOAMVRAM 3

#define VBlankCycles 4560
#define HBlankCycles 204
#define ReadingOAMCycles 80
#define ReadingOAMVRAMCycles 172

class GPU : public IMemoryUnit
{
    friend class GPUTests;

public:
    GPU(IMMU* pMMU);
    ~GPU();

    void Step(unsigned long cycles);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    void LaunchDMATransfer(const byte address);

private:
    IMMU* m_MMU;
    byte m_VRAM[0x1FFF + 1];
    byte m_OAM[0x009F + 1];

    unsigned long m_ModeClock;
    byte m_Mode;
    
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
