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

/*
    CGB colours are 5 bits per channel. The emulator hands the raw RGB555 value
    to test harnesses through the native frame; for the on-screen RGBA buffer it
    uses the plain 5->8 expansion, which is what the reference screenshots for
    cgb-acid2 / AGE use and what most host displays expect.
*/
static inline byte Expand5To8(byte value)
{
    return static_cast<byte>((value << 3) | (value >> 2));
}

GPU::GPU(IMMU* pMMU, ICPU* pCPU) :
    m_MMU(pMMU),
    m_CPU(pCPU),
    m_mode(GameBoyMode::DMG),
    m_VRAMBank(0x00),
    m_ModeClock(0),
    m_Mode3Cycles(ReadingOAMVRAMCycles),
    m_Mode0Cycles(HBlankCycles),
    m_StatInterruptLine(false),
    m_InternalMode2STATEventFired(false),
    m_Line153LYReset(false),
    m_FirstLineAfterLCDEnable(false),
    m_WindowLineCounter(0),
    m_WindowYTriggered(false),
    m_lineSpriteCount(0),
    m_RenderX(0),
    m_PixelStartupCycles(0),
    m_PixelStallCycles(0),
    m_PixelFetcherAlignmentCycles(0),
    m_BGPixelFIFOHead(0),
    m_BGPixelFIFOSize(0),
    m_BGFetcherStage(0),
    m_BGFetcherTileNumber(0),
    m_BGFetcherTileAttributes(0),
    m_BGFetcherTileLow(0),
    m_BGFetcherTileHigh(0),
    m_BGFetcherTileRow(0),
    m_BGFetcherAddress(0),
    m_BGFetcherPixelX(0),
    m_BGFetcherFineDiscard(0),
    m_BGFetcherWindow(false),
    m_BGFetcherUnsignedTiles(false),
    m_PixelStallPausesFetcher(false),
    m_OBJEnabledAtTransferStart(false),
    m_WindowTriggeredThisLine(false),
    m_WindowTriggerMissedThisLine(false),
    m_WindowStartedThisLine(false),
    m_WindowEligibleThisLine(false),
    m_WindowPenaltyScheduled(false),
    m_WindowStartX(0),
    m_DMAActive(false),
    m_DMASourceAddress(0x0000),
    m_DMAOffset(0x00),
    m_DMASnoopByte(0xFF),
    m_DMACyclesAccumulated(0),
    m_DMARegister(0x00),
    m_DMAPending(false),
    m_DMAPendingSource(0x0000),
    m_DMAPendingCyclesRemaining(0),
    m_pVSyncCallback(nullptr),
    m_LCDControl(0x00),
    m_LCDControllerStatus(0x00),
    m_ScrollY(0x00),
    m_ScrollX(0x00),
    m_LCDControllerYCoordinate(0),
    m_LYCompare(0x00),
    m_WindowYPosition(0x00),
    m_WindowXPositionMinus7(0x00),
    m_BGPaletteData(0x00),
    m_ObjectPalette0Data(0x00),
    m_ObjectPalette1Data(0x00),
    m_BGPaletteIndex(0x00),
    m_OBJPaletteIndex(0x00),
    m_HDMASource(0x0000),
    m_HDMADestination(0x8000),
    m_HDMABlocksRemaining(0x00),
    m_HDMAActive(false),
    m_HDMABlockTransferred(false),
    m_GDMATransferCount(0),
    m_HDMATerminationWindow(false),
    m_DMAStallCycles(0)
{
    // The LCD starts disabled. Real hardware reports mode 0 and LY=0 while
    // the display is off.
    SETMODE(ModeHBlank);
    memset(m_lineSpriteOAMIndex, 0x00, ARRAYSIZE(m_lineSpriteOAMIndex));
    memset(m_lineSpriteFetched, 0x00, ARRAYSIZE(m_lineSpriteFetched));
    memset(m_BGPixelFIFO, 0x00, ARRAYSIZE(m_BGPixelFIFO));
    memset(m_BGPixelFIFOAttributes, 0x00, ARRAYSIZE(m_BGPixelFIFOAttributes));
    memset(m_bgColorIndexLine, 0x00, ARRAYSIZE(m_bgColorIndexLine));
    memset(m_VRAM, 0x00, sizeof(m_VRAM));
    memset(m_OAM, 0x00, sizeof(m_OAM));
    memset(m_DisplayPixels, 0x00, ARRAYSIZE(m_DisplayPixels));
    memset(m_NativePixels, 0x00, sizeof(m_NativePixels));
    memset(m_LineColorIndex, 0x00, sizeof(m_LineColorIndex));
    memset(m_LineBGPriority, 0x00, sizeof(m_LineBGPriority));

    // The CGB boot ROM leaves palette RAM filled with white, which is also what
    // a game sees before it programs its own palettes.
    memset(m_BGPaletteRAM, 0xFF, sizeof(m_BGPaletteRAM));
    memset(m_OBJPaletteRAM, 0xFF, sizeof(m_OBJPaletteRAM));
}

GPU::~GPU()
{
}

void GPU::SetGameBoyMode(GameBoyMode mode)
{
    m_mode = mode;

    if (!IsCGBFeatureMode(mode))
    {
        // Lock the CGB-only state into its inert configuration so a DMG (or a
        // CGB running a monochrome cartridge) can never observe it.
        m_VRAMBank = 0x00;
        m_HDMAActive = false;
        m_HDMABlocksRemaining = 0x00;
        m_GDMATransferCount = 0;
        m_HDMATerminationWindow = false;
        m_DMAStallCycles = 0;
    }
}

// CGB colour rendering is only active for a CGB-aware cartridge. A CGB running
// a monochrome cartridge renders through the DMG palette registers.
bool GPU::IsCGBRendering() const
{
    return IsCGBFeatureMode(m_mode);
}

unsigned long GPU::ConsumeDMAStallCycles()
{
    const unsigned long stall = m_DMAStallCycles;
    m_DMAStallCycles = 0;
    return stall;
}

unsigned long GPU::ConsumeGDMATransferCount()
{
    const unsigned long count = m_GDMATransferCount;
    m_GDMATransferCount = 0;
    return count;
}

