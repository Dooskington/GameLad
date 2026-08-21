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

/*
    CGB-only PPU registers.

    FF4F - VBK   - VRAM bank select (bit 0)
    FF51 - HDMA1 - VRAM DMA source, high byte
    FF52 - HDMA2 - VRAM DMA source, low byte
    FF53 - HDMA3 - VRAM DMA destination, high byte
    FF54 - HDMA4 - VRAM DMA destination, low byte
    FF55 - HDMA5 - VRAM DMA length / mode / start
    FF68 - BCPS/BGPI - background palette index (bit 7 = auto increment)
    FF69 - BCPD/BGPD - background palette data
    FF6A - OCPS/OBPI - object palette index (bit 7 = auto increment)
    FF6B - OCPD/OBPD - object palette data
*/
#define VRAMBankSelect 0xFF4F
#define NewDMASourceHigh 0xFF51
#define NewDMASourceLow 0xFF52
#define NewDMADestinationHigh 0xFF53
#define NewDMADestinationLow 0xFF54
#define NewDMALength 0xFF55
#define BGPaletteIndex 0xFF68
#define BGPaletteWriteData 0xFF69
#define OBJPaletteIndex 0xFF6A
#define OBJPaletteWriteData 0xFF6B
#define ObjectPriorityMode 0xFF6C

#define ModeHBlank 0
#define ModeVBlank 1
#define ModeReadingOAM 2
#define ModeReadingOAMVRAM 3

#define CyclesPerScanline 456
#define VBlankCycles 456
#define HBlankCycles 204
#define ReadingOAMCycles 80
#define ReadingOAMVRAMCycles 172

// OAM DMA transfers one byte per CPU M-cycle for all 160 OAM bytes.
#define OAMDMACycles 640
#define OAMDMABytes 0xA0

// A write to FF46 takes effect 2 CPU M-cycles after the write.
#define OAMDMAStartDelayCycles 8

// LY becomes 144 three PPU dots before the VBlank request is visible. In CGB
// double speed those dots span six CPU T-cycles.
#define CGBDoubleSpeedVBlankInterruptDelayCycles 6

// While OAM DMA is in progress, the DMG CPU may only access High RAM
// (0xFF80-0xFFFE) and the Interrupt Enable register (0xFFFF). The DMA
// register itself (0xFF46) is always writable so a transfer can be restarted.
#define IsHRAMOrIEAddress(address) (((address) >= 0xFF80) && ((address) <= 0xFFFF))

// Maximum number of sprites the hardware evaluates and draws per scanline.
#define MaxSpritesPerScanline 10

enum class OAMBugAccess : byte
{
    Read,
    Write
};

enum class OAMBugOrigin : byte
{
    AddressBus,
    MemoryBus
};

class GPU : public IMemoryUnit
{
    friend class GPUTests;

public:
    GPU(IMMU* pMMU, ICPU* pCPU);
    ~GPU();

    void Step(unsigned long cycles);
    void Step(unsigned long baseCycles, unsigned long cpuCycles);
    // Host-facing video is latched only after all 144 visible lines complete.
    // The PPU keeps drawing the next hardware frame into separate working
    // buffers, so a frontend can never observe a top/bottom split mid-scanout.
    byte* GetCurrentFrame();
    void Serialize(StateSerializer& state);
    byte* GetVideoRAM() { return &m_VRAM[0][0]; }
    const byte* GetVideoRAM() const { return &m_VRAM[0][0]; }
    size_t GetVideoRAMSize() const { return sizeof(m_VRAM); }
    byte* GetOAM() { return m_OAM; }
    const byte* GetOAM() const { return m_OAM; }
    size_t GetOAMSize() const { return sizeof(m_OAM); }

    /*
        The latest completed, unconverted frame, one ushort per pixel:
        - CGB: the RGB555 value the PPU actually latched, with no host colour
          correction applied. Test oracles disagree about how RGB555 should be
          expanded to RGB888 (gambatte applies its own correction curve, the
          acid tests and AGE use a plain 5->8 expansion), so the emulator emits
          the hardware value and lets the comparison decide.
        - DMG: the shade index 0-3, before the palette is applied.
    */
    const ushort* GetCurrentNativeFrame() const { return m_CompletedNativePixels; }

    void SetGameBoyMode(GameBoyMode mode);
    GameBoyMode GetGameBoyMode() const { return m_mode; }

