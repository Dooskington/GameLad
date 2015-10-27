#include "PCH.hpp"
#include "GPU.hpp"

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

GPU::GPU()
{
    Logger::Log("GPU created.");
}

GPU::~GPU()
{
    Logger::Log("GPU destroyed.");
}

// IMemoryUnit
byte GPU::ReadByte(const ushort& address)
{
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        // TODO: Test mode to see if available
        return m_VRAM[address - 0x8000];
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        // TODO: Test mode to see if available
        return m_OAM[address - 0xFE00];
    }

    switch (address)
    {
    case LCDControl:
    case LCDControllerStatus:
    case ScrollY:
    case ScrollX:
    case LCDControllerYCoordinate:
    case LYCompare:
    case WindowYPosition:
    case WindowXPositionMinus7:
    case BGPaletteData:
    case ObjectPalette0Data:
    case ObjectPalette1Data:
    case DMATransferAndStartAddress:
        // TODO: NYI
        return 0x00;
    default:
        Logger::Log("GPU::ReadByte cannot read from address 0x%04X", address);
        return 0x00;
    }
}

bool GPU::WriteByte(const ushort& address, const byte val)
{
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        // TODO: Test mode to see if available
        m_VRAM[address - 0x8000] = val;
        return true;
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        // TODO: Test mode to see if available
        m_OAM[address - 0xFE00] = val;
        return true;
    }

    switch (address)
    {
    case LCDControl:
    case LCDControllerStatus:
    case ScrollY:
    case ScrollX:
    case LCDControllerYCoordinate:
    case LYCompare:
    case WindowYPosition:
    case WindowXPositionMinus7:
    case BGPaletteData:
    case ObjectPalette0Data:
    case ObjectPalette1Data:
    case DMATransferAndStartAddress:
        // TODO: NYI
        return true;
    default:
        Logger::Log("GPU::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}