ushort GPU::ReadCGBPalette(const byte* paletteRAM, int palette, int colorIndex) const
{
    const int offset = ((palette & 0x07) * 8) + ((colorIndex & 0x03) * 2);
    return static_cast<ushort>(paletteRAM[offset] | (paletteRAM[offset + 1] << 8));
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
    every 70224 clks.

    Mode 3's length (and the corresponding HBlank shrink) is not fixed on real hardware: it grows with
    the SCX fine-scroll discard, the number of sprites fetched on the line, and the window's fetch
    startup penalty. ComputeScanlineTiming approximates that per-line, keeping the 456 cycle total.
*/
void GPU::Step(unsigned long cycles)
{
    Step(cycles, cycles);
}

void GPU::Step(unsigned long baseCycles, unsigned long cpuCycles)
{
    /*
        OAM DMA moves one byte per CPU M-cycle. In CGB double speed that is two
        base-clock dots, so it must use the CPU-domain count while the PPU state
        machine continues to use the base-domain count.
    */
    StepOAMDMA(cpuCycles);

    // If the LCD screen is off, the whole mode state machine is frozen.
    // LY reads 0 and STAT reports mode 0 while disabled (PPU-001).
    if (!IsLCDDisplayEnabled)
    {
        return;
    }

    m_ModeClock += baseCycles;

    switch (GETMODE)
    {
    case ModeReadingOAM:
        // OAM Read mode. Scanline active.
        if (m_ModeClock >= ReadingOAMCycles)
        {
            m_ModeClock -= ReadingOAMCycles;
            EnterMode(ModeReadingOAMVRAM);
        }
        break;
    case ModeReadingOAMVRAM:
        // VRAM Read mode. Scanline active.
        AdvancePixelTransfer(baseCycles);
        if (m_ModeClock >= m_Mode3Cycles)
        {
            m_ModeClock -= m_Mode3Cycles;

            FinishPixelTransfer();

            // Go to HBlank
            EnterMode(ModeHBlank);

            // An HBlank-driven VRAM DMA copies exactly one 16-byte block per
            // HBlank period, starting with the HBlank that follows the write to
            // HDMA5.
            if (m_HDMAActive)
            {
                PerformHDMABlock();
            }

        }
        break;
    case ModeHBlank:
        if (!m_InternalMode2STATEventFired &&
            (m_LCDControllerYCoordinate < 143) &&
            (m_ModeClock >= (m_Mode0Cycles - 4)))
        {
            QueueInternalMode2STAT(
                static_cast<byte>(m_LCDControllerYCoordinate + 1));
        }

        // End of scan line.
        if (m_ModeClock >= m_Mode0Cycles)
        {
            m_ModeClock -= m_Mode0Cycles;

            m_FirstLineAfterLCDEnable = false;

            // After the last HBlank, push the framebuffer to the window
            m_LCDControllerYCoordinate++;
            if (m_LCDControllerYCoordinate == 144)
            {
                // Enter VBlank and render framebuffer
                EnterMode(ModeVBlank);
                RenderImage();

                if (m_CPU != nullptr)
                {
                    m_CPU->TriggerInterrupt(INT40);
                }
            }
            else
            {
                // The OAM scan and scanline timing for the next line are
                // computed before mode 2 begins.
                ScanSpritesForLine(m_LCDControllerYCoordinate);
                ComputeScanlineTiming();
                EnterMode(ModeReadingOAM);
            }
        }
        break;
    case ModeVBlank:
        // LY changes from 153 to 0 at dot 4 of the final VBlank line. With
        // four-dot CPU scheduling, that edge is visible on the next quantum.
        if (!m_Line153LYReset &&
            (m_LCDControllerYCoordinate == 153) &&
            (m_ModeClock >= 8))
        {
            m_LCDControllerYCoordinate = 0;
            m_Line153LYReset = true;
            UpdateCoincidenceFlag();
            UpdateStatInterruptLine();
        }

        if (m_ModeClock >= VBlankCycles)
        {
            m_ModeClock -= VBlankCycles;

            // VBlank for 10 lines
            if (m_Line153LYReset)
            {
                // Go back to the top left; start a new frame.
                m_LCDControllerYCoordinate = 0x00;

                // The window's internal line counter restarts with the frame,
                // not with LY, which is what lets a game hide the window for a
                // few lines and have it resume where it left off.
                m_WindowLineCounter = 0x00;
                m_WindowYTriggered = false;
                m_Line153LYReset = false;
                ScanSpritesForLine(m_LCDControllerYCoordinate);
                ComputeScanlineTiming();
                EnterMode(ModeReadingOAM);
            }
            else
            {
                m_LCDControllerYCoordinate++;
                UpdateCoincidenceFlag();
                UpdateStatInterruptLine();
            }
        }
        break;
    }
}

// Advances an in-progress OAM DMA transfer one byte per M-cycle.
void GPU::StepOAMDMA(unsigned long cycles)
{
    if (m_DMAPending)
    {
        if (cycles >= m_DMAPendingCyclesRemaining)
        {
            const unsigned long leftover = cycles - m_DMAPendingCyclesRemaining;
            m_DMAPendingCyclesRemaining = 0;
            m_DMAPending = false;
            m_DMASourceAddress = m_DMAPendingSource;
            m_DMAOffset = 0x00;
            m_DMACyclesAccumulated = 0;
            m_DMAActive = true;
            cycles = leftover;
        }
        else
        {
            m_DMAPendingCyclesRemaining -= cycles;
            cycles = 0;
        }
    }

    if (!m_DMAActive)
    {
        return;
    }

    m_DMACyclesAccumulated += cycles;
    while (m_DMAActive && (m_DMACyclesAccumulated >= 4))
    {
        m_DMASnoopByte =
            ReadOAMDMASourceByte(static_cast<ushort>(m_DMASourceAddress + m_DMAOffset));
        m_OAM[m_DMAOffset] = m_DMASnoopByte;
        m_DMAOffset++;
        m_DMACyclesAccumulated -= 4;

        if (m_DMAOffset >= OAMDMABytes)
        {
            m_DMAActive = false;
            m_DMACyclesAccumulated = 0;
        }
    }
}

bool GPU::IsOAMDMAActive() const
{
    return m_DMAActive;
}

int GPU::GetOAMBugRow(OAMBugOrigin origin) const
{
    if (m_mode != GameBoyMode::DMG ||
        !IsLCDDisplayEnabled ||
        m_DMAActive ||
        m_FirstLineAfterLCDEnable)
    {
        return -1;
    }

    const unsigned long memoryBusDelay =
        origin == OAMBugOrigin::MemoryBus ? 4 : 0;
    const byte mode = GETMODE;

    if (mode == ModeReadingOAM)
    {
        // A memory access is observed after its M-cycle has advanced the PPU.
        // If that cycle crossed the line boundary, it still hit the first row.
        if (m_ModeClock < memoryBusDelay)
        {
            return 1;
        }

        const unsigned long scanClock = m_ModeClock - memoryBusDelay;
        if (scanClock < 76)
        {
            return static_cast<int>(scanClock / 4) + 1;
        }
    }

    return -1;
}

ushort GPU::ReadOAMWord(int row, int word) const
{
    const int offset = row * 8 + word * 2;
    return static_cast<ushort>(
        m_OAM[offset] |
        (static_cast<ushort>(m_OAM[offset + 1]) << 8));
}

void GPU::WriteOAMWord(int row, int word, ushort value)
{
    const int offset = row * 8 + word * 2;
    m_OAM[offset] = static_cast<byte>(value & 0xFF);
    m_OAM[offset + 1] = static_cast<byte>(value >> 8);
}

void GPU::CopyOAMRow(int sourceRow, int destinationRow)
{
    memcpy(
        &m_OAM[destinationRow * 8],
        &m_OAM[sourceRow * 8],
        8);
}

void GPU::CorruptOAMWrite(int row)
{
    if (row < 1 || row > 19)
    {
        return;
    }

    const ushort current = ReadOAMWord(row, 0);
    const ushort previous = ReadOAMWord(row - 1, 0);
    const ushort previousMiddle = ReadOAMWord(row - 1, 2);
    const ushort corrupted = static_cast<ushort>(
        ((current ^ previousMiddle) & (previous ^ previousMiddle)) ^
        previousMiddle);
    WriteOAMWord(row, 0, corrupted);
    memcpy(&m_OAM[row * 8 + 2], &m_OAM[(row - 1) * 8 + 2], 6);
}

void GPU::CorruptOAMRead(int row)
{
    if (row < 1 || row > 19)
    {
        return;
    }

    if ((row % 4) == 2)
    {
        if (row < 19)
        {
            const ushort a = ReadOAMWord(row - 2, 0);
            const ushort b = ReadOAMWord(row - 1, 0);
            const ushort c = ReadOAMWord(row, 0);
            const ushort d = ReadOAMWord(row - 1, 2);
            WriteOAMWord(
                row - 1,
                0,
                static_cast<ushort>((b & (a | c | d)) | (a & c & d)));
            CopyOAMRow(row - 1, row - 2);
        }
    }
    else if ((row % 4) == 0)
    {
        if (row < 19)
        {
            ushort corrupted;
            if (row == 8)
            {
                const ushort b = ReadOAMWord(row, 0);
                const ushort c = ReadOAMWord(row - 1, 2);
                const ushort d = ReadOAMWord(row - 1, 1);
                const ushort e = ReadOAMWord(row - 1, 0);
                const ushort f = ReadOAMWord(row - 2, 1);
                const ushort g = ReadOAMWord(row - 2, 0);
                const ushort h = ReadOAMWord(row - 4, 0);
                corrupted = static_cast<ushort>(
                    (e & (h | g | (static_cast<ushort>(~d) & f) | c | b)) |
                    (c & g & h));
            }
            else
            {
                const ushort a = ReadOAMWord(row, 0);
                const ushort b = ReadOAMWord(row - 1, 2);
                const ushort c = ReadOAMWord(row - 1, 0);
                const ushort d = ReadOAMWord(row - 2, 0);
                const ushort e = ReadOAMWord(row - 4, 0);

                if (row == 4)
                {
                    corrupted = static_cast<ushort>(
                        (c & (a | b | d | e)) |
                        (a & b & d & e));
                }
                else if (row == 12)
                {
                    corrupted = static_cast<ushort>(
                        (c & (a | b | d | e)) |
                        (b & d & e));
                }
                else
                {
                    corrupted = static_cast<ushort>(
                        c | (a & b & d & e));
                }
            }

            WriteOAMWord(row - 1, 0, corrupted);
            CopyOAMRow(row - 1, row - 2);
            CopyOAMRow(row - 1, row - 4);
        }
    }
    else
    {
        const ushort current = ReadOAMWord(row, 0);
        const ushort previous = ReadOAMWord(row - 1, 0);
        const ushort previousMiddle = ReadOAMWord(row - 1, 2);
        const ushort corrupted = static_cast<ushort>(
            previous | (current & previousMiddle));
        WriteOAMWord(row - 1, 0, corrupted);
        WriteOAMWord(row, 0, corrupted);
    }

    CopyOAMRow(row - 1, row);
    if (row == 16)
    {
        CopyOAMRow(16, 0);
    }
}

void GPU::TriggerOAMBug(
    ushort address,
    OAMBugAccess access,
    OAMBugOrigin origin)
{
    if (address < 0xFE00 || address > 0xFEFF)
    {
        return;
    }

    const int row = GetOAMBugRow(origin);
    if (row < 0)
    {
        return;
    }

    if (access == OAMBugAccess::Read)
    {
        CorruptOAMRead(row);
    }
    else
    {
        CorruptOAMWrite(row);
    }
}

byte GPU::ReadOAMDMASourceByte(ushort address) const
{
    if (IsCGBHardware(m_mode) && (address >= 0xFE00))
    {
        return 0xFF;
    }

    return m_MMU->Read(address);
}

GPU::OAMDMABus GPU::BusForAddress(ushort address, bool isSource) const
{
    const bool cgb = IsCGBHardware(m_mode);

    if ((address >= 0x8000) && (address < 0xA000))
    {
        return OAMDMABus::Video;
    }

    if (address < 0xC000)
    {
        return OAMDMABus::External;
    }

    if (address < 0xFE00)
    {
        return cgb ? OAMDMABus::Work : OAMDMABus::External;
    }

    if (!isSource)
    {
        return OAMDMABus::None;
    }

    return OAMDMABus::External;
}

bool GPU::IsCPUAddressBlockedByOAMDMA(ushort address) const
{
    if (!m_DMAActive ||
        IsHRAMOrIEAddress(address) ||
        (address == DMATransferAndStartAddress))
    {
        return false;
    }

    if ((address >= 0xFE00) && (address <= 0xFEFF))
    {
        return true;
    }

    const OAMDMABus sourceBus = BusForAddress(m_DMASourceAddress, true);
    const OAMDMABus addressBus = BusForAddress(address, false);
    return (addressBus != OAMDMABus::None) && (sourceBus == addressBus);
}

bool GPU::IsLine153STATPhase() const
{
    return (GETMODE == ModeVBlank) &&
        ((m_LCDControllerYCoordinate >= 152) || m_Line153LYReset);
}

void GPU::EnterMode(byte mode)
{
    if (mode == ModeHBlank || mode == ModeVBlank)
    {
        m_InternalMode2STATEventFired = false;
    }

    SETMODE(mode);
    UpdateCoincidenceFlag();
    UpdateStatInterruptLine();

    if (mode == ModeReadingOAMVRAM)
    {
        BeginPixelTransfer();
    }
}

void GPU::QueueInternalMode2STAT(byte nextLY)
{
    m_InternalMode2STATEventFired = true;

    if (!OAMInterrupt || HBlankInterrupt || !LYCoincidenceInterrupt)
    {
        return;
    }

    if ((nextLY == 0) && VBlankInterrupt)
    {
        return;
    }

    bool overlapsCoincidence = m_LYCompare == 0
        ? nextLY <= 1
        : nextLY == static_cast<byte>(m_LYCompare + 1);
    if (overlapsCoincidence)
    {
        return;
    }

    m_StatInterruptLine = true;
    if (m_CPU != nullptr)
    {
        m_CPU->QueueInterrupt(INT48);
    }
}

void GPU::UpdateCoincidenceFlag()
{
    if (m_LYCompare == m_LCDControllerYCoordinate)
    {
        m_LCDControllerStatus = SETBIT(m_LCDControllerStatus, 2);
    }
    else
    {
        m_LCDControllerStatus = CLEARBIT(m_LCDControllerStatus, 2);
    }
}

// The four STAT interrupt sources (LYC=LY, mode 2, mode 1, mode 0) are
// logically ORed into a single line; IF is only requested on that line's
// rising edge, not on every instruction where a source happens to be true.
void GPU::UpdateStatInterruptLine()
{
    if (!IsLCDDisplayEnabled)
    {
        m_StatInterruptLine = false;
        return;
    }

    bool coincidence = ISBITSET(m_LCDControllerStatus, 2);
    byte mode = GETMODE;

    bool line =
        (LYCoincidenceInterrupt && coincidence) ||
        (OAMInterrupt && (mode == ModeReadingOAM)) ||
        (VBlankInterrupt && (mode == ModeVBlank)) ||
        (HBlankInterrupt && (mode == ModeHBlank));

    if (line && !m_StatInterruptLine && (m_CPU != nullptr))
    {
        m_CPU->TriggerInterrupt(INT48);
    }

    m_StatInterruptLine = line;
}

// Selects up to 10 sprites intersecting scanline "ly", in OAM order, per
// the hardware's per-line sprite limit (PPU-005). Final on-screen priority
// (lowest X wins, ties broken by OAM order) is resolved at draw time.
void GPU::ScanSpritesForLine(byte ly)
{
    m_lineSpriteCount = 0;

    byte height = OBJSize ? 0x10 : 0x08;
    for (byte i = 0x00; i < 0xA0; i += 4)
    {
        int y = static_cast<int>(m_OAM[i]) - 16;
        if ((y <= static_cast<int>(ly)) && ((y + height) > static_cast<int>(ly)))
        {
            m_lineSpriteOAMIndex[m_lineSpriteCount] = i;
            m_lineSpriteCount++;

            if (m_lineSpriteCount >= MaxSpritesPerScanline)
            {
                break;
            }
        }
    }
}

// Approximates mode 3's variable length (PPU-004) without a full dot-based
// FIFO pipeline: base 172 cycles, plus the SCX fine-scroll discard, plus a
// per-sprite fetch penalty for sprites selected on this line, plus a fixed
// startup penalty the first time the window is fetched on the line. Mode 0
// shrinks by the same amount so the 456 cycle scanline total is preserved.
void GPU::ComputeScanlineTiming()
{
    unsigned long penalty = static_cast<unsigned long>(m_ScrollX % 8);

    if (OBJDisplayEnable)
    {
        penalty += static_cast<unsigned long>(m_lineSpriteCount) * 6;
    }

    if (WindowDisplayEnable &&
        (m_LCDControllerYCoordinate >= m_WindowYPosition) &&
        (m_WindowXPositionMinus7 <= 166))
    {
        penalty += (m_WindowXPositionMinus7 == 166) ? 1 : 6;
    }

    m_Mode3Cycles = ReadingOAMVRAMCycles + penalty;
    m_Mode0Cycles = CyclesPerScanline - ReadingOAMCycles - m_Mode3Cycles;
}

void GPU::BeginPixelTransfer()
{
    if (!m_WindowYTriggered &&
        (m_LCDControllerYCoordinate == m_WindowYPosition) &&
        (WindowDisplayEnable || IsCGBRendering()))
    {
        // CGB silicon latches WY even while the LCDC window-enable bit is clear;
        // a later enable on the same frame can therefore start the window.
        m_WindowYTriggered = true;
    }

    m_RenderX = 0;
    m_PixelStartupCycles = 12;
    m_PixelStallCycles = 0;
    m_PixelFetcherAlignmentCycles = 0;
    m_PixelStallPausesFetcher = false;
    m_OBJEnabledAtTransferStart = OBJDisplayEnable;
    m_WindowTriggeredThisLine = false;
    m_WindowTriggerMissedThisLine = false;
    m_WindowStartedThisLine = false;
    m_WindowEligibleThisLine = WindowDisplayEnable && m_WindowYTriggered;
    m_WindowPenaltyScheduled =
        m_WindowEligibleThisLine && (m_WindowXPositionMinus7 <= 166);
    m_WindowStartX = 0;
    memset(m_lineSpriteFetched, 0x00, ARRAYSIZE(m_lineSpriteFetched));
    ResetBackgroundFetcher(false);

    if (m_WindowEligibleThisLine && (m_WindowXPositionMinus7 < 7))
    {
        m_WindowTriggeredThisLine = true;
        m_WindowStartedThisLine = true;
        m_WindowStartX = static_cast<int>(m_WindowXPositionMinus7) - 7;
        m_PixelStartupCycles += 6;
        ResetBackgroundFetcher(true);
        m_BGFetcherFineDiscard =
            static_cast<byte>(7 - m_WindowXPositionMinus7);
        if ((m_WindowXPositionMinus7 == 0) && ((m_ScrollX & 0x07) != 0))
        {
            m_PixelStartupCycles++;
        }
    }
}

void GPU::AdvancePixelTransfer(unsigned long cycles)
{
    for (unsigned long cycle = 0; cycle < cycles && m_RenderX < 160; cycle++)
    {
        if ((m_PixelStallCycles == 0) ||
            !m_PixelStallPausesFetcher ||
            (m_PixelFetcherAlignmentCycles != 0))
        {
            AdvanceBackgroundFetcher();
            if (m_PixelFetcherAlignmentCycles != 0)
            {
                m_PixelFetcherAlignmentCycles--;
            }
        }

        if (m_PixelStartupCycles != 0)
        {
            if (m_BGFetcherWindow && (m_BGFetcherFineDiscard != 0))
            {
                byte discardedPixel = 0;
                byte discardedAttributes = 0;
                if (PopBackgroundPixel(discardedPixel, discardedAttributes))
                {
                    m_BGFetcherFineDiscard--;
                }
            }
            m_PixelStartupCycles--;
            if ((m_PixelStartupCycles == 0) &&
                (m_BGFetcherFineDiscard == 0) &&
                !WindowDisplayEnable &&
                m_WindowYTriggered &&
                (m_WindowXPositionMinus7 <= 7))
            {
                m_WindowTriggerMissedThisLine = true;
            }
            continue;
        }

        if (m_BGFetcherFineDiscard != 0)
        {
            byte discardedPixel = 0;
            byte discardedAttributes = 0;
            if (PopBackgroundPixel(discardedPixel, discardedAttributes))
            {
                m_BGFetcherFineDiscard--;
                if ((m_BGFetcherFineDiscard == 0) &&
                    !WindowDisplayEnable &&
                    m_WindowYTriggered &&
                    (m_WindowXPositionMinus7 <= 7))
                {
                    m_WindowTriggerMissedThisLine = true;
                }
            }
            continue;
        }

        if (m_PixelStallCycles != 0)
        {
            m_PixelStallCycles--;
            continue;
        }

        if (m_WindowPenaltyScheduled &&
            !m_WindowTriggeredThisLine &&
            !WindowDisplayEnable &&
            m_OBJEnabledAtTransferStart &&
            (m_lineSpriteCount != 0))
        {
            const unsigned long penalty =
                (m_WindowXPositionMinus7 == 166) ? 1 : 6;
            m_Mode3Cycles -= penalty;
            m_Mode0Cycles += penalty;
            m_WindowPenaltyScheduled = false;
        }

        if (!m_WindowTriggeredThisLine &&
            !m_WindowTriggerMissedThisLine &&
            WindowDisplayEnable &&
            m_WindowYTriggered &&
            (m_WindowXPositionMinus7 <= 166) &&
            (static_cast<int>(m_RenderX) ==
                static_cast<int>(m_WindowXPositionMinus7) - 7))
        {
            m_WindowTriggeredThisLine = true;
            m_WindowStartedThisLine = true;
            m_WindowEligibleThisLine = true;
            m_WindowStartX = static_cast<int>(m_RenderX);
            ResetBackgroundFetcher(true);
            AdvanceBackgroundFetcher();
            const unsigned long penalty =
                (m_WindowXPositionMinus7 == 166) ? 1 : 6;
            m_PixelStallCycles = penalty - 1;
            m_PixelStallPausesFetcher = false;

            if (!m_WindowPenaltyScheduled)
            {
                m_Mode3Cycles += penalty;
                m_Mode0Cycles -= penalty;
                m_WindowPenaltyScheduled = true;
            }
            continue;
        }

        unsigned long spritePenalty = 0;
        unsigned long spriteAlignment = 0;
        if (OBJDisplayEnable)
        {
            for (byte sprite = 0; sprite < m_lineSpriteCount; sprite++)
            {
                if (m_lineSpriteFetched[sprite])
                {
                    continue;
                }

                const int spriteX =
                    static_cast<int>(m_OAM[m_lineSpriteOAMIndex[sprite] + 1]) - 8;
                if (spriteX <= static_cast<int>(m_RenderX))
                {
                    m_lineSpriteFetched[sprite] = true;
                    spritePenalty += 6;
                    if ((spriteAlignment == 0) && (m_RenderX < 159))
                    {
                        const byte alignmentEnd =
                            m_lineSpriteCount == MaxSpritesPerScanline ? 3 : 5;
                        spriteAlignment = m_BGFetcherStage < alignmentEnd
                            ? static_cast<unsigned long>(
                                alignmentEnd - m_BGFetcherStage)
                            : 0;
                    }
                }
            }
        }

        if (spritePenalty != 0)
        {
            spritePenalty += spriteAlignment;
            m_PixelStallCycles = spritePenalty - 1;
            m_PixelFetcherAlignmentCycles = spriteAlignment;
            m_PixelStallPausesFetcher = true;
            if (m_OBJEnabledAtTransferStart)
            {
                m_Mode3Cycles += spriteAlignment;
                m_Mode0Cycles -= spriteAlignment;
            }
            continue;
        }

        byte bgColorIndex = 0;
        byte bgAttributes = 0;
        if (!PopBackgroundPixel(bgColorIndex, bgAttributes))
        {
            continue;
        }

        RenderPixel(m_RenderX, bgColorIndex, bgAttributes);
        m_RenderX++;
    }
}

void GPU::FinishPixelTransfer()
{
    unsigned long fetchGuard = 0;
    while (m_RenderX < 160)
    {
        byte bgColorIndex = 0;
        byte bgAttributes = 0;
        while (!PopBackgroundPixel(bgColorIndex, bgAttributes) &&
            fetchGuard < 2048)
        {
            AdvanceBackgroundFetcher();
            fetchGuard++;
        }

        RenderPixel(m_RenderX, bgColorIndex, bgAttributes);
        m_RenderX++;
    }

    if (m_WindowStartedThisLine ||
        (m_WindowEligibleThisLine && (m_WindowXPositionMinus7 <= 166)))
    {
        m_WindowLineCounter++;
    }
}

void GPU::ResetBackgroundFetcher(bool window)
{
    m_BGPixelFIFOHead = 0;
    m_BGPixelFIFOSize = 0;
    m_BGFetcherStage = 0;
    m_BGFetcherTileNumber = 0;
    m_BGFetcherTileAttributes = 0;
    m_BGFetcherTileLow = 0;
    m_BGFetcherTileHigh = 0;
    m_BGFetcherTileRow = 0;
    m_BGFetcherAddress = 0;
    m_BGFetcherPixelX = 0;
    m_BGFetcherWindow = window;
    m_BGFetcherUnsignedTiles = false;
    m_BGFetcherFineDiscard =
        window ? 0 : static_cast<byte>(m_ScrollX & 0x07);
}

void GPU::AdvanceBackgroundFetcher()
{
    if ((m_BGFetcherStage == 5) && (m_BGPixelFIFOSize != 0))
    {
        return;
    }

    if (m_BGFetcherStage == 0)
    {
        if (m_BGFetcherWindow && !WindowDisplayEnable)
        {
            if (m_BGPixelFIFOSize != 0)
            {
                return;
            }

            m_WindowTriggeredThisLine = false;
            ResetBackgroundFetcher(false);
            m_BGFetcherPixelX = m_RenderX;
            m_BGFetcherFineDiscard = static_cast<byte>(
                (m_ScrollX + m_RenderX) & 0x07);
        }

        const unsigned int pixelX = m_BGFetcherWindow
            ? m_BGFetcherPixelX
            : static_cast<unsigned int>(
                m_ScrollX + m_BGFetcherPixelX) & 0xFF;
        const unsigned int pixelY = m_BGFetcherWindow
            ? m_WindowLineCounter
            : static_cast<unsigned int>(
                m_LCDControllerYCoordinate + m_ScrollY) & 0xFF;
        const ushort tileMap = m_BGFetcherWindow
            ? (WindowTileMapDisplaySelect ? 0x1C00 : 0x1800)
            : (BGTileMapDisplaySelect ? 0x1C00 : 0x1800);
        m_BGFetcherAddress = static_cast<ushort>(
            tileMap + (((pixelY / 8) * 32) & 0x03E0) +
            ((pixelX / 8) & 0x1F));
        m_BGFetcherStage = 1;
        return;
    }

    if (m_BGFetcherStage == 1)
    {
        m_BGFetcherTileNumber = m_VRAM[0][m_BGFetcherAddress];
        m_BGFetcherTileAttributes =
            IsCGBRendering() ? m_VRAM[1][m_BGFetcherAddress] : 0x00;
        m_BGFetcherStage = 2;
        return;
    }

    if ((m_BGFetcherStage == 2) || (m_BGFetcherStage == 4))
    {
        const unsigned int pixelY = m_BGFetcherWindow
            ? m_WindowLineCounter
            : static_cast<unsigned int>(
                m_LCDControllerYCoordinate + m_ScrollY) & 0xFF;
        byte tileRow = static_cast<byte>(pixelY & 0x07);
        if (IsCGBRendering() && ISBITSET(m_BGFetcherTileAttributes, 6))
        {
            tileRow = static_cast<byte>(7 - tileRow);
        }
        m_BGFetcherTileRow = tileRow;
        m_BGFetcherUnsignedTiles = BGWindowTileDataSelect;
        const ushort tileData = m_BGFetcherUnsignedTiles
            ? static_cast<ushort>(m_BGFetcherTileNumber * 0x10)
            : static_cast<ushort>(
                0x1000 + static_cast<sbyte>(m_BGFetcherTileNumber) * 0x10);
        m_BGFetcherAddress = static_cast<ushort>(
            tileData + (m_BGFetcherTileRow * 2) +
            (m_BGFetcherStage == 4 ? 1 : 0));
        m_BGFetcherStage++;
        return;
    }

    const byte tileBank =
        IsCGBRendering() ? ((m_BGFetcherTileAttributes >> 3) & 0x01) : 0x00;
    if (m_BGFetcherStage == 3)
    {
        m_BGFetcherTileLow = m_VRAM[tileBank][m_BGFetcherAddress];
        m_BGFetcherStage = 4;
        return;
    }

    m_BGFetcherTileHigh = m_VRAM[tileBank][m_BGFetcherAddress];
    for (byte pixel = 0; pixel < 8; pixel++)
    {
        const byte shift =
            IsCGBRendering() && ISBITSET(m_BGFetcherTileAttributes, 5)
                ? pixel
                : static_cast<byte>(7 - pixel);
        const byte colorIndex =
            (ISBITSET(m_BGFetcherTileLow, shift) ? 0x01 : 0x00) |
            (ISBITSET(m_BGFetcherTileHigh, shift) ? 0x02 : 0x00);
        const byte tail = static_cast<byte>(
            (m_BGPixelFIFOHead + m_BGPixelFIFOSize) & 0x0F);
        m_BGPixelFIFO[tail] = colorIndex;
        m_BGPixelFIFOAttributes[tail] = m_BGFetcherTileAttributes;
        m_BGPixelFIFOSize++;
    }
    m_BGFetcherPixelX += 8;
    m_BGFetcherStage = 0;
}

bool GPU::PopBackgroundPixel(byte& colorIndex, byte& attributes)
{
    if (m_BGPixelFIFOSize == 0)
    {
        return false;
    }

    colorIndex = m_BGPixelFIFO[m_BGPixelFIFOHead];
    attributes = m_BGPixelFIFOAttributes[m_BGPixelFIFOHead];
    m_BGPixelFIFOHead = static_cast<byte>((m_BGPixelFIFOHead + 1) & 0x0F);
    m_BGPixelFIFOSize--;
    return true;
}

void GPU::RenderPixel(byte x, byte bgColorIndex, byte bgAttributes)
{
    const bool cgb = IsCGBRendering();
    if (!cgb && !BGDisplayEnable)
    {
        bgColorIndex = 0;
        bgAttributes = 0;
    }

    const bool bgPriority = cgb && ISBITSET(bgAttributes, 7);
    ushort bgNative = 0x0000;
    byte bgRed = GBColors[0];
    byte bgGreen = GBColors[0];
    byte bgBlue = GBColors[0];

    if (cgb)
    {
        bgNative =
            ReadCGBPalette(m_BGPaletteRAM, bgAttributes & 0x07, bgColorIndex);
        bgRed = Expand5To8(bgNative & 0x1F);
        bgGreen = Expand5To8((bgNative >> 5) & 0x1F);
        bgBlue = Expand5To8((bgNative >> 10) & 0x1F);
    }
    else
    {
        bgNative =
            static_cast<ushort>((m_BGPaletteData >> (bgColorIndex * 2)) & 0x03);
        bgRed = GBColors[bgNative];
        bgGreen = bgRed;
        bgBlue = bgRed;
    }

    SetBGPixel(
        x,
        bgColorIndex,
        bgPriority,
        bgRed,
        bgGreen,
        bgBlue,
        bgNative);
    m_bgColorIndexLine[x] = bgColorIndex;

    const int outputIndex = ((m_LCDControllerYCoordinate * 160) + x) * 4;
    memcpy(m_DisplayPixels + outputIndex, m_bgPixels + outputIndex, 4);

    if (!OBJDisplayEnable)
    {
        return;
    }

    const bool useOAMPriority =
        cgb && !ISBITSET(m_MMU->Read(ObjectPriorityMode), 0);
    int winner = -1;
    byte winnerX = 0xFF;
    byte winnerPixel = 0;
    byte winnerFlags = 0;

    for (byte sprite = 0; sprite < m_lineSpriteCount; sprite++)
    {
        const int oamIndex = m_lineSpriteOAMIndex[sprite];
        const byte objectX = m_OAM[oamIndex + 1];
        const int left = static_cast<int>(objectX) - 8;
        if ((static_cast<int>(x) < left) || (static_cast<int>(x) >= left + 8))
        {
            continue;
        }

        const byte flags = m_OAM[oamIndex + 3];
        const int pixelInSprite = static_cast<int>(x) - left;
        const byte bit = ISBITSET(flags, 5)
            ? static_cast<byte>(pixelInSprite)
            : static_cast<byte>(7 - pixelInSprite);
        const int height = OBJSize ? 16 : 8;
        const int top = static_cast<int>(m_OAM[oamIndex]) - 16;
        int row = static_cast<int>(m_LCDControllerYCoordinate) - top;
        if (ISBITSET(flags, 6))
        {
            row = height - 1 - row;
        }

        // The sprite list is selected at the end of the previous line, but Y
        // and LCDC.2 are re-read here - an in-flight OAM DMA or a mid-line
        // OBJ-size change can move the row outside the tile it was chosen
        // for. A negative row would wrap the ushort index below and read past
        // the VRAM bank.
        if ((row < 0) || (row >= height))
        {
            continue;
        }

        byte tileNumber = m_OAM[oamIndex + 2];
        if (height == 16)
        {
            tileNumber &= 0xFE;
        }

        const byte tileBank = cgb ? ((flags >> 3) & 0x01) : 0x00;
        const ushort tileData =
            static_cast<ushort>((tileNumber * 0x10) + (row * 2));
        const byte pixel =
            (ISBITSET(m_VRAM[tileBank][tileData], bit) ? 0x01 : 0x00) |
            (ISBITSET(m_VRAM[tileBank][tileData + 1], bit) ? 0x02 : 0x00);
        if (pixel == 0)
        {
            continue;
        }

        if ((winner < 0) ||
            (!useOAMPriority &&
                ((objectX < winnerX) ||
                 ((objectX == winnerX) && (oamIndex < winner)))))
        {
            winner = oamIndex;
            winnerX = objectX;
            winnerPixel = pixel;
            winnerFlags = flags;

            if (useOAMPriority)
            {
                break;
            }
        }
    }

    if (winner < 0)
    {
        return;
    }

    bool objectWins;
    if (cgb)
    {
        if (!BGDisplayEnable)
        {
            objectWins = true;
        }
        else if (m_LineBGPriority[x] || ISBITSET(winnerFlags, 7))
        {
            objectWins = (m_LineColorIndex[x] == 0x00);
        }
        else
        {
            objectWins = true;
        }
    }
    else
    {
        objectWins =
            !ISBITSET(winnerFlags, 7) || (m_LineColorIndex[x] == 0x00);
    }

    if (!objectWins)
    {
        return;
    }

    if (cgb)
    {
        const ushort native =
            ReadCGBPalette(m_OBJPaletteRAM, winnerFlags & 0x07, winnerPixel);
        SetOBJPixel(
            x,
            Expand5To8(native & 0x1F),
            Expand5To8((native >> 5) & 0x1F),
            Expand5To8((native >> 10) & 0x1F),
            native);
    }
    else
    {
        const byte paletteData =
            ISBITSET(winnerFlags, 4) ? m_ObjectPalette1Data : m_ObjectPalette0Data;
        const byte shade =
            static_cast<byte>((paletteData >> (winnerPixel * 2)) & 0x03);
        const byte color = GBColors[shade];
        SetOBJPixel(x, color, color, color, shade);
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
        if (IsLCDDisplayEnabled && (GETMODE == ModeReadingOAMVRAM))
        {
            return 0xFF;
        }

        return m_VRAM[m_VRAMBank][address - 0x8000];
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (m_DMAActive ||
            (IsLCDDisplayEnabled &&
             ((GETMODE == ModeReadingOAM) || (GETMODE == ModeReadingOAMVRAM))))
        {
            return 0xFF;
        }

        return m_OAM[address - 0xFE00];
    }

    const bool cgb = IsCGBRendering();

    switch (address)
    {
    case LCDControl:
        return m_LCDControl;
    case LCDControllerStatus:
        // Bit 7 is unused and always reads back as set.
        return m_LCDControllerStatus | 0x80;
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
    case VRAMBankSelect:
        // Only bit 0 exists; the rest read as 1.
        return cgb ? static_cast<byte>(m_VRAMBank | 0xFE) : 0xFF;
    case NewDMASourceHigh:
    case NewDMASourceLow:
    case NewDMADestinationHigh:
    case NewDMADestinationLow:
        // The HDMA address registers are write-only.
        return 0xFF;
    case NewDMALength:
        if (!cgb)
        {
            return 0xFF;
        }
        // Bit 7 clear means an HBlank transfer is still running; the low 7 bits
        // are the number of blocks left, minus one.
        return m_HDMAActive
            ? static_cast<byte>((m_HDMABlocksRemaining - 1) & 0x7F)
            : static_cast<byte>(0x80 | ((m_HDMABlocksRemaining - 1) & 0x7F));
    case BGPaletteIndex:
        return cgb ? static_cast<byte>(m_BGPaletteIndex | 0x40) : 0xFF;
    case BGPaletteWriteData:
        return cgb ? m_BGPaletteRAM[m_BGPaletteIndex & 0x3F] : 0xFF;
    case OBJPaletteIndex:
        return cgb ? static_cast<byte>(m_OBJPaletteIndex | 0x40) : 0xFF;
    case OBJPaletteWriteData:
        return cgb ? m_OBJPaletteRAM[m_OBJPaletteIndex & 0x3F] : 0xFF;
    case DMATransferAndStartAddress:
        return m_DMARegister;
    default:
        // Unmapped I/O in the PPU's range reads back as 0xFF on hardware.
        return 0xFF;
    }
}

bool GPU::WriteByte(const ushort& address, const byte val)
{
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (IsLCDDisplayEnabled && (GETMODE == ModeReadingOAMVRAM))
        {
            return true;
        }

        m_VRAM[m_VRAMBank][address - 0x8000] = val;
        return true;
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (m_DMAActive ||
            (IsLCDDisplayEnabled &&
             ((GETMODE == ModeReadingOAM) || (GETMODE == ModeReadingOAMVRAM))))
        {
            return true;
        }

        m_OAM[address - 0xFE00] = val;
        return true;
    }

    const bool cgb = IsCGBRendering();

    switch (address)
    {
    case LCDControl:
        {
            bool wasOn = IsLCDDisplayEnabled;
            m_LCDControl = val;
            bool isOn = IsLCDDisplayEnabled;
            if (wasOn && !isOn)
            {
                DisableLCD();
            }
            else if (!wasOn && isOn)
            {
                EnableLCD();
            }
        }
        return true;
    case LCDControllerStatus:
        // Only bits 6-3 are writable. Bits 2-0 are status and bit 7 is unused.
        m_LCDControllerStatus = (val & 0x78) | (m_LCDControllerStatus & 0x87);
        // Enabling a STAT source while its condition already holds raises the
        // shared interrupt line immediately.
        UpdateStatInterruptLine();
        return true;
    case ScrollY:
        m_ScrollY = val;
        return true;
    case ScrollX:
        m_ScrollX = val;
        return true;
    case LCDControllerYCoordinate:
        // LY is read-only.
        return true;
    case LYCompare:
        m_LYCompare = val;
        UpdateCoincidenceFlag();
        UpdateStatInterruptLine();
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
    case VRAMBankSelect:
        if (cgb)
        {
            m_VRAMBank = val & 0x01;
        }
        return true;
    case NewDMASourceHigh:
        if (cgb)
        {
            m_HDMASource = static_cast<ushort>((m_HDMASource & 0x00FF) | (val << 8));
        }
        return true;
    case NewDMASourceLow:
        if (cgb)
        {
            // The low 4 bits of the source are ignored: transfers are 16-byte aligned.
            m_HDMASource = static_cast<ushort>((m_HDMASource & 0xFF00) | (val & 0xF0));
        }
        return true;
    case NewDMADestinationHigh:
        if (cgb)
        {
            // The destination is always in VRAM, so only bits 12-8 matter.
            m_HDMADestination = static_cast<ushort>((m_HDMADestination & 0x00FF) | ((val & 0x1F) << 8));
        }
        return true;
    case NewDMADestinationLow:
        if (cgb)
        {
            m_HDMADestination = static_cast<ushort>((m_HDMADestination & 0xFF00) | (val & 0xF0));
        }
        return true;
    case NewDMALength:
        if (cgb)
        {
            StartNewDMATransfer(val);
        }
        return true;
    case BGPaletteIndex:
        if (cgb)
        {
            m_BGPaletteIndex = val & 0xBF;
        }
        return true;
    case BGPaletteWriteData:
        if (cgb)
        {
            m_BGPaletteRAM[m_BGPaletteIndex & 0x3F] = val;
            if (ISBITSET(m_BGPaletteIndex, 7))
            {
                // Auto increment wraps within the 64-byte palette RAM and
                // deliberately preserves the auto-increment bit itself.
                m_BGPaletteIndex = static_cast<byte>(0x80 | ((m_BGPaletteIndex + 1) & 0x3F));
            }
        }
        return true;
    case OBJPaletteIndex:
        if (cgb)
        {
            m_OBJPaletteIndex = val & 0xBF;
        }
        return true;
    case OBJPaletteWriteData:
        if (cgb)
        {
            m_OBJPaletteRAM[m_OBJPaletteIndex & 0x3F] = val;
            if (ISBITSET(m_OBJPaletteIndex, 7))
            {
                m_OBJPaletteIndex = static_cast<byte>(0x80 | ((m_OBJPaletteIndex + 1) & 0x3F));
            }
        }
        return true;
    default:
        // Unmapped I/O in the PPU's range: writes are dropped on hardware.
        return true;
    }
}

