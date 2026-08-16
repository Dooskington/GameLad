#include "stdafx.h"

#include <APU.hpp>
#include <GPU.hpp>
#include <MMU.hpp>
#include <Serial.hpp>
#include <Timer.hpp>

/*
    Every test .cpp is #included into a single translation unit by TestMain.cpp,
    so macro names are global. Everything defined here carries a CGB_ prefix to
    stay clear of the register macros already defined by the DMG test files.
*/
#define CGB_KEY1 0xFF4D
#define CGB_VBK 0xFF4F
#define CGB_SVBK 0xFF70
#define CGB_BGPI 0xFF68
#define CGB_BGPD 0xFF69
#define CGB_OBPI 0xFF6A
#define CGB_OBPD 0xFF6B
#define CGB_HDMA1 0xFF51
#define CGB_HDMA2 0xFF52
#define CGB_HDMA3 0xFF53
#define CGB_HDMA4 0xFF54
#define CGB_HDMA5 0xFF55
#define CGB_OPRI 0xFF6C
#define CGB_RP 0xFF56
#define CGB_PCM12 0xFF76
#define CGB_PCM34 0xFF77
#define CGB_SB 0xFF01
#define CGB_SC 0xFF02
#define CGB_DIV 0xFF04
#define CGB_LCDC 0xFF40
#define CGB_STAT 0xFF41
#define CGB_LY 0xFF44
#define CGB_LYC 0xFF45
#define CGB_NR11 0xFF11
#define CGB_NR12 0xFF12
#define CGB_NR14 0xFF14
#define CGB_NR21 0xFF16
#define CGB_NR22 0xFF17
#define CGB_NR24 0xFF19
#define CGB_NR30 0xFF1A
#define CGB_NR31 0xFF1B
#define CGB_NR32 0xFF1C
#define CGB_NR33 0xFF1D
#define CGB_NR34 0xFF1E
#define CGB_NR41 0xFF20
#define CGB_NR42 0xFF21
#define CGB_NR44 0xFF23
#define CGB_NR52 0xFF26

// Flat 64 KB memory so the GPU's DMA/HDMA source reads are deterministic.
class CGBTestMMU : public IMMU
{
public:
    CGBTestMMU() : m_mode(GameBoyMode::CGB), m_doubleSpeed(false)
    {
        memset(m_memory, 0x00, ARRAYSIZE(m_memory));
    }

    byte Read(const ushort& address) { return m_memory[address]; }
    bool Write(const ushort& address, const byte val)
    {
        m_memory[address] = val;
        return true;
    }

    void RegisterMemoryUnit(const ushort&, const ushort&, IMemoryUnit*) {}
    unsigned short ReadUShort(const ushort& address)
    {
        return (unsigned short)(m_memory[address] | (m_memory[address + 1] << 8));
    }
    bool LoadBootROM(const char*) { return false; }

    void SetGameBoyMode(GameBoyMode mode) { m_mode = mode; }
    GameBoyMode GetGameBoyMode() const { return m_mode; }
    bool IsSpeedSwitchArmed() const { return false; }
    void CompleteSpeedSwitch() {}
    bool IsDoubleSpeed() const { return m_doubleSpeed; }

    byte m_memory[0x10000];
    GameBoyMode m_mode;
    bool m_doubleSpeed;
};

class CGBTestCPU : public ICPU
{
public:
    CGBTestCPU() : Interrupts(0), FrameSequencerClocks(0) {}

    bool Initialize() { return true; }
    bool LoadROM(const char*, const char*) { return true; }
    int Step() { return 0; }
    void TriggerInterrupt(byte) { Interrupts++; }
    byte* GetCurrentFrame() { return nullptr; }
    void SetInput(byte, byte) {}
    void SetVSyncCallback(void(*)()) {}
    void SetAudioSampleRate(unsigned int) {}
    size_t ConsumeAudioSamples(float*, size_t) { return 0; }
    void ClockAPUFrameSequencer() { FrameSequencerClocks++; }

    int Interrupts;
    int FrameSequencerClocks;
};

