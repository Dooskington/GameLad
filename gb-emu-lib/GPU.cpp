#include "pch.hpp"
#include "GPU.hpp"

#define TINT 0

/*
    FF40 - LCDC - LCD Control (R/W)
    Bit 7 - LCD Display Enable             (0=Off, 1=On)
    Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
    Bit 5 - Window Display Enable          (0=Off, 1=On)
    Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
    Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
    Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
    Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
    Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)
*/
#define IsLCDDisplayEnabled ISBITSET(m_LCDControl, 7)
#define WindowTileMapDisplaySelect ISBITSET(m_LCDControl, 6)
#define WindowDisplayEnable ISBITSET(m_LCDControl, 5)
#define BGWindowTileDataSelect ISBITSET(m_LCDControl, 4)
#define BGTileMapDisplaySelect ISBITSET(m_LCDControl, 3)
#define OBJSize ISBITSET(m_LCDControl, 2)
#define OBJDisplayEnable ISBITSET(m_LCDControl, 1)
#define BGDisplayEnable ISBITSET(m_LCDControl, 0)

/*
    FF41 - STAT - LCDC Status (R/W)
    Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
    Bit 5 - Mode 2 OAM Interrupt         (1=Enable) (Read/Write)
    Bit 4 - Mode 1 V-Blank Interrupt     (1=Enable) (Read/Write)
    Bit 3 - Mode 0 H-Blank Interrupt     (1=Enable) (Read/Write)
    Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
    Bit 1-0 - Mode Flag       (Mode 0-3, see below) (Read Only)
        0: During H-Blank
        1: During V-Blank
        2: During Searching OAM-RAM
        3: During Transfering Data to LCD Driver
*/
#define LYCoincidenceInterrupt ISBITSET(m_LCDControllerStatus , 6)
#define OAMInterrupt ISBITSET(m_LCDControllerStatus , 5)
#define VBlankInterrupt ISBITSET(m_LCDControllerStatus , 4)
#define HBlankInterrupt ISBITSET(m_LCDControllerStatus , 3)
#define SETMODE(mode) m_LCDControllerStatus = ((m_LCDControllerStatus & ~0x03) | mode)
#define GETMODE (m_LCDControllerStatus & 0x03)

const byte GBColors[]
{
    0xEB, 0xC4, 0x60, 0x00
};

GPU::GPU(IMMU* pMMU, ICPU* pCPU) :
    m_MMU(pMMU),
    m_CPU(pCPU),
    m_ModeClock(VBlankCycles),
    m_DMAClocksRemaining(0),
    m_pVSyncCallback(nullptr),
    m_LCDControl(0x00),
    m_ScrollY(0x00),
    m_ScrollX(0x00),
    m_LCDControllerYCoordinate(153),
    m_LYCompare(0x00),
    m_WindowYPosition(0x00),
    m_WindowXPositionMinus7(0x00),
    m_BGPaletteData(0x00),
    m_ObjectPalette0Data(0x00),
    m_ObjectPalette1Data(0x00)
{
    SETMODE(ModeVBlank);
    memset(m_DisplayPixels, 0x00, ARRAYSIZE(m_DisplayPixels));
}

GPU::~GPU()
{
}