/*
    HDMA5 write.

    Bit 7 selects the mode: 0 is a general purpose transfer that copies
    everything at once while the CPU is stopped, 1 is an HBlank transfer that
    copies one 16-byte block per HBlank. Writing bit 7 = 0 while an HBlank
    transfer is running cancels it instead of starting a new transfer.

    Bits 6-0 are the length in 16-byte blocks, minus one.
*/
void GPU::StartNewDMATransfer(byte val)
{
    const byte blocks = static_cast<byte>((val & 0x7F) + 1);

    if (!ISBITSET(val, 7))
    {
        if (m_HDMAActive || m_HDMATerminationWindow)
        {
            m_HDMAActive = false;
            m_HDMATerminationWindow = false;
            return;
        }

        // General purpose DMA: the whole transfer happens now.
        m_HDMABlocksRemaining = blocks;
        while (m_HDMABlocksRemaining > 0)
        {
            PerformHDMABlock();
        }

        m_GDMATransferCount++;
        m_HDMATerminationWindow = false;
        return;
    }

    m_HDMABlocksRemaining = blocks;
    m_HDMAActive = true;
    m_HDMABlockTransferred = false;

    // If the PPU is already in HBlank when the transfer is armed, the first
    // block is copied immediately rather than waiting a whole scanline.
    if (IsLCDDisplayEnabled && (GETMODE == ModeHBlank))
    {
        PerformHDMABlock();
    }
}