TEST_CLASS(CGBTests)
{
public:
    /*
        GPU::Step performs at most one mode transition per call, so tests that
        need the PPU to actually walk through a scanline have to feed it in
        small increments the way the scheduler does.
    */
    static void StepCycles(GPU& gpu, int cycles)
    {
        for (int i = 0; i < cycles; i += 4)
        {
            gpu.Step(4);
        }
    }

    // ---------------------------------------------------------------- model

    TEST_METHOD(ModeResolutionTest)
    {
        // 0x80 is "CGB enhanced, DMG compatible", 0xC0 is "CGB only".
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::Auto, 0x80) == GameBoyMode::CGB);
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::Auto, 0xC0) == GameBoyMode::CGB);
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::Auto, 0x00) == GameBoyMode::DMG);

        // Forcing a console overrides the header entirely: a CGB-only cartridge
        // asked to run on a DMG still gets DMG hardware, which is what real
        // silicon does (the cartridge's own check is what puts up an error).
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::ForceDMG, 0x00) == GameBoyMode::DMG);
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::ForceDMG, 0x80) == GameBoyMode::DMG);
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::ForceDMG, 0xC0) == GameBoyMode::DMG);

        // A CGB console running a DMG cartridge is a third, distinct mode.
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::ForceCGB, 0x00) == GameBoyMode::CGBCompatibility);
        Assert::IsTrue(ResolveGameBoyMode(ModelPreference::ForceCGB, 0x80) == GameBoyMode::CGB);

        Assert::IsTrue(IsCGBHardware(GameBoyMode::CGB));
        Assert::IsTrue(IsCGBHardware(GameBoyMode::CGBCompatibility));
        Assert::IsFalse(IsCGBHardware(GameBoyMode::DMG));

        // CGB *features* (banking, palettes, DMA) are gated by the cartridge,
        // not by the console, so compatibility mode must not expose them.
        Assert::IsTrue(IsCGBFeatureMode(GameBoyMode::CGB));
        Assert::IsFalse(IsCGBFeatureMode(GameBoyMode::CGBCompatibility));
        Assert::IsFalse(IsCGBFeatureMode(GameBoyMode::DMG));

        Assert::IsTrue(IsCGBCartridgeFlag(0x80));
        Assert::IsTrue(IsCGBCartridgeFlag(0xC0));
        Assert::IsFalse(IsCGBCartridgeFlag(0x00));
        Assert::IsTrue(IsCGBOnlyCartridgeFlag(0xC0));
        Assert::IsFalse(IsCGBOnlyCartridgeFlag(0x80));
    }

    // ------------------------------------------------------------ WRAM/SVBK

    TEST_METHOD(WRAMBankingTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::CGB);

        // SVBK reads back with the unused high bits set.
        mmu.WriteByte(CGB_SVBK, 0x03);
        Assert::AreEqual(0xFB, (int)mmu.ReadByte(CGB_SVBK));

        // D000-DFFF is the switchable half; C000-CFFF is always bank 0.
        mmu.WriteByte(0xC000, 0x11);
        mmu.WriteByte(0xD000, 0xA3);

        mmu.WriteByte(CGB_SVBK, 0x05);
        Assert::AreEqual(0xFD, (int)mmu.ReadByte(CGB_SVBK));
        mmu.WriteByte(0xD000, 0x5C);

        Assert::AreEqual(0x5C, (int)mmu.ReadByte(0xD000));
        mmu.WriteByte(CGB_SVBK, 0x03);
        Assert::AreEqual(0xA3, (int)mmu.ReadByte(0xD000));
        Assert::AreEqual(0x11, (int)mmu.ReadByte(0xC000));

        // Bank 0 is inaccessible: writing 0 selects bank 1.
        mmu.WriteByte(CGB_SVBK, 0x00);
        mmu.WriteByte(0xD000, 0x77);
        mmu.WriteByte(CGB_SVBK, 0x01);
        Assert::AreEqual(0x77, (int)mmu.ReadByte(0xD000));
    }

    TEST_METHOD(WRAMEchoFollowsBankTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::CGB);

        mmu.WriteByte(CGB_SVBK, 0x02);
        mmu.WriteByte(0xC123, 0x42);
        mmu.WriteByte(0xD123, 0x99);

        // E000-EFFF echoes bank 0, F000-FDFF echoes the *selected* bank.
        Assert::AreEqual(0x42, (int)mmu.ReadByte(0xE123));
        Assert::AreEqual(0x99, (int)mmu.ReadByte(0xF123));

        mmu.WriteByte(CGB_SVBK, 0x04);
        Assert::AreEqual(0x42, (int)mmu.ReadByte(0xE123));
        Assert::IsFalse(mmu.ReadByte(0xF123) == 0x99);

        // Writes through echo land in the same storage.
        mmu.WriteByte(0xF200, 0x5A);
        Assert::AreEqual(0x5A, (int)mmu.ReadByte(0xD200));
    }

    TEST_METHOD(DMGIgnoresWRAMBankingTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::DMG);

        // SVBK does not exist on DMG; the whole register is an open-bus read.
        mmu.WriteByte(CGB_SVBK, 0x03);
        Assert::AreEqual(0xFF, (int)mmu.ReadByte(CGB_SVBK));

        mmu.WriteByte(0xD000, 0x33);
        mmu.WriteByte(CGB_SVBK, 0x05);
        Assert::AreEqual(0x33, (int)mmu.ReadByte(0xD000));
    }

    // ------------------------------------------------------------ KEY1/speed

    TEST_METHOD(KEY1SpeedSwitchTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::CGB);

        // Only bit 0 is writable; bit 7 reports the current speed.
        Assert::AreEqual(0x7E, (int)mmu.ReadByte(CGB_KEY1));
        Assert::IsFalse(mmu.IsSpeedSwitchArmed());

        mmu.WriteByte(CGB_KEY1, 0x01);
        Assert::IsTrue(mmu.IsSpeedSwitchArmed());
        Assert::AreEqual(0x7F, (int)mmu.ReadByte(CGB_KEY1));
        Assert::IsFalse(mmu.IsDoubleSpeed());

        // STOP performs the switch; the armed bit clears and bit 7 flips.
        mmu.CompleteSpeedSwitch();
        Assert::IsTrue(mmu.IsDoubleSpeed());
        Assert::IsFalse(mmu.IsSpeedSwitchArmed());
        Assert::AreEqual(0xFE, (int)mmu.ReadByte(CGB_KEY1));

        mmu.WriteByte(CGB_KEY1, 0x01);
        mmu.CompleteSpeedSwitch();
        Assert::IsFalse(mmu.IsDoubleSpeed());
        Assert::AreEqual(0x7E, (int)mmu.ReadByte(CGB_KEY1));
    }

    TEST_METHOD(KEY1UnavailableOnDMGTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::DMG);

        mmu.WriteByte(CGB_KEY1, 0x01);
        Assert::AreEqual(0xFF, (int)mmu.ReadByte(CGB_KEY1));
        Assert::IsFalse(mmu.IsSpeedSwitchArmed());
    }

    TEST_METHOD(CGBOnlyIORegisterMasksTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::CGB);

        // OPRI: bit 0 writable, rest read as 1.
        mmu.WriteByte(CGB_OPRI, 0x00);
        Assert::AreEqual(0xFE, (int)mmu.ReadByte(CGB_OPRI));
        mmu.WriteByte(CGB_OPRI, 0x01);
        Assert::AreEqual(0xFF, (int)mmu.ReadByte(CGB_OPRI));

        // RP (infrared) has no emulated peer, so reads report "no signal".
        mmu.WriteByte(CGB_RP, 0xC1);
        Assert::AreEqual(0xC1, (int)(mmu.ReadByte(CGB_RP) & 0xC1));

        // FF75 keeps only bits 4-6.
        mmu.WriteByte(0xFF75, 0xFF);
        Assert::AreEqual(0xFF, (int)mmu.ReadByte(0xFF75));
        mmu.WriteByte(0xFF75, 0x00);
        Assert::AreEqual(0x8F, (int)mmu.ReadByte(0xFF75));

        // Unmapped I/O is open bus, not zero.
        Assert::AreEqual(0xFF, (int)mmu.ReadByte(0xFF03));
    }

    TEST_METHOD(CompatibilityModeHidesCGBRegistersTest)
    {
        MMU mmu;
        mmu.SetGameBoyMode(GameBoyMode::CGBCompatibility);

        // A DMG cartridge on CGB hardware must not see the CGB *feature* set,
        // otherwise DMG software can bank WRAM out from under itself.
        mmu.WriteByte(CGB_SVBK, 0x03);
        Assert::AreEqual(0xFF, (int)mmu.ReadByte(CGB_SVBK));
        mmu.WriteByte(0xD000, 0x33);
        mmu.WriteByte(CGB_SVBK, 0x05);
        Assert::AreEqual(0x33, (int)mmu.ReadByte(0xD000));

        // KEY1 is console-level rather than cartridge-level, so it stays
        // reachable: the silicon really is a CGB.
        mmu.WriteByte(CGB_KEY1, 0x01);
        Assert::IsTrue(mmu.IsSpeedSwitchArmed());
        mmu.CompleteSpeedSwitch();
        Assert::IsTrue(mmu.IsDoubleSpeed());
    }

    // -------------------------------------------------------------- VRAM/VBK

    TEST_METHOD(VRAMBankSelectTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        Assert::AreEqual(0xFE, (int)gpu.ReadByte(CGB_VBK));

        gpu.WriteByte(0x8000, 0x11);
        gpu.WriteByte(CGB_VBK, 0x01);
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_VBK));
        gpu.WriteByte(0x8000, 0x22);
        Assert::AreEqual(0x22, (int)gpu.ReadByte(0x8000));

        gpu.WriteByte(CGB_VBK, 0x00);
        Assert::AreEqual(0x11, (int)gpu.ReadByte(0x8000));

        // Only bit 0 selects the bank.
        gpu.WriteByte(CGB_VBK, 0xFE);
        Assert::AreEqual(0x11, (int)gpu.ReadByte(0x8000));
    }

    TEST_METHOD(DMGHasSingleVRAMBankTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::DMG);

        gpu.WriteByte(0x8000, 0x11);
        gpu.WriteByte(CGB_VBK, 0x01);
        Assert::AreEqual(0x11, (int)gpu.ReadByte(0x8000));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_VBK));
    }

    // -------------------------------------------------------------- palettes

    TEST_METHOD(PaletteAutoIncrementTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        // Bit 7 enables auto-increment; bit 6 always reads back as 1.
        gpu.WriteByte(CGB_BGPI, 0x80);
        for (int i = 0; i < 64; i++)
        {
            gpu.WriteByte(CGB_BGPD, (byte)i);
        }

        // 64 increments from index 0 wrap back to 0, with bit 7 still set.
        Assert::AreEqual(0xC0, (int)gpu.ReadByte(CGB_BGPI));

        gpu.WriteByte(CGB_BGPI, 0x10);
        Assert::AreEqual(0x50, (int)gpu.ReadByte(CGB_BGPI));
        Assert::AreEqual(0x10, (int)gpu.ReadByte(CGB_BGPD));

        // Without bit 7 the index must not move.
        gpu.WriteByte(CGB_BGPD, 0x7F);
        Assert::AreEqual(0x50, (int)gpu.ReadByte(CGB_BGPI));
        Assert::AreEqual(0x7F, (int)gpu.ReadByte(CGB_BGPD));

        // OBJ palettes are an independent index/RAM pair.
        gpu.WriteByte(CGB_OBPI, 0x80);
        gpu.WriteByte(CGB_OBPD, 0xAA);
        gpu.WriteByte(CGB_OBPD, 0xBB);
        Assert::AreEqual(0xC2, (int)gpu.ReadByte(CGB_OBPI));

        gpu.WriteByte(CGB_OBPI, 0x00);
        Assert::AreEqual(0xAA, (int)gpu.ReadByte(CGB_OBPD));
        gpu.WriteByte(CGB_OBPI, 0x01);
        Assert::AreEqual(0xBB, (int)gpu.ReadByte(CGB_OBPD));

        // BG palette RAM is untouched by OBJ writes.
        gpu.WriteByte(CGB_BGPI, 0x00);
        Assert::AreEqual(0x00, (int)gpu.ReadByte(CGB_BGPD));
    }

    TEST_METHOD(PalettesUnavailableOnDMGTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::DMG);

        gpu.WriteByte(CGB_BGPI, 0x80);
        gpu.WriteByte(CGB_BGPD, 0x12);
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_BGPI));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_BGPD));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_OBPI));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_OBPD));
    }

    // ------------------------------------------------------------ VRAM DMA

    TEST_METHOD(GeneralPurposeDMATest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        for (int i = 0; i < 0x40; i++)
        {
            mmu.m_memory[0x4000 + i] = (byte)(0x80 + i);
        }

        // Source and destination are 16-byte aligned by hardware.
        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x03);
        gpu.WriteByte(CGB_HDMA3, 0x81);
        gpu.WriteByte(CGB_HDMA4, 0x07);

        // Bit 7 clear = general purpose DMA, length = (n+1) * 16 bytes.
        gpu.WriteByte(CGB_HDMA5, 0x03);

        for (int i = 0; i < 0x40; i++)
        {
            Assert::AreEqual(0x80 + i, (int)gpu.ReadByte((ushort)(0x8100 + i)));
        }

        // A completed transfer reads back as 0xFF.
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_HDMA5));

        // GDMA stalls the CPU for 32 cycles per 16-byte block.
        Assert::AreEqual(4 * 32, (int)gpu.ConsumeDMAStallCycles());
        Assert::AreEqual(0, (int)gpu.ConsumeDMAStallCycles());
    }

    TEST_METHOD(HBlankDMATransfersOneBlockPerHBlankTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        for (int i = 0; i < 0x40; i++)
        {
            mmu.m_memory[0x4000 + i] = (byte)(0x10 + i);
        }

        gpu.WriteByte(CGB_LCDC, 0x80);
        // Land at the start of a visible line so HBlanks actually occur.
        gpu.Step(4);

        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x82);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x83);   // bit 7 set = HBlank DMA, 4 blocks

        // Nothing is copied until the first HBlank is reached.
        Assert::AreEqual(0x00, (int)gpu.ReadByte(0x8200));
        Assert::AreEqual(0x03, (int)gpu.ReadByte(CGB_HDMA5));

        StepCycles(gpu, ReadingOAMCycles + ReadingOAMVRAMCycles);
        Assert::AreEqual(0x10, (int)gpu.ReadByte(0x8200));
        Assert::AreEqual(0x0F + 0x10, (int)gpu.ReadByte(0x820F));
        Assert::AreEqual(0x00, (int)gpu.ReadByte(0x8210));
        Assert::AreEqual(0x02, (int)gpu.ReadByte(CGB_HDMA5));

        // Exactly one more block per subsequent scanline.
        StepCycles(gpu, 456);
        Assert::AreEqual(0x01, (int)gpu.ReadByte(CGB_HDMA5));
        Assert::AreEqual(0x20, (int)gpu.ReadByte(0x8210));
        Assert::AreEqual(0x00, (int)gpu.ReadByte(0x8220));

        StepCycles(gpu, 456);
        StepCycles(gpu, 456);

        for (int i = 0; i < 0x40; i++)
        {
            Assert::AreEqual(0x10 + i, (int)gpu.ReadByte((ushort)(0x8200 + i)));
        }

        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_HDMA5));
    }

    TEST_METHOD(HBlankDMATerminationWindowAbsorbsLateDisableTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        for (int i = 0; i < 0x20; i++)
        {
            mmu.m_memory[0x4000 + i] = (byte)(0x10 + i);
        }

        gpu.WriteByte(CGB_LCDC, 0x80);
        gpu.Step(4);
        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x82);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x80);

        StepCycles(gpu, ReadingOAMCycles + ReadingOAMVRAMCycles);
        Assert::AreEqual(0x10, (int)gpu.ReadByte(0x8200));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_HDMA5));

        gpu.WriteByte(CGB_HDMA5, 0x00);
        Assert::AreEqual(0x00, (int)gpu.ReadByte(0x8210));

        gpu.NotifyDMAStallPaid();
        gpu.WriteByte(CGB_HDMA5, 0x00);
        Assert::AreEqual(0x20, (int)gpu.ReadByte(0x8210));
    }

    TEST_METHOD(HBlankDMACancelTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        gpu.WriteByte(CGB_LCDC, 0x80);
        gpu.Step(4);

        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x83);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x87);

        gpu.Step(ReadingOAMCycles + ReadingOAMVRAMCycles);
        Assert::AreEqual(0x07, (int)gpu.ReadByte(CGB_HDMA5));

        StepCycles(gpu, ReadingOAMCycles + ReadingOAMVRAMCycles);
        Assert::AreEqual(0x06, (int)gpu.ReadByte(CGB_HDMA5));

        // Writing HDMA5 with bit 7 clear during an active HBlank DMA stops it.
        gpu.WriteByte(CGB_HDMA5, 0x00);
        Assert::AreEqual(0x86, (int)gpu.ReadByte(CGB_HDMA5));

        const byte before = gpu.ReadByte(0x8310);
        StepCycles(gpu, 456);
        StepCycles(gpu, 456);
        Assert::AreEqual((int)before, (int)gpu.ReadByte(0x8310));
    }

    TEST_METHOD(SpeedSwitchAbortsOnlyUnstartedHBlankDMATest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        gpu.WriteByte(CGB_LCDC, 0x80);
        gpu.Step(4);
        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x83);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x87);

        gpu.Step(ReadingOAMCycles + ReadingOAMVRAMCycles);
        StepCycles(gpu, ReadingOAMCycles + ReadingOAMVRAMCycles);
        Assert::AreEqual(0x06, (int)gpu.ReadByte(CGB_HDMA5));

        gpu.AbortHBlankDMAForSpeedSwitch();
        Assert::AreEqual(0x06, (int)gpu.ReadByte(CGB_HDMA5));
        StepCycles(gpu, ReadingOAMCycles + ReadingOAMVRAMCycles);
        Assert::AreEqual(0x05, (int)gpu.ReadByte(CGB_HDMA5));
    }

    TEST_METHOD(SpeedSwitchTerminatesUnstartedHBlankDMATest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        gpu.WriteByte(CGB_LCDC, 0x80);
        gpu.Step(4);
        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x83);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x87);

        gpu.AbortHBlankDMAForSpeedSwitch();
        Assert::AreEqual(0x87, (int)gpu.ReadByte(CGB_HDMA5));

        const byte before = gpu.ReadByte(0x8310);
        StepCycles(gpu, 456);
        StepCycles(gpu, 456);
        Assert::AreEqual((int)before, (int)gpu.ReadByte(0x8310));
    }

    TEST_METHOD(VRAMDMASourceUsesExternalBusTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        for (int i = 0; i < 0x10; i++)
        {
            mmu.m_memory[0x4000 + i] = (byte)(0x10 + i);
            mmu.m_memory[0xFF80 + i] = (byte)(0x20 + i);
        }

        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x80);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x00);
        Assert::AreEqual(0x10, (int)gpu.ReadByte(0x8000));
        Assert::AreEqual(0x1F, (int)gpu.ReadByte(0x800F));

        gpu.WriteByte(CGB_HDMA1, 0xFF);
        gpu.WriteByte(CGB_HDMA2, 0x80);
        gpu.WriteByte(CGB_HDMA3, 0x81);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x00);
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(0x8100));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(0x810F));

        gpu.WriteByte(CGB_HDMA1, 0x80);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x82);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x00);
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(0x8200));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(0x820F));
    }

    TEST_METHOD(VRAMDMAUnavailableOnDMGTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::DMG);

        mmu.m_memory[0x4000] = 0x5A;

        gpu.WriteByte(CGB_HDMA1, 0x40);
        gpu.WriteByte(CGB_HDMA2, 0x00);
        gpu.WriteByte(CGB_HDMA3, 0x80);
        gpu.WriteByte(CGB_HDMA4, 0x00);
        gpu.WriteByte(CGB_HDMA5, 0x00);

        Assert::AreEqual(0x00, (int)gpu.ReadByte(0x8000));
        Assert::AreEqual(0xFF, (int)gpu.ReadByte(CGB_HDMA5));
        Assert::AreEqual(0, (int)gpu.ConsumeDMAStallCycles());
    }

    // ------------------------------------------------------- STAT interrupt

    TEST_METHOD(STATInterruptLineIsEdgeTriggeredTest)
    {
        CGBTestMMU mmu;
        CGBTestCPU cpu;
        GPU gpu(&mmu, &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);

        gpu.WriteByte(CGB_LCDC, 0x80);
        gpu.Step(4);   // LY = 0, mode 2

        // Enable only the LY=LYC source, with the coincidence already true.
        cpu.Interrupts = 0;
        gpu.WriteByte(CGB_LYC, 0x00);
        gpu.WriteByte(CGB_STAT, 0x40);
        Assert::AreEqual(1, cpu.Interrupts);

        // Holding the same condition across mode changes must not re-fire.
        gpu.Step(ReadingOAMCycles);
        gpu.Step(ReadingOAMVRAMCycles);
        Assert::AreEqual(1, cpu.Interrupts);

        // Leaving and re-entering coincidence is a new rising edge.
        gpu.Step(HBlankCycles);
        Assert::AreEqual(1, cpu.Interrupts);
        gpu.WriteByte(CGB_LYC, 0x01);
        Assert::AreEqual(2, cpu.Interrupts);
    }

    // ---------------------------------------------------------------- serial

    TEST_METHOD(SerialFastClockTest)
    {
        CGBTestCPU dmgCPU;
        Serial dmgSerial(&dmgCPU);
        dmgSerial.SetGameBoyMode(GameBoyMode::DMG);
        dmgSerial.WriteByte(CGB_SB, 0xAA);
        dmgSerial.WriteByte(CGB_SC, 0x81);

        // 8 bits at 8192 Hz = 8 * 512 cycles.
        dmgSerial.Step(8 * 512 - 8);
        Assert::AreEqual(0, dmgCPU.Interrupts);
        dmgSerial.Step(16);
        Assert::AreEqual(1, dmgCPU.Interrupts);

        // Bit 1 of SC selects the CGB 32x clock: 8 * 16 cycles.
        CGBTestCPU cgbCPU;
        Serial cgbSerial(&cgbCPU);
        cgbSerial.SetGameBoyMode(GameBoyMode::CGB);
        cgbSerial.WriteByte(CGB_SB, 0xAA);
        cgbSerial.WriteByte(CGB_SC, 0x83);

        cgbSerial.Step(8 * 16 - 8);
        Assert::AreEqual(0, cgbCPU.Interrupts);
        cgbSerial.Step(16);
        Assert::AreEqual(1, cgbCPU.Interrupts);
    }

    TEST_METHOD(SerialFastClockIgnoredOnDMGTest)
    {
        CGBTestCPU cpu;
        Serial serial(&cpu);
        serial.SetGameBoyMode(GameBoyMode::DMG);

        // DMG has no fast clock, so bit 1 must not shorten the transfer.
        serial.WriteByte(CGB_SC, 0x83);
        serial.Step(8 * 16 + 64);
        Assert::AreEqual(0, cpu.Interrupts);
    }

    // ----------------------------------------------------------------- timer

    TEST_METHOD(TimerPostBootDividerTest)
    {
        CGBTestCPU cpu;

        Timer dmgTimer(&cpu);
        dmgTimer.SetGameBoyMode(GameBoyMode::DMG);
        dmgTimer.PreBoot();
        Assert::AreEqual(0xAB, (int)dmgTimer.ReadByte(CGB_DIV));

        Timer cgbTimer(&cpu);
        cgbTimer.SetGameBoyMode(GameBoyMode::CGB);
        cgbTimer.PreBoot();
        Assert::AreEqual(0x26, (int)cgbTimer.ReadByte(CGB_DIV));
    }

    TEST_METHOD(DoubleSpeedFrameSequencerTapTest)
    {
        // In double speed the APU's frame sequencer is driven from DIV bit 13
        // instead of bit 12, so it still ticks at 512 Hz in real time even
        // though the CPU is running twice as fast.
        CGBTestCPU normalCPU;
        Timer normalTimer(&normalCPU);
        normalTimer.SetGameBoyMode(GameBoyMode::CGB);
        normalTimer.Step(32768);
        Assert::AreEqual(4, normalCPU.FrameSequencerClocks);

        CGBTestCPU fastCPU;
        Timer fastTimer(&fastCPU);
        fastTimer.SetGameBoyMode(GameBoyMode::CGB);
        fastTimer.SetDoubleSpeed(true);
        fastTimer.Step(32768);
        Assert::AreEqual(2, fastCPU.FrameSequencerClocks);
    }

    // ------------------------------------------------------------------- APU

    TEST_METHOD(PCMRegistersTest)
    {
        APU cgbAPU;
        cgbAPU.SetGameBoyMode(GameBoyMode::CGB);

        // Powered off, the amplitude taps read zero rather than open bus.
        Assert::AreEqual(0x00, (int)cgbAPU.ReadByte(CGB_PCM12));
        Assert::AreEqual(0x00, (int)cgbAPU.ReadByte(CGB_PCM34));

        // They are read-only.
        cgbAPU.WriteByte(CGB_PCM12, 0xFF);
        Assert::AreEqual(0x00, (int)cgbAPU.ReadByte(CGB_PCM12));

        // The registers do not exist at all on DMG.
        APU dmgAPU;
        dmgAPU.SetGameBoyMode(GameBoyMode::DMG);
        Assert::AreEqual(0xFF, (int)dmgAPU.ReadByte(CGB_PCM12));
        Assert::AreEqual(0xFF, (int)dmgAPU.ReadByte(CGB_PCM34));
    }

    TEST_METHOD(PCMReportsChannelAmplitudeTest)
    {
        APU apu;
        apu.SetGameBoyMode(GameBoyMode::CGB);
        apu.WriteByte(CGB_NR52, 0x80);

        // Fill wave RAM with a constant so CH3's amplitude is predictable.
        for (ushort address = 0xFF30; address <= 0xFF3F; address++)
        {
            apu.WriteByte(address, 0xFF);
        }

        apu.WriteByte(CGB_NR30, 0x80);   // DAC on
        apu.WriteByte(CGB_NR32, 0x20);   // 100% volume
        apu.WriteByte(CGB_NR33, 0xFF);   // frequency 2047
        apu.WriteByte(CGB_NR34, 0x87);   // trigger, no length enable
        apu.Step(8);                     // initial period + six-cycle fetch delay

        Assert::IsTrue(apu.IsChannelEnabled(2));
        Assert::AreEqual(15, (int)apu.GetChannelAmplitude(2));
        Assert::AreEqual(0x00, (int)apu.ReadByte(CGB_PCM12));

        const byte pcm34 = apu.ReadByte(CGB_PCM34);
        Assert::AreEqual(15, (int)(pcm34 & 0x0F));
        Assert::AreEqual((int)apu.GetChannelAmplitude(3), (int)(pcm34 >> 4));
    }

    TEST_METHOD(PoweredOffLengthWriteIsCGBBlockedTest)
    {
        /*
            On DMG the length counters stay writable while the APU is powered
            off; on CGB the whole register file is inert. The observable
            difference is whether a length loaded during power-off survives to
            expire after power-on (blargg dmg_sound/cgb_sound 11).
        */
        APU dmgAPU;
        dmgAPU.SetGameBoyMode(GameBoyMode::DMG);
        dmgAPU.WriteByte(CGB_NR52, 0x00);
        dmgAPU.WriteByte(CGB_NR31, 0xFF);   // length = 1 tick
        dmgAPU.WriteByte(CGB_NR52, 0x80);
        dmgAPU.WriteByte(CGB_NR30, 0x80);
        dmgAPU.WriteByte(CGB_NR34, 0xC0);   // trigger with length enable
        dmgAPU.Step(8192);
        Assert::IsFalse(dmgAPU.IsChannelEnabled(2));

        APU cgbAPU;
        cgbAPU.SetGameBoyMode(GameBoyMode::CGB);
        cgbAPU.WriteByte(CGB_NR52, 0x00);
        cgbAPU.WriteByte(CGB_NR31, 0xFF);   // ignored on CGB
        cgbAPU.WriteByte(CGB_NR52, 0x80);
        cgbAPU.WriteByte(CGB_NR30, 0x80);
        cgbAPU.WriteByte(CGB_NR34, 0xC0);
        cgbAPU.Step(8192);
        Assert::IsTrue(cgbAPU.IsChannelEnabled(2));
    }

    TEST_METHOD(PowerOffClearsLengthCountersOnCGBTest)
    {
        APU dmgAPU;
        dmgAPU.SetGameBoyMode(GameBoyMode::DMG);
        dmgAPU.WriteByte(CGB_NR52, 0x80);
        dmgAPU.WriteByte(CGB_NR11, 0x3F);
        dmgAPU.WriteByte(CGB_NR21, 0x3F);
        dmgAPU.WriteByte(CGB_NR31, 0xFF);
        dmgAPU.WriteByte(CGB_NR41, 0x3F);
        dmgAPU.WriteByte(CGB_NR52, 0x00);
        dmgAPU.WriteByte(CGB_NR52, 0x80);
        dmgAPU.WriteByte(CGB_NR12, 0xF0);
        dmgAPU.WriteByte(CGB_NR14, 0xC0);
        dmgAPU.WriteByte(CGB_NR22, 0xF0);
        dmgAPU.WriteByte(CGB_NR24, 0xC0);
        dmgAPU.WriteByte(CGB_NR30, 0x80);
        dmgAPU.WriteByte(CGB_NR34, 0xC0);
        dmgAPU.WriteByte(CGB_NR42, 0xF0);
        dmgAPU.WriteByte(CGB_NR44, 0xC0);
        dmgAPU.Step(8192);
        Assert::IsFalse(dmgAPU.IsChannelEnabled(0));
        Assert::IsFalse(dmgAPU.IsChannelEnabled(1));
        Assert::IsFalse(dmgAPU.IsChannelEnabled(2));
        Assert::IsFalse(dmgAPU.IsChannelEnabled(3));

        APU cgbAPU;
        cgbAPU.SetGameBoyMode(GameBoyMode::CGB);
        cgbAPU.WriteByte(CGB_NR52, 0x80);
        cgbAPU.WriteByte(CGB_NR11, 0x3F);
        cgbAPU.WriteByte(CGB_NR21, 0x3F);
        cgbAPU.WriteByte(CGB_NR31, 0xFF);
        cgbAPU.WriteByte(CGB_NR41, 0x3F);
        cgbAPU.WriteByte(CGB_NR52, 0x00);
        cgbAPU.WriteByte(CGB_NR52, 0x80);
        cgbAPU.WriteByte(CGB_NR12, 0xF0);
        cgbAPU.WriteByte(CGB_NR14, 0xC0);
        cgbAPU.WriteByte(CGB_NR22, 0xF0);
        cgbAPU.WriteByte(CGB_NR24, 0xC0);
        cgbAPU.WriteByte(CGB_NR30, 0x80);
        cgbAPU.WriteByte(CGB_NR34, 0xC0);
        cgbAPU.WriteByte(CGB_NR42, 0xF0);
        cgbAPU.WriteByte(CGB_NR44, 0xC0);
        cgbAPU.Step(8192);
        Assert::IsTrue(cgbAPU.IsChannelEnabled(0));
        Assert::IsTrue(cgbAPU.IsChannelEnabled(1));
        Assert::IsTrue(cgbAPU.IsChannelEnabled(2));
        Assert::IsTrue(cgbAPU.IsChannelEnabled(3));
    }

    TEST_METHOD(CGBWaveRAMIsAccessibleWhileChannelIsOnTest)
    {
        APU cgbAPU;
        cgbAPU.SetGameBoyMode(GameBoyMode::CGB);
        cgbAPU.WriteByte(CGB_NR52, 0x80);
        for (ushort i = 0; i < 16; i++)
        {
            cgbAPU.WriteByte(0xFF30 + i, (byte)(0xA0 | i));
        }

        cgbAPU.WriteByte(CGB_NR30, 0x80);
        cgbAPU.WriteByte(CGB_NR32, 0x20);
        cgbAPU.WriteByte(CGB_NR34, 0x80);
        Assert::IsTrue(cgbAPU.IsChannelEnabled(2));

        byte first = cgbAPU.ReadByte(0xFF30);
        Assert::IsFalse(first == 0xFF);
        for (ushort i = 1; i < 16; i++)
        {
            Assert::AreEqual((int)first, (int)cgbAPU.ReadByte(0xFF30 + i));
        }

        cgbAPU.WriteByte(0xFF30, 0x5C);
        Assert::AreEqual(0x5C, (int)cgbAPU.ReadByte(0xFF30));
    }

    TEST_METHOD(DMGWaveRAMIsOpenBusWhileChannelIsOnTest)
    {
        APU dmgAPU;
        dmgAPU.SetGameBoyMode(GameBoyMode::DMG);
        dmgAPU.WriteByte(CGB_NR52, 0x80);
        for (ushort i = 0; i < 16; i++)
        {
            dmgAPU.WriteByte(0xFF30 + i, (byte)(0xA0 | i));
        }

        dmgAPU.WriteByte(CGB_NR30, 0x80);
        dmgAPU.WriteByte(CGB_NR32, 0x20);
        dmgAPU.WriteByte(CGB_NR34, 0x80);
        Assert::IsTrue(dmgAPU.IsChannelEnabled(2));

        dmgAPU.Step(64);
        Assert::AreEqual(0xFF, (int)dmgAPU.ReadByte(0xFF35));
    }
};
