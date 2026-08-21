#include "stdafx.h"

#include <GPU.hpp>
#include <StateSerializer.hpp>

#include <vector>

class GPUTestCPU : public ICPU
{
public:
    GPUTestCPU() : InterruptCount(0), QueuedInterruptCount(0) {}

    bool Initialize() { return true; }
    bool LoadROM(const char*, const char*) { return true; }
    int Step() { return 0; }
    void TriggerInterrupt(byte) { InterruptCount++; }
    void QueueInterrupt(byte) { QueuedInterruptCount++; }
    byte* GetCurrentFrame() { return nullptr; }
    void SetInput(byte, byte) {}
    void SetVSyncCallback(void(*)()) {}
    void SetAudioSampleRate(unsigned int) {}
    size_t ConsumeAudioSamples(float*, size_t) { return 0; }
    void ClockAPUFrameSequencer() {}

    int InterruptCount;
    int QueuedInterruptCount;
};

TEST_CLASS(GPUTests)
{
private:
    // This is a test MMU for use by the GPUTests
    class GPUTestsMMU : public IMMU
    {
    public:
        GPUTestsMMU(byte* memory, int size)
        {
            memset(m_data, 0x00, ARRAYSIZE(m_data));
            if (memory != nullptr)
            {
                memcpy(m_data, memory, size);
            }
        }

        ~GPUTestsMMU()
        {
        }

        bool Initialize()
        {
            // Nothing to do
            return true;
        }

        void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit)
        {
            // Ignore registration, we got this.
        }

        unsigned short ReadUShort(const ushort& address)
        {
            ushort val = Read(address + 1);
            val = val << 8;
            val |= Read(address);
            return val;
        }

        bool LoadBootROM(const char* bootROMPath)
        {
            return true;
        }

        void SetGameBoyMode(GameBoyMode mode) { m_mode = mode; }
        GameBoyMode GetGameBoyMode() const { return m_mode; }
        bool IsSpeedSwitchArmed() const { return false; }
        void CompleteSpeedSwitch() { }
        bool IsDoubleSpeed() const { return m_doubleSpeed; }
        void SetDoubleSpeed(bool doubleSpeed) { m_doubleSpeed = doubleSpeed; }

        byte Read(const ushort& address)
        {
            return m_data[address];
        }

        bool Write(const ushort& address, const byte val)
        {
            m_data[address] = val;
            return true;
        }

    private:
        byte m_data[0xFFFF + 1];
        GameBoyMode m_mode = GameBoyMode::DMG;
        bool m_doubleSpeed = false;
    };

