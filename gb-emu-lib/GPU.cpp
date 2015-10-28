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

GPU::GPU(IMMU* pMMU) :
    m_MMU(pMMU),
    m_LCDControl(0x00),
    m_LCDControllerStatus(0x00),
    m_ScrollY(0x00),
    m_ScrollX(0x00),
    m_LCDControllerYCoordinate(0x00),
    m_LYCompare(0x00),
    m_WindowYPosition(0x00),
    m_WindowXPositionMinus7(0x00),
    m_BGPaletteData(0x00),
    m_ObjectPalette0Data(0x00),
    m_ObjectPalette1Data(0x00)
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
        return m_LCDControl;
    case LCDControllerStatus:
        return m_LCDControllerStatus;
    case ScrollY:
        return m_ScrollY;
    case ScrollX:
        return m_ScrollX;
    case LCDControllerYCoordinate:
        return m_LCDControllerYCoordinate;
    case LYCompare:
        return m_LYCompare;
    case WindowYPosition:
        return m_WindowYPosition;
    case WindowXPositionMinus7:
        return m_WindowXPositionMinus7;
    case BGPaletteData:
        return m_BGPaletteData;
    case ObjectPalette0Data:
        return m_ObjectPalette0Data;
    case ObjectPalette1Data:
        return m_ObjectPalette1Data;
    case DMATransferAndStartAddress:
        Logger::Log("GPU::ReadByte cannot read from address 0x%04X (DMATransferAndStartAddress)", address);
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
        m_LCDControl = val;
        return true;
    case LCDControllerStatus:
        // Bits 7-3 are writable, Bits 2-0 are read only
        m_LCDControllerStatus = (val & 0xF8) | (m_LCDControllerStatus & 0x07);
        return true;
    case ScrollY:
        m_ScrollY = val;
        return true;
    case ScrollX:
        m_ScrollX = val;
        return true;
    case LCDControllerYCoordinate:
        Logger::Log("GPU::WriteByte cannot write to address 0x%04X (LCDControllerYCoordinate)", address);
        return false;
    case LYCompare:
        m_LYCompare = val;
        return true;
    case WindowYPosition:
        m_WindowYPosition = val;
        return true;
    case WindowXPositionMinus7:
        m_WindowXPositionMinus7 = val;
        return true;
    case BGPaletteData:
        m_BGPaletteData = val;
        return true;
    case ObjectPalette0Data:
        m_ObjectPalette0Data = val;
        return true;
    case ObjectPalette1Data:
        m_ObjectPalette1Data = val;
        return true;
    case DMATransferAndStartAddress:
        LaunchDMATransfer(val);
        return true;
    default:
        Logger::Log("GPU::WriteByte cannot write to address 0x%04X", address);
        return false;
    }
}

void GPU::LaunchDMATransfer(const byte address)
{
    /*
    DMA Transfer and Start Address
    Writing to this register launches a DMA transfer from ROM or RAM to OAM memory (sprite attribute
    table). The written value specifies the transfer source address divided by 100h, ie. source &
    destination are:

    Source:      XX00-XX9F   ;XX in range from 00-F1h
    Destination: FE00-FE9F
    */

    ushort source = (static_cast<ushort>(address) * 0x0100);
    for (byte offset = 0x00; offset < 0x9F; offset++)
    {
        WriteByte(0xFE00 | offset, m_MMU->ReadByte(source | offset));
    }
}