byte GPU::ReadHDMASourceByte(ushort address) const
{
    if (((address >= 0x8000) && (address < 0xA000)) || (address >= 0xFE00))
    {
        return 0xFF;
    }

    return m_MMU->Read(address);
}

void GPU::AbortHBlankDMAForSpeedSwitch()
{
    if (!m_HDMABlockTransferred)
    {
        m_HDMAActive = false;
    }
}

void GPU::PerformHDMABlock()
{
    if (m_HDMABlocksRemaining == 0)
    {
        m_HDMAActive = false;
        return;
    }

    for (byte offset = 0; offset < 0x10; offset++)
    {
        const ushort source = static_cast<ushort>(m_HDMASource + offset);
        const byte value = ReadHDMASourceByte(source);

        // The destination always lands in the currently selected VRAM bank.
        const ushort destination = static_cast<ushort>((m_HDMADestination + offset) & 0x1FFF);
        m_VRAM[m_VRAMBank][destination] = value;
    }

    m_HDMASource = static_cast<ushort>(m_HDMASource + 0x10);
    m_HDMADestination = static_cast<ushort>(m_HDMADestination + 0x10);
    m_HDMABlocksRemaining--;
    m_HDMABlockTransferred = true;

    // Each block costs 8 machine cycles of bus time.
    m_DMAStallCycles += 32;

    if (m_HDMABlocksRemaining == 0)
    {
        m_HDMATerminationWindow = m_HDMAActive;
        m_HDMAActive = false;
    }
}