    /*
        Base-clock cycles the CPU still owes for a GDMA/HBlank-DMA block that
        has already been performed. The copy itself happens atomically and the
        CPU is charged for it afterwards, which stalls the program for the right
        amount of time without needing a cycle-accurate copy engine.
    */
    unsigned long ConsumeDMAStallCycles();
    unsigned long ConsumeGDMATransferCount();
    void NotifyDMAStallPaid() { m_HDMATerminationWindow = false; }
    void AbortHBlankDMAForSpeedSwitch();

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);
    void SetVSyncCallback(void(*pCallback)());
    void PreBoot();

    // Bus-access contract consulted by the CPU. OAM DMA owns either the main
    // bus or the video bus; I/O and HRAM remain independently accessible.
    bool IsOAMDMAActive() const;
    bool IsCPUAddressBlockedByOAMDMA(ushort address) const;
    void TriggerOAMBug(
        ushort address,
        OAMBugAccess access,
        OAMBugOrigin origin);
    byte GetOAMDMASnoopByte() const
    {
        return m_DMAActive
            ? ReadOAMDMASourceByte(static_cast<ushort>(m_DMASourceAddress + m_DMAOffset))
            : m_DMASnoopByte;
    }
    ushort GetOAMDMASourceAddress() const { return m_DMASourceAddress; }
    bool IsLine153STATPhase() const;

private:
    void LaunchDMATransfer(const byte address);
    void StepOAMDMA(unsigned long cycles);
    void StepVBlankInterruptDelay(unsigned long cpuCycles);
    int GetOAMBugRow(OAMBugOrigin origin) const;
    ushort ReadOAMWord(int row, int word) const;
    void WriteOAMWord(int row, int word, ushort value);
    void CopyOAMRow(int sourceRow, int destinationRow);
    void CorruptOAMWrite(int row);
    void CorruptOAMRead(int row);
    void EnterMode(byte mode);
    void DisableLCD();
    void EnableLCD();
    void UpdateCoincidenceFlag();
    void UpdateStatInterruptLine();
    void QueueInternalMode2STAT(byte nextLY);
    void ScanSpritesForLine(byte ly);
    void ComputeScanlineTiming();
    void BeginPixelTransfer();
    void AdvancePixelTransfer(unsigned long cycles);
    void FinishPixelTransfer();
    void ResetBackgroundFetcher(bool window);
    void AdvanceBackgroundFetcher();
    bool PopBackgroundPixel(byte& colorIndex, byte& attributes);
    void RenderPixel(byte x, byte bgColorIndex, byte bgAttributes);
    void RenderScanline();
    void RenderImage();
    void RenderBackgroundScanline();
    void RenderWindowScanline();
    void RenderOBJScanline();

    void SetBGPixel(int x, byte colorIndex, bool bgPriority, byte r, byte g, byte b, ushort nativeValue);
    void SetOBJPixel(int x, byte r, byte g, byte b, ushort nativeValue);

    // Resolves a CGB palette entry. paletteRAM is 8 palettes x 4 colours x 2 bytes.
    ushort ReadCGBPalette(const byte* paletteRAM, int palette, int colorIndex) const;

    void StartNewDMATransfer(byte val);
    byte ReadOAMDMASourceByte(ushort address) const;
    byte ReadHDMASourceByte(ushort address) const;
    void PerformHDMABlock();

    enum class OAMDMABus : byte { None, External, Video, Work };
    OAMDMABus BusForAddress(ushort address, bool isSource) const;
    bool IsCGBRendering() const;