public:
    TEST_METHOD(GPUCycleTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU = std::unique_ptr<GPUTestsMMU>(new GPUTestsMMU(nullptr, 0));
        std::unique_ptr<GPU> spGPU = std::unique_ptr<GPU>(new GPU(spMMU.get(), nullptr));

        // On real hardware, while the LCD is disabled, LY reads 0 and STAT
        // reports mode 0 (PPU-001).
        Assert::AreEqual(0, (int)spGPU->m_ModeClock);
        Assert::AreEqual(ModeHBlank, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
        Assert::AreEqual(0, (int)spGPU->m_LCDControllerYCoordinate);
        Assert::IsFalse(ISBITSET(spGPU->m_LCDControl, 7));
        spGPU->Step(4);

        // LCD is off, nothing happens:
        spGPU->Step(4);
        Assert::AreEqual(0, (int)spGPU->m_ModeClock);
        Assert::AreEqual(ModeHBlank, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
        Assert::AreEqual(0, (int)spGPU->m_LCDControllerYCoordinate);
        Assert::IsFalse(ISBITSET(spGPU->m_LCDControl, 7));

        // Endable LCD
        Assert::IsTrue(spGPU->WriteByte(LCDControl, 0x80));
        Assert::IsTrue(ISBITSET(spGPU->m_LCDControl, 7));
        spGPU->Step(4);
        Assert::AreEqual(4, (int)spGPU->m_ModeClock);

        for (int line = 0; line < 154; line++)
        {
            Assert::AreEqual(line, (int)spGPU->m_LCDControllerYCoordinate);

            if (line < 144) // Drawing lines [0-143]
            {
                for (int cycles = spGPU->m_ModeClock; cycles < ReadingOAMCycles; cycles += 4)
                {
                    Assert::AreEqual(ModeReadingOAM, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
                    spGPU->Step(4);
                }

                const int mode3Cycles = (int)spGPU->m_Mode3Cycles;
                for (int cycles = spGPU->m_ModeClock; cycles < mode3Cycles; cycles += 4)
                {
                    Assert::AreEqual(ModeReadingOAMVRAM, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
                    spGPU->Step(4);
                }

                const int mode0Cycles = (int)spGPU->m_Mode0Cycles;
                for (int cycles = spGPU->m_ModeClock; cycles < mode0Cycles; cycles += 4)
                {
                    Assert::AreEqual(ModeHBlank, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
                    spGPU->Step(4);
                }
            }
            else    // VBlank [144-153]
            {
                for (int cycles = spGPU->m_ModeClock; cycles < VBlankCycles; cycles += 4)
                {
                    Assert::AreEqual(ModeVBlank, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
                    spGPU->Step(4);
                }
            }
        }

        // By the time we get here, we should be back at the start
        Assert::AreEqual(ModeReadingOAM, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
        Assert::AreEqual(0, (int)spGPU->m_LCDControllerYCoordinate);

        spGPU.reset();
        spMMU.reset();
    }

    TEST_METHOD(Line153LYResetTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU gpu(spMMU.get(), nullptr);

        gpu.m_LCDControl = 0x80;
        gpu.m_LCDControllerYCoordinate = 153;
        gpu.m_ModeClock = 0;
        gpu.m_Line153LYReset = false;
        gpu.m_LCDControllerStatus = ModeVBlank;

        gpu.Step(4);
        Assert::AreEqual(153, (int)gpu.ReadByte(LCDControllerYCoordinate));

        gpu.Step(4);
        Assert::AreEqual(0, (int)gpu.ReadByte(LCDControllerYCoordinate));
        Assert::AreEqual(ModeVBlank, (int)(gpu.ReadByte(LCDControllerStatus) & 0x03));
    }

    TEST_METHOD(DMABusConflictTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU mainBusDMA(spMMU.get(), nullptr);

        mainBusDMA.LaunchDMATransfer(0x00);
        mainBusDMA.Step(8);
        Assert::IsTrue(mainBusDMA.IsCPUAddressBlockedByOAMDMA(0x0000));
        Assert::IsTrue(mainBusDMA.IsCPUAddressBlockedByOAMDMA(0xC000));
        Assert::IsFalse(mainBusDMA.IsCPUAddressBlockedByOAMDMA(0x8000));
        Assert::IsFalse(mainBusDMA.IsCPUAddressBlockedByOAMDMA(0xFF4B));
        Assert::IsFalse(mainBusDMA.IsCPUAddressBlockedByOAMDMA(0xFF80));
        Assert::IsTrue(mainBusDMA.IsCPUAddressBlockedByOAMDMA(0xFE00));
        mainBusDMA.Step(4);
        Assert::AreEqual(0, (int)mainBusDMA.GetOAMDMASnoopByte());

        GPU videoBusDMA(spMMU.get(), nullptr);
        videoBusDMA.LaunchDMATransfer(0x80);
        videoBusDMA.Step(8);
        Assert::IsFalse(videoBusDMA.IsCPUAddressBlockedByOAMDMA(0x0000));
        Assert::IsTrue(videoBusDMA.IsCPUAddressBlockedByOAMDMA(0x8000));
        Assert::IsFalse(videoBusDMA.IsCPUAddressBlockedByOAMDMA(0xC000));
    }

    TEST_METHOD(OAMDMADoubleSpeedClockDomainTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU gpu(spMMU.get(), nullptr);
        gpu.SetGameBoyMode(GameBoyMode::CGB);
        gpu.m_LCDControl = 0x00;

        gpu.LaunchDMATransfer(0xC0);
        gpu.Step(4, 8);
        Assert::IsTrue(gpu.IsOAMDMAActive());
        Assert::AreEqual(0, (int)gpu.m_DMAOffset);

        for (int byte = 0; byte < OAMDMABytes - 1; byte++)
        {
            gpu.Step(2, 4);
        }

        Assert::IsTrue(gpu.IsOAMDMAActive());
        Assert::AreEqual(OAMDMABytes - 1, (int)gpu.m_DMAOffset);

        gpu.Step(2, 4);
        Assert::IsFalse(gpu.IsOAMDMAActive());
        Assert::AreEqual(OAMDMABytes, (int)gpu.m_DMAOffset);
    }

    TEST_METHOD(VBlankInterruptTimingTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));

        GPUTestCPU doubleSpeedCPU;
        GPU doubleSpeed(spMMU.get(), &doubleSpeedCPU);
        doubleSpeed.SetGameBoyMode(GameBoyMode::CGB);
        doubleSpeed.m_LCDControl = 0x80;
        doubleSpeed.m_LCDControllerStatus = ModeHBlank;
        doubleSpeed.m_LCDControllerYCoordinate = 143;
        doubleSpeed.m_ModeClock = HBlankCycles - 2;
        doubleSpeed.m_Mode0Cycles = HBlankCycles;

        doubleSpeed.Step(2, 4);
        Assert::AreEqual(144, (int)doubleSpeed.m_LCDControllerYCoordinate);
        Assert::AreEqual(ModeVBlank, (int)(doubleSpeed.m_LCDControllerStatus & 0x03));
        Assert::AreEqual(0, doubleSpeedCPU.InterruptCount);
        Assert::AreEqual(
            CGBDoubleSpeedVBlankInterruptDelayCycles,
            (int)doubleSpeed.m_VBlankInterruptDelayCycles);

        doubleSpeed.Step(2, 4);
        Assert::AreEqual(0, doubleSpeedCPU.InterruptCount);
        Assert::AreEqual(2, (int)doubleSpeed.m_VBlankInterruptDelayCycles);

        doubleSpeed.Step(2, 4);
        Assert::AreEqual(1, doubleSpeedCPU.InterruptCount);
        Assert::AreEqual(0, (int)doubleSpeed.m_VBlankInterruptDelayCycles);

        doubleSpeed.Step(2, 4);
        Assert::AreEqual(1, doubleSpeedCPU.InterruptCount);

        GPUTestCPU singleSpeedCPU;
        GPU singleSpeed(spMMU.get(), &singleSpeedCPU);
        singleSpeed.SetGameBoyMode(GameBoyMode::CGB);
        singleSpeed.m_LCDControl = 0x80;
        singleSpeed.m_LCDControllerStatus = ModeHBlank;
        singleSpeed.m_LCDControllerYCoordinate = 143;
        singleSpeed.m_ModeClock = HBlankCycles - 4;
        singleSpeed.m_Mode0Cycles = HBlankCycles;

        singleSpeed.Step(4, 4);
        Assert::AreEqual(144, (int)singleSpeed.m_LCDControllerYCoordinate);
        Assert::AreEqual(1, singleSpeedCPU.InterruptCount);
        Assert::AreEqual(0, (int)singleSpeed.m_VBlankInterruptDelayCycles);
    }

    TEST_METHOD(PendingVBlankInterruptCancelledWhenLCDDisabledTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPUTestCPU cpu;
        GPU gpu(spMMU.get(), &cpu);
        gpu.SetGameBoyMode(GameBoyMode::CGB);
        gpu.m_LCDControl = 0x80;
        gpu.m_LCDControllerStatus = ModeHBlank;
        gpu.m_LCDControllerYCoordinate = 143;
        gpu.m_ModeClock = HBlankCycles - 2;
        gpu.m_Mode0Cycles = HBlankCycles;

        gpu.Step(2, 4);
        Assert::AreEqual(
            CGBDoubleSpeedVBlankInterruptDelayCycles,
            (int)gpu.m_VBlankInterruptDelayCycles);

        gpu.WriteByte(LCDControl, 0x00);
        Assert::AreEqual(0, (int)gpu.m_VBlankInterruptDelayCycles);
        gpu.Step(2, 4);
        gpu.Step(2, 4);
        Assert::AreEqual(0, cpu.InterruptCount);
    }

    TEST_METHOD(PendingVBlankInterruptStateRoundTripTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU source(spMMU.get(), nullptr);
        source.SetGameBoyMode(GameBoyMode::CGB);
        source.m_LCDControl = 0x80;
        source.m_LCDControllerStatus = ModeVBlank;
        source.m_LCDControllerYCoordinate = 144;
        source.m_VBlankInterruptDelayCycles = 2;

        std::vector<byte> serialized;
        StateSerializer writer(serialized);
        source.Serialize(writer);
        Assert::IsTrue(writer.IsValid());

        GPUTestCPU cpu;
        GPU restored(spMMU.get(), &cpu);
        StateSerializer reader(serialized.data(), serialized.size());
        restored.Serialize(reader);
        Assert::IsTrue(reader.IsValid());
        Assert::AreEqual(0, (int)reader.Remaining());
        Assert::AreEqual(2, (int)restored.m_VBlankInterruptDelayCycles);

        restored.Step(2, 4);
        Assert::AreEqual(1, cpu.InterruptCount);
        Assert::AreEqual(0, (int)restored.m_VBlankInterruptDelayCycles);
    }

    TEST_METHOD(LCDEnableFirstLineTimingTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU gpu(spMMU.get(), nullptr);

        gpu.WriteByte(LCDControl, 0x80);
        Assert::IsTrue(gpu.m_FirstLineAfterLCDEnable);

        for (int cycles = 0; cycles < 448; cycles += 4)
        {
            gpu.Step(4);
        }
        Assert::AreEqual(0, (int)gpu.ReadByte(LCDControllerYCoordinate));

        gpu.Step(4);
        Assert::AreEqual(1, (int)gpu.ReadByte(LCDControllerYCoordinate));
        Assert::IsFalse(gpu.m_FirstLineAfterLCDEnable);
    }

    TEST_METHOD(OAMCorruptionPatternTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU gpu(spMMU.get(), nullptr);

        for (int i = 0; i < OAMDMABytes; i++)
        {
            gpu.m_OAM[i] = (byte)i;
        }

        gpu.m_LCDControl = 0x80;
        gpu.m_LCDControllerStatus = ModeReadingOAM;
        gpu.m_ModeClock = 8;
        gpu.m_FirstLineAfterLCDEnable = false;

        gpu.WriteOAMWord(3, 0, 0xAAAA);
        gpu.WriteOAMWord(2, 0, 0xCCCC);
        gpu.WriteOAMWord(2, 2, 0xF0F0);
        gpu.TriggerOAMBug(
            0xFE00,
            OAMBugAccess::Write,
            OAMBugOrigin::AddressBus);

        Assert::AreEqual(0xE8E8, (int)gpu.ReadOAMWord(3, 0));
        for (int i = 2; i < 8; i++)
        {
            Assert::AreEqual(
                (int)gpu.m_OAM[2 * 8 + i],
                (int)gpu.m_OAM[3 * 8 + i]);
        }

        for (int i = 0; i < OAMDMABytes; i++)
        {
            gpu.m_OAM[i] = (byte)i;
        }

        gpu.m_ModeClock = 12;
        gpu.WriteOAMWord(3, 0, 0xAAAA);
        gpu.WriteOAMWord(2, 0, 0x0F0F);
        gpu.WriteOAMWord(2, 2, 0xF0F0);
        gpu.TriggerOAMBug(
            0xFE00,
            OAMBugAccess::Read,
            OAMBugOrigin::MemoryBus);

        Assert::AreEqual(0xAFAF, (int)gpu.ReadOAMWord(2, 0));
        for (int i = 0; i < 8; i++)
        {
            Assert::AreEqual(
                (int)gpu.m_OAM[2 * 8 + i],
                (int)gpu.m_OAM[3 * 8 + i]);
        }

        const ushort row3Word = gpu.ReadOAMWord(3, 0);
        gpu.SetGameBoyMode(GameBoyMode::CGB);
        gpu.TriggerOAMBug(
            0xFE00,
            OAMBugAccess::Read,
            OAMBugOrigin::MemoryBus);
        Assert::AreEqual((int)row3Word, (int)gpu.ReadOAMWord(3, 0));
    }

    TEST_METHOD(WindowRightEdgeTimingTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU gpu(spMMU.get(), nullptr);

        gpu.m_LCDControl = 0xA0;
        gpu.m_LCDControllerYCoordinate = 0;
        gpu.m_WindowYPosition = 0;
        gpu.m_WindowXPositionMinus7 = 166;
        gpu.m_ScrollX = 0;
        gpu.m_lineSpriteCount = 0;
        gpu.ComputeScanlineTiming();

        Assert::AreEqual(ReadingOAMVRAMCycles + 1, (int)gpu.m_Mode3Cycles);
        Assert::AreEqual(
            CyclesPerScanline - ReadingOAMCycles - ReadingOAMVRAMCycles - 1,
            (int)gpu.m_Mode0Cycles);
    }

    TEST_METHOD(CGBWindowYLatchesWhileDisabledTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU gpu(spMMU.get(), nullptr);
        gpu.SetGameBoyMode(GameBoyMode::CGB);
        gpu.m_LCDControl = 0x80;
        gpu.m_LCDControllerYCoordinate = 7;
        gpu.m_WindowYPosition = 7;

        gpu.BeginPixelTransfer();

        Assert::IsTrue(gpu.m_WindowYTriggered);
        Assert::IsFalse(gpu.m_WindowEligibleThisLine);
    }

    TEST_METHOD(CGBWindowEnableDeadlineTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPU early(spMMU.get(), nullptr);
        early.SetGameBoyMode(GameBoyMode::CGB);
        early.m_LCDControl = 0x80;
        early.m_WindowYTriggered = true;
        early.m_WindowXPositionMinus7 = 7;
        early.BeginPixelTransfer();
        early.AdvancePixelTransfer(10);
        early.m_LCDControl = 0xA0;
        early.AdvancePixelTransfer(3);
        Assert::IsTrue(early.m_WindowTriggeredThisLine);

        GPU late(spMMU.get(), nullptr);
        late.SetGameBoyMode(GameBoyMode::CGB);
        late.m_LCDControl = 0x80;
        late.m_WindowYTriggered = true;
        late.m_WindowXPositionMinus7 = 7;
        late.BeginPixelTransfer();
        late.AdvancePixelTransfer(12);
        late.m_LCDControl = 0xA0;
        late.AdvancePixelTransfer(1);
        Assert::IsFalse(late.m_WindowTriggeredThisLine);
    }

    TEST_METHOD(InternalMode2STATEventTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU(new GPUTestsMMU(nullptr, 0));
        GPUTestCPU cpu;
        GPU gpu(spMMU.get(), &cpu);

        gpu.m_LCDControl = 0x80;
        gpu.m_LCDControllerStatus = 0x60 | ModeHBlank;
        gpu.m_LCDControllerYCoordinate = 1;
        gpu.m_LYCompare = 0;
        gpu.m_ModeClock = HBlankCycles - 8;
        gpu.m_Mode0Cycles = HBlankCycles;
        gpu.m_StatInterruptLine = false;
        gpu.m_InternalMode2STATEventFired = false;

        gpu.Step(4);

        Assert::AreEqual(
            ModeHBlank,
            (int)(gpu.ReadByte(LCDControllerStatus) & 0x03));
        Assert::AreEqual(0, cpu.InterruptCount);
        Assert::AreEqual(1, cpu.QueuedInterruptCount);
    }
};