void GPU::SetVSyncCallback(void(*pCallback)())
{
    m_pVSyncCallback = pCallback;
}

void GPU::PreBoot()
{
    // The real boot ROM leaves LCDC = 0x91 (LCD on, BG & window tile data at
    // 0x8000, BG display on) when it hands off control. Previously this was
    // never set here, so m_LCDControl stayed at its constructor default of
    // 0x00 (LCD "off") until a ROM happened to write LCDC itself - any ROM
    // that assumes the screen is already on at boot (as real hardware
    // guarantees) rendered nothing until then.
    m_LCDControl = 0x91;
    m_LCDControllerYCoordinate = 0x91;
    m_ScrollY = 0x00;
    m_ScrollX = 0x00;
    m_LYCompare = 0x00;
    m_BGPaletteData = 0xFC;
    m_ObjectPalette0Data = 0xFF;
    m_ObjectPalette1Data = 0xFF;
    m_WindowYPosition = 0x00;
    m_WindowXPositionMinus7 = 0x00;

    // The reference boot ROM completes partway through VBlank on line 0x91.
    m_ModeClock = 0;
    m_Line153LYReset = false;
    m_FirstLineAfterLCDEnable = false;
    m_InternalMode2STATEventFired = false;
    m_lineSpriteCount = 0;
    SETMODE(ModeVBlank);
    UpdateCoincidenceFlag();
    UpdateStatInterruptLine();

    m_VRAMBank = 0x00;
    m_BGPaletteIndex = 0x00;
    m_OBJPaletteIndex = 0x00;
    m_WindowLineCounter = 0x00;
    m_WindowYTriggered = false;
    m_HDMAActive = false;
    m_HDMABlocksRemaining = 0x00;
    m_GDMATransferCount = 0;
    m_HDMATerminationWindow = false;
    m_DMAStallCycles = 0;

    /*
        The CGB boot ROM leaves the HDMA registers set to all ones, so the
        first read of HDMA5 reports "no transfer in progress" and the source and
        destination latches are saturated.
    */
    m_HDMASource = 0xFFF0;
    m_HDMADestination = 0x1FF0;

    // Initialize color to white
    memset(m_DisplayPixels, GBColors[0], ARRAYSIZE(m_DisplayPixels));
    for (unsigned int a = 0;a < ARRAYSIZE(m_DisplayPixels);a += 4)
    {
        m_DisplayPixels[a] = 0xFF;   // Set Alpha to 0xFF
    }

    for (unsigned int a = 0; a < ARRAYSIZE(m_NativePixels); a++)
    {
        m_NativePixels[a] = IsCGBRendering() ? 0x7FFF : 0x0000;
    }
}

