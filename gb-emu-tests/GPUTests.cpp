#include "stdafx.h"

#include <GPU.hpp>

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
    };

public:
    TEST_METHOD(GPUCycleTest)
    {
        std::unique_ptr<GPUTestsMMU> spMMU = std::unique_ptr<GPUTestsMMU>(new GPUTestsMMU(nullptr, 0));
        std::unique_ptr<GPU> spGPU = std::unique_ptr<GPU>(new GPU(spMMU.get(), nullptr));

        Assert::AreEqual(VBlankCycles, (int)spGPU->m_ModeClock);
        Assert::AreEqual(ModeVBlank, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
        Assert::IsFalse(ISBITSET(spGPU->m_LCDControl, 7));
        spGPU->Step(4);

        // LCD is off, nothing happens:
        spGPU->Step(4);
        Assert::AreEqual(VBlankCycles, (int)spGPU->m_ModeClock);
        Assert::AreEqual(ModeVBlank, (int)(spGPU->ReadByte(LCDControllerStatus) & 0x03));
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
};