/*
    This method is called after the CPU executes an operation.  It tallies the number of cycles spent
    and ensures that the GPU switches between the various states listed below.  Essentially it loops
    through states 2, 3, and 0 until all 144 lines have been drawn. Then, it moves to state 1 where it
    cycles for 4560 cycles (10 lines @ 456 cycles per line). Finally, it starts over.

    The following are typical when the display is enabled:
    Mode 2  2_____2_____2_____2_____2_____2___________________2____
    Mode 3  _33____33____33____33____33____33__________________3___
    Mode 0  ___000___000___000___000___000___000________________000
    Mode 1  ____________________________________11111111111111_____

    The Mode Flag goes through the values 0, 2, and 3 at a cycle of about 109uS. 0 is present about
    48.6uS, 2 about 19uS, and 3 about 41uS. This is interrupted every 16.6ms by the VBlank (1).
    The mode flag stays set at 1 for about 1.08 ms.

    Mode 0 is present between 201-207 clks, 2 about 77-83 clks, and 3 about 169-175 clks. A complete
    cycle through these states takes 456 clks. VBlank lasts 4560 clks. A complete screen refresh occurs
    every 70224 clks.)
*/
void GPU::Step(unsigned long cycles)
{
    if (m_DMAClocksRemaining > 0)
    {
        // TODO: I think the spec says the CPU cannot access ANYTHING except HRAM while this happens
        // Still copying data
        m_DMAClocksRemaining -= cycles;
    }

    // If the LCD screen is off, exit
    if (!IsLCDDisplayEnabled)
    {
        return;
    }

    m_ModeClock += cycles;

    switch (GETMODE)
    {
    case ModeReadingOAM:
        // OAM Read mode. Scanline active.
        if (m_ModeClock >= ReadingOAMCycles)
        {
            m_ModeClock -= ReadingOAMCycles;
            SETMODE(ModeReadingOAMVRAM);
        }
        break;
    case ModeReadingOAMVRAM:
        // VRAM Read mode. Scanline active.
        if (m_ModeClock >= ReadingOAMVRAMCycles)
        {
            m_ModeClock -= ReadingOAMVRAMCycles;

            // Write a scanline to the framebuffer
            RenderScanline();

            // Go to HBlank
            SETMODE(ModeHBlank);
            if (HBlankInterrupt && (m_CPU != nullptr))
            {
                m_CPU->TriggerInterrupt(INT48);
            }
        }
        break;
    case ModeHBlank:
        // End of scan line.
        if (m_ModeClock >= HBlankCycles)
        {
            m_ModeClock -= HBlankCycles;

            // After the last HBlank, push the framebuffer to the window
            m_LCDControllerYCoordinate++;
            if (m_LCDControllerYCoordinate == 144)
            {
                // Enter VBlank and render framebuffer
                SETMODE(ModeVBlank);
                RenderImage();

                if (m_CPU != nullptr)
                {
                    m_CPU->TriggerInterrupt(INT40);
                    if (VBlankInterrupt)
                    {
                        m_CPU->TriggerInterrupt(INT48);
                    }
                }
            }
            else
            {
                // Move onto next line
                SETMODE(ModeReadingOAM);
                if (OAMInterrupt && (m_CPU != nullptr))
                {
                    m_CPU->TriggerInterrupt(INT48);
                }
            }
        }
        break;
    case ModeVBlank:
        if (m_ModeClock >= VBlankCycles)
        {
            m_ModeClock -= VBlankCycles;

            // VBlank for 10 lines
            m_LCDControllerYCoordinate++;
            if (m_LCDControllerYCoordinate == 154)
            {
                // Go back to the top left
                SETMODE(ModeReadingOAM);
                m_LCDControllerYCoordinate = 0x00;
                if (OAMInterrupt && (m_CPU != nullptr))
                {
                    m_CPU->TriggerInterrupt(INT48);
                }
            }
        }
        break;
    }

    // Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
    if (m_LYCompare == m_LCDControllerYCoordinate)
    {
        m_LCDControllerStatus = SETBIT(m_LCDControllerStatus, 2);
        if (LYCoincidenceInterrupt && (m_CPU != nullptr))
        {
            m_CPU->TriggerInterrupt(INT48);
        }
    }
    else
    {
        m_LCDControllerStatus = CLEARBIT(m_LCDControllerStatus, 2);
    }
}

byte* GPU::GetCurrentFrame()
{
    return m_DisplayPixels;
}

