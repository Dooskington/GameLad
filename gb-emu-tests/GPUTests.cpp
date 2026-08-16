#include "stdafx.h"

#include <GPU.hpp>

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

                for (int cycles = spGPU->m_ModeClock; cycles < ReadingOAMVRAMCycles; cycles += 4)
                {
                    Assert::AreEqual(ModeReadingOAMVRAM, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
                    spGPU->Step(4);
                }

                for (int cycles = spGPU->m_ModeClock; cycles < HBlankCycles; cycles += 4)
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