// The display was turned off. Real hardware resets LY to 0 and reports mode
// 0 for as long as it stays disabled, and it does not resume from wherever
// it left off when re-enabled (PPU-001).
void GPU::DisableLCD()
{
    // Clear the screen to white while the LCD is off.
    memset(m_DisplayPixels, GBColors[0], ARRAYSIZE(m_DisplayPixels));
    for (unsigned int a = 0; a < ARRAYSIZE(m_DisplayPixels); a += 4)
    {
        m_DisplayPixels[a] = 0xFF;   // Set Alpha to 0xFF
    }
    for (unsigned int a = 0; a < ARRAYSIZE(m_NativePixels); a++)
    {
        m_NativePixels[a] = IsCGBRendering() ? 0x7FFF : 0x0000;
    }

    m_LCDControllerYCoordinate = 0;
    m_ModeClock = 0;
    m_Line153LYReset = false;
    m_FirstLineAfterLCDEnable = false;
    m_InternalMode2STATEventFired = false;
    m_WindowLineCounter = 0;
    m_WindowYTriggered = false;
    m_lineSpriteCount = 0;
    SETMODE(ModeHBlank);
    UpdateCoincidenceFlag();
    m_StatInterruptLine = false;
}

// Re-enabling the LCD always begins a fresh frame: LY=0, mode 2 (OAM scan).
void GPU::EnableLCD()
{
    m_ModeClock = 0;
    m_LCDControllerYCoordinate = 0;
    m_Line153LYReset = false;
    m_FirstLineAfterLCDEnable = true;
    m_InternalMode2STATEventFired = false;
    m_WindowLineCounter = 0;
    m_WindowYTriggered = false;
    ScanSpritesForLine(m_LCDControllerYCoordinate);
    ComputeScanlineTiming();
    // The LCD-restart scanline advances LY after 452 dots instead of 456.
    m_Mode0Cycles -= 4;
    EnterMode(ModeReadingOAM);
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

    The transfer progresses one byte per M-cycle (4 T-cycles) for all 160 bytes (640 T-cycles total),
    reading the source fresh from the bus on each step rather than copying everything up front. The new
    source/offset do not take effect until 2 M-cycles (8 T-cycles) after this write. If a transfer is
    already running, it is not immediately stopped: it keeps copying from its old source until that delay
    elapses, at which point the new source takes over from byte 0 (DMA-001).
    */
    m_DMAPendingSource = static_cast<ushort>(address) * 0x0100;
    m_DMAPendingCyclesRemaining = OAMDMAStartDelayCycles;
    m_DMAPending = true;
    m_DMARegister = address;
}