private:
    IMMU* m_MMU;
    ICPU* m_CPU;
    GameBoyMode m_mode;

    /*
        VRAM. A CGB has two 8 KB banks; bank 1 holds the BG attribute maps and
        a second set of tiles. DMG only ever touches bank 0, so the DMG data
        layout is unchanged.
    */
    byte m_VRAM[2][0x1FFF + 1];
    byte m_VRAMBank;

    byte m_OAM[0x009F + 1];
    byte m_bgPixels[160 * 144 * 4];
    // Working scanout buffers, updated progressively as mode 3 emits pixels.
    byte m_DisplayPixels[160 * 144 * 4];
    ushort m_NativePixels[160 * 144];
    // Stable host-visible buffers, updated atomically on entry to VBlank.
    byte m_CompletedPixels[160 * 144 * 4];
    ushort m_CompletedNativePixels[160 * 144];

    /*
        Per-scanline shadow of the background/window layer used for sprite
        priority. Storing the colour *index* rather than comparing final pixel
        colours (which aliases whenever a palette maps two indices to the same
        shade) is what the hardware actually compares.
    */
    byte m_LineColorIndex[160];
    bool m_LineBGPriority[160];

    // The 2-bit BG/window color index (0-3) for the current scanline, kept
    // separate from the rendered shade so OBJ-behind-BG priority (which is
    // defined in terms of color index, not shade) can be evaluated correctly.
    byte m_bgColorIndexLine[160];

    unsigned long m_ModeClock;
    unsigned long m_Mode3Cycles;
    unsigned long m_Mode0Cycles;
    bool m_StatInterruptLine;
    bool m_InternalMode2STATEventFired;
    unsigned long m_VBlankInterruptDelayCycles;
    bool m_Line153LYReset;
    // The shortened LCD-restart line does not expose the DMG OAM bug.
    bool m_FirstLineAfterLCDEnable;

    // Internal window line counter. Only increments on lines where the
    // window is actually rendered, independent of the absolute LY/WY delta.
    byte m_WindowLineCounter;
    bool m_WindowYTriggered;

    // Sprites selected (in OAM order) for the scanline currently being
    // drawn, per the hardware's 10-sprites-per-line limit.
    byte m_lineSpriteOAMIndex[MaxSpritesPerScanline];
    byte m_lineSpriteCount;
    bool m_lineSpriteFetched[MaxSpritesPerScanline];
    byte m_RenderX;
    unsigned long m_PixelStartupCycles;
    unsigned long m_PixelStallCycles;
    unsigned long m_PixelFetcherAlignmentCycles;
    byte m_BGPixelFIFO[16];
    byte m_BGPixelFIFOAttributes[16];
    byte m_BGPixelFIFOHead;
    byte m_BGPixelFIFOSize;
    byte m_BGFetcherStage;
    byte m_BGFetcherTileNumber;
    byte m_BGFetcherTileAttributes;
    byte m_BGFetcherTileLow;
    byte m_BGFetcherTileHigh;
    byte m_BGFetcherTileRow;
    ushort m_BGFetcherAddress;
    unsigned int m_BGFetcherPixelX;
    byte m_BGFetcherFineDiscard;
    bool m_BGFetcherWindow;
    bool m_BGFetcherUnsignedTiles;
    bool m_PixelStallPausesFetcher;
    bool m_OBJEnabledAtTransferStart;
    bool m_WindowTriggeredThisLine;
    bool m_WindowTriggerMissedThisLine;
    bool m_WindowStartedThisLine;
    bool m_WindowEligibleThisLine;
    bool m_WindowPenaltyScheduled;
    int m_WindowStartX;

    // OAM DMA state. Transfers progress one byte per M-cycle rather than
    // completing instantly, and the CPU bus is restricted while active.
    bool m_DMAActive;
    ushort m_DMASourceAddress;
    byte m_DMAOffset;
    byte m_DMASnoopByte;
    unsigned long m_DMACyclesAccumulated;

    // The raw last-written value of FF46. Reads of FF46 always return this,
    // independent of the transfer's internal progress/pending state
    // (Mooneye oam_dma/reg_read).
    byte m_DMARegister;

    // A write to FF46 does not take effect immediately: real hardware delays
    // 2 M-cycles (8 T-cycles) before the new source/offset become live. If a
    // transfer is already active, it keeps running unaffected (still blocking
    // the bus) during that window; if none is active, the bus stays free
    // until the delay elapses (Mooneye oam_dma_start/oam_dma_restart).
    bool m_DMAPending;
    ushort m_DMAPendingSource;
    unsigned long m_DMAPendingCyclesRemaining;
    void(*m_pVSyncCallback)();

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

    // CGB palette RAM. 8 palettes * 4 colours * 2 bytes, little endian RGB555.
    byte m_BGPaletteRAM[64];
    byte m_OBJPaletteRAM[64];
    byte m_BGPaletteIndex;
    byte m_OBJPaletteIndex;

    // CGB VRAM DMA
    ushort m_HDMASource;
    ushort m_HDMADestination;
    byte m_HDMABlocksRemaining;   // number of 16-byte blocks still to copy
    bool m_HDMAActive;            // an HBlank-driven transfer is in progress
    bool m_HDMABlockTransferred;  // the current transfer has moved at least one block
    unsigned long m_GDMATransferCount; // GDMA transfers awaiting their setup charge
    bool m_HDMATerminationWindow; // final HBlank block copied, but its bus debt remains
    unsigned long m_DMAStallCycles;
};