// IMemoryUnit
byte GPU::ReadByte(const ushort& address)
{
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        // TODO: It is possible some of our graphical issues come from this
        // Zelda reads/writes from this when it shouldn't.
        return m_VRAM[address - 0x8000];
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        // TODO: It is possible some of our graphical issues come from this
        // Zelda reads/writes from this when it shouldn't.
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
        // TODO: It is possible some of our graphical issues come from this
        // Zelda reads/writes from this when it shouldn't.
        m_VRAM[address - 0x8000] = val;
        return true;
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        // TODO: It is possible some of our graphical issues come from this
        // Zelda reads/writes from this when it shouldn't.
        m_OAM[address - 0xFE00] = val;
        return true;
    }

    switch (address)
    {
    case LCDControl:
        {
            bool isOn = IsLCDDisplayEnabled;
            m_LCDControl = val;
            if (isOn && !IsLCDDisplayEnabled)
            {
                if (GETMODE != ModeVBlank)
                {
                    Logger::Log("The LCD should not be turned off while not in VBlank.");
                }

                // The display was turned off, clear the screen
                // Set color to white
                memset(m_DisplayPixels, GBColors[0], ARRAYSIZE(m_DisplayPixels));
                for (unsigned int a = 0;a < ARRAYSIZE(m_DisplayPixels);a += 4)
                {
                    m_DisplayPixels[a] = 0xFF;   // Set Alpha to 0xFF
                }

                m_LCDControllerYCoordinate = 153;
                m_ModeClock = VBlankCycles;
                SETMODE(ModeVBlank);
            }
        }
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
        m_LCDControllerYCoordinate = 0;
        return true;
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

void GPU::SetVSyncCallback(void(*pCallback)())
{
    m_pVSyncCallback = pCallback;
}

void GPU::PreBoot()
{
    m_LCDControllerYCoordinate = 0x91;
    m_ScrollY = 0x00;
    m_ScrollX = 0x00;
    m_LYCompare = 0x00;
    m_BGPaletteData = 0xFC;
    m_ObjectPalette0Data = 0xFF;
    m_ObjectPalette1Data = 0xFF;
    m_WindowYPosition = 0x00;
    m_WindowXPositionMinus7 = 0x00;

    // Initialize color to white
    memset(m_DisplayPixels, GBColors[0], ARRAYSIZE(m_DisplayPixels));
    for (unsigned int a = 0;a < ARRAYSIZE(m_DisplayPixels);a += 4)
    {
        m_DisplayPixels[a] = 0xFF;   // Set Alpha to 0xFF
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

    It takes 160 microseconds until the transfer has completed. (0.016ms or 752 clocks).
    */
    m_DMAClocksRemaining = 752;

    ushort source = (static_cast<ushort>(address) * 0x0100);
    for (byte offset = 0x00; offset <= 0x9F; offset++)
    {
        m_OAM[offset] = m_MMU->Read(source | offset);
    }
}

void GPU::RenderScanline()
{
    RenderBackgroundScanline();
    if (WindowDisplayEnable)
    {
        RenderWindowScanline();
    }


    // Copy this line from m_bgPixels (BG and Window) to m_DisplayPixels
    memcpy(
        m_DisplayPixels + (m_LCDControllerYCoordinate * 160 * 4),
        m_bgPixels + (m_LCDControllerYCoordinate * 160 * 4),
        160 * 4);

    if (OBJDisplayEnable)
    {
        RenderOBJScanline();
    }
}

void GPU::RenderImage()
{
    if (m_pVSyncCallback != nullptr)
    {
        m_pVSyncCallback();
    }
}

void GPU::RenderBackgroundScanline()
{
    if (!BGDisplayEnable)
    {
        /*
        LCDC.0 - 1) Monochrome Gameboy and SGB: BG Display
        When Bit 0 is cleared, the background becomes blank (white).
        Window and Sprites may still be displayed (if enabled in Bit 1 and/or Bit 5).

        So, we just need to render a white background and get out early
        */
        for (int x = 0;x < 160;x++)
        {
            // If BG is disabled, render a white background
            int index = ((m_LCDControllerYCoordinate * 160) + x) * 4;
            m_bgPixels[index + 3] = GBColors[0];   // R
            m_bgPixels[index + 2] = GBColors[0];   // G
            m_bgPixels[index + 1] = GBColors[0];   // B
            m_bgPixels[index + 0] = 0xFF;          // A
        }

        return;
    }

    // Load BG (and window) palette data
    const byte palette[]
    {
        GBColors[m_BGPaletteData & 0x03],
        GBColors[(m_BGPaletteData >> 2) & 0x03],
        GBColors[(m_BGPaletteData >> 4) & 0x03],
        GBColors[(m_BGPaletteData >> 6) & 0x03],
    };

    // If bit 3 is NOT set: BG Tile Numbers at 0x9800
    // if bit 3 IS     set: BG Tile Numbers at 0x9C00
    //     Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
    ushort tileNumberMap = BGTileMapDisplaySelect ? 0x9C00 : 0x9800;
    tileNumberMap -= 0x8000; // Map for direct VRAM access

    // If bit 4 is     set: BG Tile Data at 0x8000
    // If bit 4 is NOT set: BG Tile Data at 0x8800
    //      Note: Tile data #0 is actually 0x9000, and the tile number is a SIGNED byte where
    //            0x80 (-128) is the lowest tile and is at 0x8800
    // Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
    ushort tileData = BGWindowTileDataSelect ? 0x8000 : 0x9000;
    tileData -= 0x8000; // Map for direct VRAM access

    // This is confusing, but we need to figure out WHICH tile in the 32x32 tile map to render
    // tileY is the tile # we will later lookup in the Tile Data. We take the current line #, add
    // the scroll value, then divide by 8 (since there are 8 lines per map).  Finally we MOD that
    // by 32, since the BG map wraps and is 32 rows high.
    byte tileY = (byte)(((m_LCDControllerYCoordinate + m_ScrollY) / 8) % 32);

    // This is the "offset" into the current tile we need to render. Basically it'll be the 
    // current line plus the scroll and MOD by 8 (the # of pixels per tile).
    byte tileYOffset = (byte)((m_LCDControllerYCoordinate + m_ScrollY) % 8);

    // Now loop through each pixel on this line
    for (byte x = 0; x < 160; x++)
    {
        // We need to determine the current X tile (in the same way we did the Y tile)
        byte tileX = (byte)(((m_ScrollX + x) / 8) % 32);

        // Finally, we can read the correct tile number from the tile map (32x32)
        byte tileNumber = m_VRAM[(ushort)(tileNumberMap + (tileY * 32) + tileX)];

        // Now we need to get a pointer to the tile data
        ushort tileDataPtr = 0;
        if (BGWindowTileDataSelect)
        {
            // Tile number is unsigned and each tile is 16 bytes
            tileDataPtr = (ushort)(tileData + tileNumber * 0x10);
        }
        else
        {
            // Tile number is "signed" and each tile is 16 bytes
            tileDataPtr = (ushort)(tileData + static_cast<sbyte>(tileNumber) * 0x10);
    }

        // Each line is 2 bytes long, so we need to offset for the current line
        tileDataPtr += (ushort)(tileYOffset * 2);

        // Read the two bytes!
        byte b1 = m_VRAM[tileDataPtr];
        byte b2 = m_VRAM[(ushort)(tileDataPtr + 1)];

        // Figure out which palette # it uses
        byte bit = (byte)(7 - ((m_ScrollX + x) % 8));
        byte pLo = ISBITSET(b1, bit) ? 0x01 : 0x00;
        byte pHi = ISBITSET(b2, bit) ? 0x02 : 0x00;

        // Get the color
        byte color = palette[pLo + pHi];

        // Find and set the index of the image
        int index = ((m_LCDControllerYCoordinate * 160) + x) * 4;
        m_bgPixels[index + 3] = color; // R
#if TINT
        if (m_bgPixels[index + 3] == 0x00) m_bgPixels[index + 3] = 0x30;
        m_bgPixels[index + 2] = 0x00; // G
        m_bgPixels[index + 1] = 0x00; // B
#else
        m_bgPixels[index + 2] = color; // G
        m_bgPixels[index + 1] = color; // B
#endif
        m_bgPixels[index + 0] = 0xFF;  // A
}
}

void GPU::RenderWindowScanline()
{
    // Get the "relative" window position, if it is less than 0, we are off screen, exit
    int winY = m_LCDControllerYCoordinate - m_WindowYPosition;
    if (winY < 0)
        return;

    const byte palette[]
    {
        GBColors[m_BGPaletteData & 0x03],
        GBColors[(m_BGPaletteData >> 2) & 0x03],
        GBColors[(m_BGPaletteData >> 4) & 0x03],
        GBColors[(m_BGPaletteData >> 6) & 0x03],
    };

    // If bit 6 is NOT set: BG Tile Numbers at 0x9800
    // if bit 6 IS     set: BG Tile Numbers at 0x9C00
    //     Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
    ushort tileNumberMap = WindowTileMapDisplaySelect ? 0x9C00 : 0x9800;
    tileNumberMap -= 0x8000;    // Mapped for direct VRAM access

    // If bit 4 is     set: BG Tile Data at 0x8000
    // If bit 4 is NOT set: BG Tile Data at 0x8800
    //      Note: Tile data #0 is actually 0x9000, and the tile number is a SIGNED byte where
    //            0x80 (-128) is the lowest tile and is at 0x8800
    // Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
    ushort tileData = BGWindowTileDataSelect ? 0x8000 : 0x9000;
    tileData -= 0x8000;    // Mapped for direct VRAM access

    // The Window is also 32x32 tiles, and the tile are 8 pixels tall, so figure out which tile we
    // need by dividing by 8.  Also get the offset by getting the remainder
    byte tileY = (byte)(winY / 8);
    byte tileYOffset = (byte)(winY % 8);

    // Get the relative window position
    int winX = m_WindowXPositionMinus7 - 7;
    for (int x = 0; x < 160; x++)
    {
        // If the window is to the right of here, don't do anything
        if (x < winX)
            continue;

        // Get the X tile for this pixel
        byte tileX = (byte)((x - winX) / 8);

        // Calculate the tile number
        byte tileNumber = m_VRAM[(ushort)(tileNumberMap + (tileY * 32) + tileX)];

        // Find the tile data
        ushort tileDataPtr = 0;
        if (BGWindowTileDataSelect)
        {
            tileDataPtr = (ushort)(tileData + tileNumber * 0x10);
    }
        else
        {
            tileDataPtr = (ushort)(tileData + static_cast<sbyte>(tileNumber) * 0x10);
        }

        tileDataPtr += (ushort)(tileYOffset * 2);

        // Read tile data
        byte b1 = m_VRAM[tileDataPtr];
        byte b2 = m_VRAM[(ushort)(tileDataPtr + 1)];

        // Find the pixel we care about and look up palette and color
        byte bit = (byte)(7 - x % 8);
        byte pLo = ISBITSET(b1, bit) ? 0x01 : 0x00;
        byte pHi = ISBITSET(b2, bit) ? 0x02 : 0x00;
        byte color = palette[pLo + pHi];

        // Set the image color
        int index = ((m_LCDControllerYCoordinate * 160) + x) * 4;
        m_bgPixels[index + 1] = color; // B
#if TINT
        if (m_bgPixels[index + 1] == 0x00) m_bgPixels[index + 1] = 0x30;
        m_bgPixels[index + 2] = 0x00; // G
        m_bgPixels[index + 3] = 0x00; // R
#else
        m_bgPixels[index + 2] = color; // G
        m_bgPixels[index + 3] = color; // R
#endif
        m_bgPixels[index + 0] = 0xFF;  // A
}
}

void GPU::RenderOBJScanline()
{
    const byte SPRITESIZEINBYTES = 16;
    const byte bgPalette[]
    {
        GBColors[m_BGPaletteData & 0x03],
        GBColors[(m_BGPaletteData >> 2) & 0x03],
        GBColors[(m_BGPaletteData >> 4) & 0x03],
        GBColors[(m_BGPaletteData >> 6) & 0x03],
    };

    // Loop through each sprite (backwards)
    for (int i = 156; i >= 0; i -= 4)
    {
        // Grab the sprite data
        byte objY = m_OAM[i];                // The sprite Y position, minus 16 (apparently)
        byte spriteSize = OBJSize ? 0x10 : 0x08; // 0x00 = 8x8, 0x01 = 8x16
        int height = spriteSize;

        // Sprite rect:
        // x = spriteX - 8
        // y = spriteY - 16
        // w = 8
        // h = spriteSize

        int y = objY - 16;

        // Check if the sprite is on the current scanline
        if ((y <= m_LCDControllerYCoordinate) && ((y + height) > m_LCDControllerYCoordinate))
        {
            byte objX = m_OAM[i + 1];               // The sprite X position, minus 8 (apparently)
            byte spriteTileNumber = m_OAM[i + 2];   // The tile or pattern number of the sprite
            byte spriteFlags = m_OAM[i + 3];        // The sprites render flags (priority, flip, palette)

            if (spriteSize == 0x10)
            {
                spriteTileNumber &= 0xFE;
            }

            byte paletteNumber = ISBITSET(spriteFlags, 4) ? 0x01 : 0x00;

            int x = objX - 8;

            // Mapped for direct VRAM access
            const ushort tileData = 0x0000;

            // Create the palette to use for this sprite
            byte palette[]
            {
                0x00, // Unused - Transparent
                GBColors[(paletteNumber == 0x00) ? (m_ObjectPalette0Data >> 2 & 0x03) : (m_ObjectPalette1Data >> 2 & 0x03)],
                GBColors[(paletteNumber == 0x00) ? (m_ObjectPalette0Data >> 4 & 0x03) : (m_ObjectPalette1Data >> 4 & 0x03)],
                GBColors[(paletteNumber == 0x00) ? (m_ObjectPalette0Data >> 6 & 0x03) : (m_ObjectPalette1Data >> 6 & 0x03)]
            };

            // The memory location of this sprites tile can be found by adding the sprites tile
            // number to the location of the tile data.
            // If the spriteSize == 0x00, ignore the lower bit of the tile number.
            ushort tilePointer = tileData + (spriteTileNumber * SPRITESIZEINBYTES);
            byte tileYOffset = ISBITSET(spriteFlags, 6) ? ((height - 1) - (m_LCDControllerYCoordinate - y)) : (m_LCDControllerYCoordinate - y);
            tilePointer += (tileYOffset * 2);

            // The data for this line of the sprite, 8 pixels
            byte low = m_VRAM[tilePointer];
            byte high = m_VRAM[(ushort)(tilePointer + 1)];

            // Loop through all 8 pixels of this line
            for (int indexX = 0; indexX < 8; indexX++)
            {
                int pixelX = x + indexX;
                // Check if the pixel is still on screen
                if (pixelX >= 0 && pixelX < 160)
                {
                    byte bit = ISBITSET(spriteFlags, 5) ? indexX : 7 - indexX;
                    byte pixelVal = 0x00;
                    if (ISBITSET(high, bit)) pixelVal |= 0x02;
                    if (ISBITSET(low, bit)) pixelVal |= 0x01;
                    byte color = palette[pixelVal];

                    // If two sprites x coordinates are the same on DMG OR CGB, the one with the lower address in OAM will be 'on top'
                    // If two sprites x coordinates are different on DMG, the one with the x coordinate closer to the ? right ? of the screen will be on top, regardless of position in OAM. (When in DMG mode(i.e.when playing a non - color enhanced game), the CGB emulates this behavior)
                    // If two sprites x coordinates are different on CGB in CGB mode, the one with the lower address in OAM will be 'on top', regardless of x coordinate.

                    // If the pixel is not transparent
                    if (pixelVal != 0x00)
                    {
                        int index = ((m_LCDControllerYCoordinate * 160) + pixelX) * 4;

                        // If the sprite has priority 0 (Render above BG)
                        if (!ISBITSET(spriteFlags, 7) || (m_bgPixels[index + 3] == bgPalette[0x00]))
                        {
                            // If the BG pixel is white
                            //  This sprite has priority 1 (Render behind BG)
                            //  The sprite pixels only get rendered above BG pixels that are white.
                            //  All other BG pixels stay on top.
                            // Render that sprite pixel
                            m_DisplayPixels[index + 2] = color; // G
#if TINT
                            if (m_DisplayPixels[index + 2] == 0x00) m_DisplayPixels[index + 2] = 0x30;
                            m_DisplayPixels[index + 3] = 0x00; // R
                            m_DisplayPixels[index + 1] = 0x00; // B
#else
                            m_DisplayPixels[index + 3] = color; // R
                            m_DisplayPixels[index + 1] = color; // B
#endif
                            m_DisplayPixels[index + 0] = 0xFF;  // A
                        }
                    }
                }
            }
        }
    }
}