void GPU::RenderScanline()
{
    RenderBackgroundScanline();

    /*
        The window is only drawn once LY has reached WY, and only while WX is
        on screen. Every line on which it is drawn advances an internal counter
        that is completely independent of LY: cgb-acid2 relies on this by
        parking WX off screen for part of the frame and expecting the window to
        resume from the row it had reached, not from LY - WY.
    */
    if (WindowDisplayEnable &&
        (m_LCDControllerYCoordinate >= m_WindowYPosition) &&
        (m_WindowXPositionMinus7 <= 166))
    {
        RenderWindowScanline();
        m_WindowLineCounter++;
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

void GPU::SetBGPixel(int x, byte colorIndex, bool bgPriority, byte r, byte g, byte b, ushort nativeValue)
{
    const int pixel = (m_LCDControllerYCoordinate * 160) + x;
    const int index = pixel * 4;

    m_bgPixels[index + 3] = r;
#if TINT
    if (m_bgPixels[index + 3] == 0x00) m_bgPixels[index + 3] = 0x30;
    m_bgPixels[index + 2] = 0x00;
    m_bgPixels[index + 1] = 0x00;
#else
    m_bgPixels[index + 2] = g;
    m_bgPixels[index + 1] = b;
#endif
    m_bgPixels[index + 0] = 0xFF;

    m_NativePixels[pixel] = nativeValue;
    m_LineColorIndex[x] = colorIndex;
    m_LineBGPriority[x] = bgPriority;
}

void GPU::SetOBJPixel(int x, byte r, byte g, byte b, ushort nativeValue)
{
    const int pixel = (m_LCDControllerYCoordinate * 160) + x;
    const int index = pixel * 4;

    m_DisplayPixels[index + 2] = g;
#if TINT
    if (m_DisplayPixels[index + 2] == 0x00) m_DisplayPixels[index + 2] = 0x30;
    m_DisplayPixels[index + 3] = 0x00;
    m_DisplayPixels[index + 1] = 0x00;
#else
    m_DisplayPixels[index + 3] = r;
    m_DisplayPixels[index + 1] = b;
#endif
    m_DisplayPixels[index + 0] = 0xFF;

    m_NativePixels[pixel] = nativeValue;
}

void GPU::RenderBackgroundScanline()
{
    const bool cgb = IsCGBRendering();

    /*
        LCDC.0 means two different things depending on the mode.

        On a DMG, clearing it blanks the background to white; the window and
        sprites keep drawing.

        On a CGB running a colour cartridge it stops being a "BG enable" bit and
        becomes "BG/window lose all priority": the background is still drawn,
        but sprites are unconditionally in front of it.
    */
    if (!cgb && !BGDisplayEnable)
    {
        for (int x = 0; x < 160; x++)
        {
            SetBGPixel(x, 0x00, false, GBColors[0], GBColors[0], GBColors[0], 0x0000);
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
    byte tileRow = (byte)((m_LCDControllerYCoordinate + m_ScrollY) % 8);

    // Now loop through each pixel on this line
    for (byte x = 0; x < 160; x++)
    {
        // We need to determine the current X tile (in the same way we did the Y tile)
        byte tileX = (byte)(((m_ScrollX + x) / 8) % 32);

        const ushort mapOffset = (ushort)(tileNumberMap + (tileY * 32) + tileX);

        // Finally, we can read the correct tile number from the tile map (32x32)
        byte tileNumber = m_VRAM[0][mapOffset];

        /*
            On a CGB the second VRAM bank holds an attribute byte for every tile
            map entry: palette (bits 0-2), tile bank (bit 3), X/Y flip (bits 5/6)
            and the BG-over-OBJ priority flag (bit 7).
        */
        const byte attributes = cgb ? m_VRAM[1][mapOffset] : 0x00;
        const byte tileBank = cgb ? ((attributes >> 3) & 0x01) : 0x00;
        const byte tileYOffset = (cgb && ISBITSET(attributes, 6)) ? (byte)(7 - tileRow) : tileRow;

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
        byte b1 = m_VRAM[tileBank][tileDataPtr];
        byte b2 = m_VRAM[tileBank][(ushort)(tileDataPtr + 1)];

        // Figure out which palette # it uses
        byte pixelInTile = (byte)((m_ScrollX + x) % 8);
        byte bit = (cgb && ISBITSET(attributes, 5)) ? pixelInTile : (byte)(7 - pixelInTile);
        byte pLo = ISBITSET(b1, bit) ? 0x01 : 0x00;
        byte pHi = ISBITSET(b2, bit) ? 0x02 : 0x00;
        byte colorIndex = pLo + pHi;

        if (cgb)
        {
            const ushort native = ReadCGBPalette(m_BGPaletteRAM, attributes & 0x07, colorIndex);
            SetBGPixel(
                x,
                colorIndex,
                ISBITSET(attributes, 7),
                Expand5To8(native & 0x1F),
                Expand5To8((native >> 5) & 0x1F),
                Expand5To8((native >> 10) & 0x1F),
                native);
        }
        else
        {
            const byte shade = (m_BGPaletteData >> (colorIndex * 2)) & 0x03;
            const byte color = palette[colorIndex];
            SetBGPixel(x, colorIndex, false, color, color, color, shade);
        }
    }
}

void GPU::RenderWindowScanline()
{
    const bool cgb = IsCGBRendering();

    const int winY = m_WindowLineCounter;

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
    byte tileY = (byte)((winY / 8) % 32);
    byte tileRow = (byte)(winY % 8);

    // Get the relative window position
    int winX = m_WindowXPositionMinus7 - 7;
    for (int x = 0; x < 160; x++)
    {
        // If the window is to the right of here, don't do anything
        if (x < winX)
            continue;

        // Get the X tile for this pixel
        byte tileX = (byte)(((x - winX) / 8) % 32);

        const ushort mapOffset = (ushort)(tileNumberMap + (tileY * 32) + tileX);

        // Calculate the tile number
        byte tileNumber = m_VRAM[0][mapOffset];

        const byte attributes = cgb ? m_VRAM[1][mapOffset] : 0x00;
        const byte tileBank = cgb ? ((attributes >> 3) & 0x01) : 0x00;
        const byte tileYOffset = (cgb && ISBITSET(attributes, 6)) ? (byte)(7 - tileRow) : tileRow;

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
        byte b1 = m_VRAM[tileBank][tileDataPtr];
        byte b2 = m_VRAM[tileBank][(ushort)(tileDataPtr + 1)];

        /*
            The pixel column has to be taken relative to the window origin, not
            to the screen. They only agree when WX-7 happens to be a multiple of
            eight, which is why the old screen-relative version worked for the
            common WX == 7 case and skewed every other window position.
        */
        byte pixelInTile = (byte)((x - winX) % 8);
        byte bit = (cgb && ISBITSET(attributes, 5)) ? pixelInTile : (byte)(7 - pixelInTile);
        byte pLo = ISBITSET(b1, bit) ? 0x01 : 0x00;
        byte pHi = ISBITSET(b2, bit) ? 0x02 : 0x00;
        byte colorIndex = pLo + pHi;

        if (cgb)
        {
            const ushort native = ReadCGBPalette(m_BGPaletteRAM, attributes & 0x07, colorIndex);
            SetBGPixel(
                x,
                colorIndex,
                ISBITSET(attributes, 7),
                Expand5To8(native & 0x1F),
                Expand5To8((native >> 5) & 0x1F),
                Expand5To8((native >> 10) & 0x1F),
                native);
        }
        else
        {
            const byte shade = (m_BGPaletteData >> (colorIndex * 2)) & 0x03;
            const byte color = palette[colorIndex];
            SetBGPixel(x, colorIndex, false, color, color, color, shade);
        }
    }
}

void GPU::RenderOBJScanline()
{
    const byte SPRITESIZEINBYTES = 16;
    const bool cgb = IsCGBRendering();
    const int height = OBJSize ? 0x10 : 0x08;

    /*
        Object priority.

        The PPU scans OAM in address order and keeps at most the first ten
        sprites that overlap the current line; sprites found after that are
        dropped entirely, no matter where they are.

        Which of the kept sprites ends up on top depends on the mode:
        - CGB with a colour cartridge (and OPRI clear) uses the OAM index alone.
        - DMG, CGB-in-compatibility-mode, and CGB with OPRI set use the smaller
          X coordinate, falling back to the OAM index when X ties.
    */
    int candidates[10];
    int candidateCount = 0;

    for (int oam = 0; oam < 160 && candidateCount < 10; oam += 4)
    {
        const int y = static_cast<int>(m_OAM[oam]) - 16;
        if ((y <= m_LCDControllerYCoordinate) && ((y + height) > m_LCDControllerYCoordinate))
        {
            candidates[candidateCount++] = oam;
        }
    }

    if (candidateCount == 0)
    {
        return;
    }

    bool useOAMPriority = cgb;
    if (cgb)
    {
        // OPRI bit 0 set puts a CGB back on the DMG's X-coordinate priority.
        useOAMPriority = !ISBITSET(m_MMU->Read(ObjectPriorityMode), 0);
    }

    if (!useOAMPriority)
    {
        // Stable insertion sort: smaller X first, OAM order preserved on ties.
        for (int i = 1; i < candidateCount; i++)
        {
            const int current = candidates[i];
            const byte currentX = m_OAM[current + 1];
            int j = i - 1;
            while (j >= 0 && m_OAM[candidates[j] + 1] > currentX)
            {
                candidates[j + 1] = candidates[j];
                j--;
            }

            candidates[j + 1] = current;
        }
    }

    // The winning sprite pixel is resolved before the background is consulted,
    // because a sprite hidden behind the background still hides the sprites
    // underneath it.
    bool objWritten[160] = {};
    byte objColorIndex[160] = {};
    byte objPalette[160] = {};
    bool objBehindBG[160] = {};

    for (int slot = 0; slot < candidateCount; slot++)
    {
        const int i = candidates[slot];

        const byte objY = m_OAM[i];
        const byte objX = m_OAM[i + 1];
        byte spriteTileNumber = m_OAM[i + 2];
        const byte spriteFlags = m_OAM[i + 3];

        if (height == 0x10)
        {
            spriteTileNumber &= 0xFE;
        }

        const int y = objY - 16;
        const int x = objX - 8;

        const byte tileBank = cgb ? ((spriteFlags >> 3) & 0x01) : 0x00;

        // Mapped for direct VRAM access
        const ushort tileData = 0x0000;

        ushort tilePointer = tileData + (spriteTileNumber * SPRITESIZEINBYTES);
        byte tileYOffset = ISBITSET(spriteFlags, 6)
            ? (byte)((height - 1) - (m_LCDControllerYCoordinate - y))
            : (byte)(m_LCDControllerYCoordinate - y);
        tilePointer += (tileYOffset * 2);

        // The data for this line of the sprite, 8 pixels
        const byte low = m_VRAM[tileBank][tilePointer];
        const byte high = m_VRAM[tileBank][(ushort)(tilePointer + 1)];

        for (int indexX = 0; indexX < 8; indexX++)
        {
            const int pixelX = x + indexX;
            if (pixelX < 0 || pixelX >= 160)
            {
                continue;
            }

            if (objWritten[pixelX])
            {
                // A higher priority sprite already owns this pixel.
                continue;
            }

            const byte bit = ISBITSET(spriteFlags, 5) ? (byte)indexX : (byte)(7 - indexX);
            byte pixelVal = 0x00;
            if (ISBITSET(high, bit)) pixelVal |= 0x02;
            if (ISBITSET(low, bit)) pixelVal |= 0x01;

            if (pixelVal == 0x00)
            {
                // Colour 0 is transparent for objects.
                continue;
            }

            objWritten[pixelX] = true;
            objColorIndex[pixelX] = pixelVal;
            objPalette[pixelX] = cgb
                ? (byte)(spriteFlags & 0x07)
                : (byte)(ISBITSET(spriteFlags, 4) ? 0x01 : 0x00);
            objBehindBG[pixelX] = ISBITSET(spriteFlags, 7);
        }
    }

    for (int pixelX = 0; pixelX < 160; pixelX++)
    {
        if (!objWritten[pixelX])
        {
            continue;
        }

        bool objectWins;
        if (cgb)
        {
            if (!BGDisplayEnable)
            {
                // LCDC.0 clear on a CGB strips the background of all priority.
                objectWins = true;
            }
            else if (m_LineBGPriority[pixelX] || objBehindBG[pixelX])
            {
                objectWins = (m_LineColorIndex[pixelX] == 0x00);
            }
            else
            {
                objectWins = true;
            }
        }
        else
        {
            objectWins = !objBehindBG[pixelX] || (m_LineColorIndex[pixelX] == 0x00);
        }

        if (!objectWins)
        {
            continue;
        }

        if (cgb)
        {
            const ushort native = ReadCGBPalette(m_OBJPaletteRAM, objPalette[pixelX], objColorIndex[pixelX]);
            SetOBJPixel(
                pixelX,
                Expand5To8(native & 0x1F),
                Expand5To8((native >> 5) & 0x1F),
                Expand5To8((native >> 10) & 0x1F),
                native);
        }
        else
        {
            const byte paletteData = (objPalette[pixelX] == 0x00) ? m_ObjectPalette0Data : m_ObjectPalette1Data;
            const byte shade = (paletteData >> (objColorIndex[pixelX] * 2)) & 0x03;
            const byte color = GBColors[shade];
            SetOBJPixel(pixelX, color, color, color, shade);
        }
    }
}
