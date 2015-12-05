#include "stdafx.h"

#include <CPU.hpp>

TEST_CLASS(CPUTests)
{
private:
    // This is a test MMU for use by the CPUTests
    class CPUTestsMMU : public IMMU
    {
    public:
        CPUTestsMMU(byte* memory, int size)
        {
            memset(m_data, 0x00, ARRAYSIZE(m_data));
            if (memory != nullptr)
            {
                memcpy(m_data, memory, size);
            }
        }

        ~CPUTestsMMU()
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
    // Misc tests
    TEST_METHOD(Timing_Test)
    {
        byte opTimes[] = 
        {
            1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
            0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
            2,3,2,2,1,1,2,1,2,2,2,2,1,1,2,1,
            2,3,2,2,3,3,3,1,2,2,2,2,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            2,3,3,4,3,4,2,4,2,4,3,0,3,6,2,4,
            2,3,3,0,3,4,2,4,2,4,3,0,3,0,2,4,
            3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
            3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4
        };

        byte condOpTimes[] =
        {
            1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
            0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
            3,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
            3,3,2,2,3,3,3,1,3,2,2,2,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
            5,3,4,4,6,4,2,4,5,4,4,0,6,6,2,4,
            5,3,4,0,6,4,2,4,5,4,4,0,6,0,2,4,
            3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
            3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4
        };

        byte cbOpTimes[] =
        {
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
            2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
            2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
            2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
            2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2
        };

        for (int opCode = 0x00;opCode <= 0xFF; opCode++)
        {
            byte m_Mem[] = { (byte)(opCode) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            if (spCPU->m_operationMap[opCode] == nullptr)
                continue;

            spCPU->Step();

            if (opTimes[opCode] == condOpTimes[opCode])
            {
                Assert::AreEqual((int)opTimes[opCode] * 4, (int)spCPU->m_cycles);
            }
            else
            {
                if (opTimes[opCode] * 4 != (int)spCPU->m_cycles)
                {
                    Assert::AreEqual((int)condOpTimes[opCode] * 4, (int)spCPU->m_cycles);
                }
            }
        }

        for (int opCode = 0x00;opCode <= 0xFF; opCode++)
        {
            byte m_Mem[] = { (byte)(opCode) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            if (spCPU->m_operationMap[opCode] == nullptr)
                continue;

            spCPU->m_AF = 0x00FF;
            spCPU->Step();

            if (opTimes[opCode] == condOpTimes[opCode])
            {
                Assert::AreEqual((int)opTimes[opCode] * 4, (int)spCPU->m_cycles);
            }
            else
            {
                if (opTimes[opCode] * 4 != (int)spCPU->m_cycles)
                {
                    Assert::AreEqual((int)condOpTimes[opCode] * 4, (int)spCPU->m_cycles);
                }
            }
        }

        for (int opCode = 0x00;opCode <= 0xFF; opCode++)
        {
            byte m_Mem[] = { 0xCB, (byte)(opCode) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            if (spCPU->m_operationMapCB[opCode] == nullptr)
                continue;

            spCPU->Step();

            Assert::AreEqual((int)cbOpTimes[opCode] * 4, (int)spCPU->m_cycles);
        }

    }

    TEST_METHOD(Endian_Test)
    {
        ushort value = 0x1234; // [0x34 0x12]
        Assert::AreEqual(0x12, (int)*(reinterpret_cast<byte*>(&value) + 1));
        Assert::AreEqual(0x34, (int)*(reinterpret_cast<byte*>(&value)));
    }

    TEST_METHOD(GetHighByte_Test)
    {
        ushort value = 0x1234;
        Assert::AreEqual(0x12, (int)CPU::GetHighByte(value));
    }

    TEST_METHOD(GetLowByte_Test)
    {
        ushort value = 0x1234;
        Assert::AreEqual(0x34, (int)CPU::GetLowByte(value));
    }

    TEST_METHOD(GetByteRegister_Test)
    {
        int value = 0x12;
        for (byte flags = 0x00; flags <= 0x07;flags++, value++)
        {
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            byte* reg = spCPU->GetByteRegister(flags);

            *reg = value;

            switch (flags)
            {
            case 0x00:  // B
                Assert::AreEqual(value << 8, (int)spCPU->m_BC);
                break;
            case 0x01:  // C
                Assert::AreEqual(value, (int)spCPU->m_BC);
                break;
            case 0x02:  // D
                Assert::AreEqual(value << 8, (int)spCPU->m_DE);
                break;
            case 0x03:  // E
                Assert::AreEqual(value, (int)spCPU->m_DE);
                break;
            case 0x04:  // H
                Assert::AreEqual(value << 8, (int)spCPU->m_HL);
                break;
            case 0x05:  // L
                Assert::AreEqual(value, (int)spCPU->m_HL);
                break;
            case 0x07:  // A
                Assert::AreEqual(value << 8, (int)spCPU->m_AF);
                break;
            }

            spCPU.reset();
        }
    }

    TEST_METHOD(GetUShortRegister_Test)
    {
        int value = 0x1234;
        for (byte flags = 0x00; flags <= 0x03;flags++, value++)
        {
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            ushort* reg = spCPU->GetUShortRegister(flags, false);

            *reg = (int)value;

            switch (flags)
            {
            case 0x00:  // BC
                Assert::AreEqual(value, (int)spCPU->m_BC);
                break;
            case 0x01:  // DE
                Assert::AreEqual(value, (int)spCPU->m_DE);
                break;
            case 0x02:  // HL
                Assert::AreEqual(value, (int)spCPU->m_HL);
                break;
            case 0x03:  // SP
                Assert::AreEqual(value, (int)spCPU->m_SP);
                break;
            }

            spCPU.reset();
        }
    }

    // OpCode Test

    TEST_METHOD(ANDr_Test)
    {
        // Test for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)(0xA0 | reg) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_AF = 0x0000;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0x0100;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x0001;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0x0100;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x0001;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0x0100;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x0001;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0x0100;
                break;
            }

            spCPU->Step();

            byte result = spCPU->GetHighByte(spCPU->m_AF);

            if (reg == 0x07)
            {
                Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
                Assert::AreEqual(0x01, (int)result);
            }
            else
            {
                Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
                Assert::AreEqual(0x00, (int)result);
            }

            // Verify expectations after
            Assert::AreEqual(4, (int)spCPU->m_cycles);
            Assert::AreEqual(1, (int)spCPU->m_PC);

            spCPU.reset();
        }
    }

    // 0xA6
    TEST_METHOD(AND_HL__Test)
    {
        byte m_Mem[] = { 0xA6, 0xA6 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x0000;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x01);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0, (int)spCPU->m_PC);

        spCPU->Step();

        byte result = spCPU->GetHighByte(spCPU->m_AF);

        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
        Assert::AreEqual(0x00, (int)result);

        spCPU->m_AF = 0x0500;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x05);

        spCPU->Step();

        result = spCPU->GetHighByte(spCPU->m_AF);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
        Assert::AreEqual(0x05, (int)result);

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(2, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // LD r, R
    TEST_METHOD(LDrR_Test)
    {
        // Test LD r, R for each register (Except F, of course)
        for (byte left = 0x00; left <= 0x07; left++)
        {
            for (byte right = 0x00; right <= 0x07; right++)
            {
                if (left == 0x06 || right == 0x06) continue;

                byte m_Mem[] = { (byte)(0x40 | (left << 3) | right) };
                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);
                Assert::AreEqual(0, (int)spCPU->m_PC);

                switch (right)
                {
                case 0x00:  // B
                    spCPU->m_BC = 0x1200;
                    break;
                case 0x01:  // C
                    spCPU->m_BC = 0x0012;
                    break;
                case 0x02:  // D
                    spCPU->m_DE = 0x1200;
                    break;
                case 0x03:  // E
                    spCPU->m_DE = 0x0012;
                    break;
                case 0x04:  // H
                    spCPU->m_HL = 0x1200;
                    break;
                case 0x05:  // L
                    spCPU->m_HL = 0x0012;
                    break;
                case 0x07:  // A
                    spCPU->m_AF = 0x1200;
                    break;
                }

                spCPU->Step();

                switch (left)
                {
                case 0x00:  // B
                    Assert::AreEqual(0x1200, (int)spCPU->m_BC & 0xFF00);
                    break;
                case 0x01:  // C
                    Assert::AreEqual(0x0012, (int)spCPU->m_BC & 0x00FF);
                    break;
                case 0x02:  // D
                    Assert::AreEqual(0x1200, (int)spCPU->m_DE & 0xFF00);
                    break;
                case 0x03:  // E
                    Assert::AreEqual(0x0012, (int)spCPU->m_DE & 0x00FF);
                    break;
                case 0x04:  // H
                    Assert::AreEqual(0x1200, (int)spCPU->m_HL & 0xFF00);
                    break;
                case 0x05:  // L
                    Assert::AreEqual(0x0012, (int)spCPU->m_HL & 0x00FF);
                    break;
                case 0x07:  // A
                    Assert::AreEqual(0x1200, (int)spCPU->m_AF & 0xFF00);
                    break;
                }

                // Verify expectations after
                Assert::AreEqual(4, (int)spCPU->m_cycles);
                Assert::AreEqual(1, (int)spCPU->m_PC);

                spCPU.reset();
            }
        }
    }

    // LD r, (HL)
    TEST_METHOD(LDr_HL_Test)
    {
        // Test LD r, (HL) for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)(0x40 | (reg << 3) | 0x06) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            spCPU->m_HL = 0x1234;
            spCPU->m_MMU->Write(0x1234, 0x12);

            spCPU->Step();

            switch (reg)
            {
            case 0x00:  // B
                Assert::AreEqual(0x1200, (int)spCPU->m_BC & 0xFF00);
                break;
            case 0x01:  // C
                Assert::AreEqual(0x0012, (int)spCPU->m_BC & 0x00FF);
                break;
            case 0x02:  // D
                Assert::AreEqual(0x1200, (int)spCPU->m_DE & 0xFF00);
                break;
            case 0x03:  // E
                Assert::AreEqual(0x0012, (int)spCPU->m_DE & 0x00FF);
                break;
            case 0x04:  // H
                Assert::AreEqual(0x1200, (int)spCPU->m_HL & 0xFF00);
                break;
            case 0x05:  // L
                Assert::AreEqual(0x0012, (int)spCPU->m_HL & 0x00FF);
                break;
            case 0x07:  // A
                Assert::AreEqual(0x1200, (int)spCPU->m_AF & 0xFF00);
                break;
            }

            // Verify expectations after
            Assert::AreEqual(8, (int)spCPU->m_cycles);
            Assert::AreEqual(1, (int)spCPU->m_PC);

            spCPU.reset();
        }
    }

    // LD (HL), r
    TEST_METHOD(LD_HL_r_Test)
    {
        // Test LD (HL), r for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)(0x70 | reg) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            spCPU->m_HL = 0x1234;
            spCPU->m_MMU->Write(0x1234, 0xFF);

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0x1200;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x0012;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0x1200;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x0012;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0x1200;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x0012;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0x1200;
                break;
            }

            spCPU->Step();

            byte result = spCPU->m_MMU->Read(spCPU->m_HL);

            switch (reg)
            {
            case 0x00:  // B
                Assert::AreEqual(0x12, (int)result);
                break;
            case 0x01:  // C
                Assert::AreEqual(0x12, (int)result);
                break;
            case 0x02:  // D
                Assert::AreEqual(0x12, (int)result);
                break;
            case 0x03:  // E
                Assert::AreEqual(0x12, (int)result);
                break;
            case 0x04:  // H
                Assert::AreEqual(0x12, (int)result);
                break;
            case 0x05:  // L
                Assert::AreEqual(0x12, (int)result);
                break;
            case 0x07:  // A
                Assert::AreEqual(0x12, (int)result);
                break;
            }

            // Verify expectations after
            Assert::AreEqual(8, (int)spCPU->m_cycles);
            Assert::AreEqual(1, (int)spCPU->m_PC);

            spCPU.reset();
        }
    }

    // 0x07
    TEST_METHOD(RLCA2_Test)
    {
        // Load RLCA
        byte m_Mem[] = { 0x07, 0x07 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x3B10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0x09 0x19 0x29 0x39
    TEST_METHOD(ADDHLss_Test)
    {
        for (byte ss = 0x00; ss <= 0x03;ss++)
        {
            byte opCode = (byte)(0x09 | (ss << 4));
            byte m_Mem[] = { opCode };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_HL = 0x1111;
            switch (ss)
            {
            case 0x00:
                spCPU->m_BC = 0x1111;
                break;
            case 0x01:
                spCPU->m_DE = 0x1111;
                break;
            case 0x02:
                spCPU->m_HL = 0x1111;
                break;
            case 0x03:
                spCPU->m_SP = 0x1111;
                break;
            }

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0000, (int)spCPU->m_PC);

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(8, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0001, (int)spCPU->m_PC);
            Assert::AreEqual(0x2222, (int)spCPU->m_HL);
        }
    }

    // 0xCB BIT
    TEST_METHOD(BITbr_Test)
    {
        for (byte bit = 0x00; bit <= 0x07; bit++)
        {
            for (byte reg = 0x00; reg <= 0x07; reg++)
            {
                if (reg == 0x06) continue;

                byte opCode = (byte)(0x40 | (bit << 3) | reg);
                byte m_Mem[] = { 0xCB, opCode, 0xCB, opCode };

                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                switch (reg)
                {
                case 0x00:
                case 0x01:
                    spCPU->m_BC = 0x0000;
                    break;
                case 0x02:
                case 0x03:
                    spCPU->m_DE = 0x0000;
                    break;
                case 0x04:
                case 0x05:
                    spCPU->m_HL = 0x0000;
                    break;
                case 0x07:
                    spCPU->m_AF = 0x0000;
                    break;
                }

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0000, (int)spCPU->m_PC);

                // Step the CPU 1 OpCode
                spCPU->Step();

                // Verify expectations after
                Assert::AreEqual(8, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0002, (int)spCPU->m_PC);
                Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));

                ushort value = 1 << bit;
                switch (reg)
                {
                case 0x00:
                    spCPU->m_BC = value << 8;
                    break;
                case 0x01:
                    spCPU->m_BC = value;
                    break;
                case 0x02:
                    spCPU->m_DE = value << 8;
                    break;
                case 0x03:
                    spCPU->m_DE = value;
                    break;
                case 0x04:
                    spCPU->m_HL = value << 8;
                    break;
                case 0x05:
                    spCPU->m_HL = value;
                    break;
                case 0x07:
                    spCPU->m_AF = value << 8;
                    break;
                }

                // Step the CPU 1 OpCode
                spCPU->Step();

                // Verify expectations after
                Assert::AreEqual(16, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0004, (int)spCPU->m_PC);
                Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));

                spCPU.reset();
            }
        }
    }

    TEST_METHOD(BITb_HL_Test)
    {
        for (byte bit = 0x00; bit <= 0x07; bit++)
        {
            byte opCode = (byte)(0x40 | (bit << 3) | 0x06);
            byte m_Mem[] = { 0xCB, opCode, 0xCB, opCode, 0x00, 0xFF };

            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            // No bits set
            spCPU->m_HL = 0x0004;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0000, (int)spCPU->m_PC);

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(12, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0002, (int)spCPU->m_PC);
            Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
            Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
            Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));

            // All bits set
            spCPU->m_HL = 0x0005;

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(24, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0004, (int)spCPU->m_PC);
            Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
            Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
            Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));

            spCPU.reset();
        }
    }

    // CB RES
    TEST_METHOD(RESbr_Test)
    {
        for (byte bit = 0x00; bit <= 0x07; bit++)
        {
            for (byte reg = 0x00; reg <= 0x07; reg++)
            {
                if (reg == 0x06) continue;

                byte opCode = (byte)(0x80 | (bit << 3) | reg);
                byte m_Mem[] = { 0xCB, opCode };

                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                switch (reg)
                {
                case 0x00:
                case 0x01:
                    spCPU->m_BC = 0xFFFF;
                    break;
                case 0x02:
                case 0x03:
                    spCPU->m_DE = 0xFFFF;
                    break;
                case 0x04:
                case 0x05:
                    spCPU->m_HL = 0xFFFF;
                    break;
                case 0x07:
                    spCPU->m_AF = 0xFFFF;
                    break;
                }

                ushort value = 0xFF & ~(1 << bit);

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0000, (int)spCPU->m_PC);

                // Step the CPU 1 OpCode
                spCPU->Step();

                // Verify expectations after
                Assert::AreEqual(8, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0002, (int)spCPU->m_PC);
                switch (reg)
                {
                case 0x00:
                    Assert::AreEqual(((value << 8) | 0x00FF), (int)spCPU->m_BC);
                    break;
                case 0x01:
                    Assert::AreEqual((value | 0xFF00), (int)spCPU->m_BC);
                    break;
                case 0x02:
                    Assert::AreEqual(((value << 8) | 0x00FF), (int)spCPU->m_DE);
                    break;
                case 0x03:
                    Assert::AreEqual((value | 0xFF00), (int)spCPU->m_DE);
                    break;
                case 0x04:
                    Assert::AreEqual(((value << 8) | 0x00FF), (int)spCPU->m_HL);
                    break;
                case 0x05:
                    Assert::AreEqual((value | 0xFF00), (int)spCPU->m_HL);
                    break;
                case 0x07:
                    Assert::AreEqual(((value << 8) | 0x00FF), (int)spCPU->m_AF);
                    break;
                }

                spCPU.reset();
            }
        }
    }

    TEST_METHOD(RESb_HL_Test)
    {
        for (byte bit = 0x00; bit <= 0x07; bit++)
        {
            byte opCode = (byte)(0x80 | (bit << 3) | 0x06);
            byte m_Mem[] = { 0xCB, opCode, 0xFF };

            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            ushort value = 0xFF & ~(1 << bit);

            spCPU->m_HL = 0x0002;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0000, (int)spCPU->m_PC);

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(16, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0002, (int)spCPU->m_PC);

            Assert::AreEqual((int)value, (int)spCPU->m_MMU->Read(spCPU->m_HL));

            spCPU.reset();
        }
    }

    // CB SET
    TEST_METHOD(SETbr_Test)
    {
        for (byte bit = 0x00; bit <= 0x07; bit++)
        {
            for (byte reg = 0x00; reg <= 0x07; reg++)
            {
                if (reg == 0x06) continue;

                byte opCode = (byte)(0xC0 | (bit << 3) | reg);
                byte m_Mem[] = { 0xCB, opCode };

                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                switch (reg)
                {
                case 0x00:
                case 0x01:
                    spCPU->m_BC = 0x0000;
                    break;
                case 0x02:
                case 0x03:
                    spCPU->m_DE = 0x0000;
                    break;
                case 0x04:
                case 0x05:
                    spCPU->m_HL = 0x0000;
                    break;
                case 0x07:
                    spCPU->m_AF = 0x0000;
                    break;
                }

                ushort value = (1 << bit);

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0000, (int)spCPU->m_PC);

                // Step the CPU 1 OpCode
                spCPU->Step();

                // Verify expectations after
                Assert::AreEqual(8, (int)spCPU->m_cycles);
                Assert::AreEqual(0x0002, (int)spCPU->m_PC);
                switch (reg)
                {
                case 0x00:
                    Assert::AreEqual((value << 8), (int)spCPU->m_BC);
                    break;
                case 0x01:
                    Assert::AreEqual((int)value, (int)spCPU->m_BC);
                    break;
                case 0x02:
                    Assert::AreEqual((value << 8), (int)spCPU->m_DE);
                    break;
                case 0x03:
                    Assert::AreEqual((int)value, (int)spCPU->m_DE);
                    break;
                case 0x04:
                    Assert::AreEqual((value << 8), (int)spCPU->m_HL);
                    break;
                case 0x05:
                    Assert::AreEqual((int)value, (int)spCPU->m_HL);
                    break;
                case 0x07:
                    Assert::AreEqual((value << 8), (int)spCPU->m_AF);
                    break;
                }

                spCPU.reset();
            }
        }
    }

    TEST_METHOD(SETb_HL_Test)
    {
        for (byte bit = 0x00; bit <= 0x07; bit++)
        {
            byte opCode = (byte)(0xC0 | (bit << 3) | 0x06);
            byte m_Mem[] = { 0xCB, opCode, 0x00 };

            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_HL = 0x0002;

            ushort value = (1 << bit);

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0000, (int)spCPU->m_PC);

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(16, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0002, (int)spCPU->m_PC);
            Assert::AreEqual((int)value, (int)spCPU->m_MMU->Read(spCPU->m_HL));

            spCPU.reset();
        }
    }

    // 0x00
    TEST_METHOD(NOP_Test)
    {
        // Load a NOP into the first byte of memory
        byte m_Mem[] = { 0x00 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(1, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0x01
    TEST_METHOD(LDBCnn_Test)
    {
        // Load LDHLnn
        byte m_Mem[] = { 0x01, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x1234, (int)spCPU->m_BC);

        spCPU.reset();
    }

    // 0x02
    TEST_METHOD(LD_BC_A_Test)
    {
        // Load LD (bc), a
        byte m_Mem[] = { 0x02 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;
        spCPU->m_BC = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->Read(0x1234));

        spCPU.reset();
    }

    // 0x03
    TEST_METHOD(INCBC_Test)
    {
        byte m_Mem[] = { 0x03 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1235, (int)spCPU->m_BC);

        spCPU.reset();
    }

    // 0x04
    TEST_METHOD(INCB_Test)
    {
        // Load two INCB instructions
        byte m_Mem[] = { 0x04, 0x04 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0100, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set B to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_BC = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x05
    TEST_METHOD(DECB_Test)
    {
        byte m_Mem[] = { 0x05, 0x05, 0x05 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_BC = 0x0100;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_BC = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x06
    TEST_METHOD(LDBn_Test)
    {
        // Load LDBe and 0x12 into memory
        byte m_Mem[] = { 0x06, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_BC);

        spCPU.reset();
    }

    // 0x0B
    TEST_METHOD(DECBC_Test)
    {
        byte m_Mem[] = { 0x0B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1233, (int)spCPU->m_BC);

        spCPU.reset();
    }

    // 0x10
    TEST_METHOD(STOP_Test)
    {
        byte m_Mem[] = { 0x10 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->m_isHalted);

        spCPU.reset();
    }

    // 0x12
    TEST_METHOD(LD_DE_A_Test)
    {
        // Load LD (de), a
        byte m_Mem[] = { 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;
        spCPU->m_DE = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->Read(0x1234));

        spCPU.reset();
    }

    // 0x1B
    TEST_METHOD(DECDE_Test)
    {
        byte m_Mem[] = { 0x1B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1233, (int)spCPU->m_DE);

        spCPU.reset();
    }

    // 0x2B
    TEST_METHOD(DECHL_Test)
    {
        byte m_Mem[] = { 0x2B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1233, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x3B
    TEST_METHOD(DECSP_Test)
    {
        byte m_Mem[] = { 0x3B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1233, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0x0C
    TEST_METHOD(INCC_Test)
    {
        // Load two INCC operators
        byte m_Mem[] = { 0x0C, 0x0C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0001, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set C to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_BC = 0x00FF;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x0D
    TEST_METHOD(DECC_Test)
    {
        byte m_Mem[] = { 0x0D, 0x0D, 0x0D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_BC = 0x0001;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_BC = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x0E
    TEST_METHOD(LDCn_Test)
    {
        // Load LDCe and 0x12 into memory
        byte m_Mem[] = { 0x0E, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0012, (int)spCPU->m_BC);

        spCPU.reset();
    }

    // 0x11
    TEST_METHOD(LDDEnn_Test)
    {
        // Load LDDEnn and 0x1234 into memory
        byte m_Mem[] = { 0x11, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x1234, (int)spCPU->m_DE);

        spCPU.reset();
    }

    // 0x08
    TEST_METHOD(LD_nn_SP_Test)
    {
        byte m_Mem[] = { 0x08, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xFFFE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(20, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0xFFFE, (int)spCPU->m_MMU->ReadUShort(0x1234));

        spCPU.reset();
    }

    // 0x13
    TEST_METHOD(INCDE_Test)
    {
        byte m_Mem[] = { 0x13 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1235, (int)spCPU->m_DE);

        spCPU.reset();
    }

    // 0x1C
    TEST_METHOD(INCE_Test)
    {
        // Load two INCE instructions
        byte m_Mem[] = { 0x1C, 0x1C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0001, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set B to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_DE = 0x00FF;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x14
    TEST_METHOD(INCD_Test)
    {
        // Load two INCD instructions
        byte m_Mem[] = { 0x14, 0x14 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0100, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set B to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_DE = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x15
    TEST_METHOD(DECD_Test)
    {
        byte m_Mem[] = { 0x15, 0x15, 0x15 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_DE = 0x0100;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_DE = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x16
    TEST_METHOD(LDDn_Test)
    {
        byte m_Mem[] = { 0x16, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_DE);

        spCPU.reset();
    }

    // 0x18
    TEST_METHOD(JRe_Test)
    {
        // Load JRe and other data to test
        byte m_Mem[] = { 0x18, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0006, (int)spCPU->m_PC);
    }

    // 0x1A
    TEST_METHOD(LDA_DE__Test)
    {
        // Load LDA_DE_ and filler
        byte m_Mem[] = { 0x1A, 0x00, 0x00, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Point DE to 0x12 in memory
        spCPU->m_DE = 0x0003;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x0A
    TEST_METHOD(LDA_BC__Test)
    {
        byte m_Mem[] = { 0x0A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x12);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xE8
    TEST_METHOD(ADDSPdd_Test)
    {
        byte m_Mem[] = { 0xE8, 0x01 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0x0001;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0002, (int)spCPU->m_SP);

        // TODO
        // Expand this test? I don't really know what the flags are supposed
        // to even be doing, due to lack of documentation.

        spCPU.reset();
    }

    // 0x96
    TEST_METHOD(SUB_HL__Test)
    {
        byte m_Mem[] = { 0x96, 0x96 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0xFF);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(1, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_MMU->Write(0x1234, 0x05);

        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xDE
    TEST_METHOD(SBCAn_Test)
    {
        byte m_Mem[] = { 0xDE, 0xFF, 0xDE, 0x05 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(2, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0500;
        spCPU->SetFlag(CarryFlag);

        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x9E
    TEST_METHOD(SBCA_HL__Test)
    {
        byte m_Mem[] = { 0x9E, 0x9E };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0xFF);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(1, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0500;
        spCPU->m_MMU->Write(0x1234, 0x05);
        spCPU->SetFlag(CarryFlag);

        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    TEST_METHOD(SBCAr_Test)
    {
        // Test for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)((0x90 | reg) + 8), (byte)((0x90 | reg) + 8) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_AF = 0xFF00;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0xFF00;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x00FF;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0xFF00;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x00FF;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0xFF00;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x00FF;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0xFF00;
                break;
            }

            spCPU->Step();

            byte result = spCPU->GetHighByte(spCPU->m_AF);


            Assert::AreEqual(4, (int)spCPU->m_cycles);
            Assert::AreEqual(1, (int)spCPU->m_PC);
            Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
            Assert::AreEqual(0x00, (int)result);

            spCPU->m_AF = 0x0500;

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0x0500;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x0005;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0x0500;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x0005;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0x0500;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x0005;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0x0500;
                break;
            }

            spCPU->SetFlag(CarryFlag);

            spCPU->Step();

            Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
            Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
            Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
            Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

            Assert::AreEqual(8, (int)spCPU->m_cycles);
            Assert::AreEqual(2, (int)spCPU->m_PC);

            spCPU.reset();
        }
    }

    // 0x1D
    TEST_METHOD(DECE_Test)
    {
        byte m_Mem[] = { 0x1D, 0x1D, 0x1D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_DE = 0x0001;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_DE = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x1E
    TEST_METHOD(LDEn_Test)
    {
        byte m_Mem[] = { 0x1E, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0012, (int)spCPU->m_DE);

        spCPU.reset();
    }

    // 0x20
    TEST_METHOD(JRNZe_Test)
    {
        // Load JRNZe and other data to test
        byte m_Mem[] = { 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x20, 0xFA };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->ClearFlag(ZeroFlag);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0006, (int)spCPU->m_PC);

        spCPU->SetFlag(ZeroFlag);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(20, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0008, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0x21
    TEST_METHOD(LDHLnn_Test)
    {
        // Load LDHLnn
        byte m_Mem[] = { 0x21, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x1234, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x23
    TEST_METHOD(INCHL_Test)
    {
        byte m_Mem[] = { 0x23 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1235, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x24
    TEST_METHOD(INCH_Test)
    {
        // Load two INCH instructions
        byte m_Mem[] = { 0x24, 0x24 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0100, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set H to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_HL = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x25
    TEST_METHOD(DECH_Test)
    {
        byte m_Mem[] = { 0x25, 0x25, 0x25 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_HL = 0x0100;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_HL = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x26
    TEST_METHOD(LDHn_Test)
    {
        byte m_Mem[] = { 0x26, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x28
    TEST_METHOD(JRZe_Test)
    {
        byte m_Mem[] = { 0x28, 0x04, 0x00, 0x00, 0x00, 0x00, 0x28, 0x02 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->SetFlag(ZeroFlag);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0006, (int)spCPU->m_PC);

        spCPU->ClearFlag(ZeroFlag);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(20, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0008, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0x2C
    TEST_METHOD(INCL_Test)
    {
        // Load two INCL instructions
        byte m_Mem[] = { 0x2C, 0x2C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0001, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set L to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_HL = 0x00FF;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x2D
    TEST_METHOD(DECL_Test)
    {
        byte m_Mem[] = { 0x2D, 0x2D, 0x2D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_HL = 0x0001;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_HL = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x2E
    TEST_METHOD(LDLn_Test)
    {
        byte m_Mem[] = { 0x2E, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0012, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x30
    TEST_METHOD(JRNCe_Test)
    {
        byte m_Mem[] = { 0x30, 0x04, 0x00, 0x00, 0x00, 0x00, 0x30, 0xFA };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->ClearFlag(CarryFlag);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0006, (int)spCPU->m_PC);

        spCPU->SetFlag(CarryFlag);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(20, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0008, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0x31
    TEST_METHOD(LDSPnn_Test)
    {
        // Load LDHLnn
        byte m_Mem[] = { 0x31, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x1234, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0x22
    TEST_METHOD(LDI_HL_A_Test)
    {
        byte m_Mem[] = { 0x22 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_AF = 0x1200;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->Read(0x1234));
        Assert::AreEqual(0x1235, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x2A
    TEST_METHOD(LDIA_HL_Test)
    {
        byte m_Mem[] = { 0x2A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x12);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_AF);
        Assert::AreEqual(0x1235, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x32
    TEST_METHOD(LDD_HL_A_Test)
    {
        // Load LDD_HL_A
        byte m_Mem[] = { 0x32 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_AF = 0x1200;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->Read(0x1234));
        Assert::AreEqual(0x1233, (int)spCPU->m_HL);

        spCPU.reset();
    }

    // 0x3A
    TEST_METHOD(LDDA_HL__Test)
    {
        byte m_Mem[] = { 0x3A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_AF = 0x0000;
        spCPU->m_MMU->Write(0x1234, 0x12);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x33
    TEST_METHOD(INCSP_Test)
    {
        byte m_Mem[] = { 0x33 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1235, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0x34
    TEST_METHOD(INC_HL_Test)
    {
        byte m_Mem[] = { 0x34, 0x34, 0x34 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x00);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x01, (int)spCPU->m_MMU->Read(0x1234));
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_MMU->Write(0x1234, 0x02);
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(24, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x03, (int)spCPU->m_MMU->Read(0x1234));
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_MMU->Write(0x1234, 0xFF);
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(36, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x00, (int)spCPU->m_MMU->Read(0x1234));
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x35
    TEST_METHOD(DEC_HL_Test)
    {
        byte m_Mem[] = { 0x35, 0x35, 0x35 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0xFF);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE, (int)spCPU->m_MMU->Read(0x1234));
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_MMU->Write(0x1234, 0x01);
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(24, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00, (int)spCPU->m_MMU->Read(0x1234));
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_MMU->Write(0x1234, 0x00);
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(36, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF, (int)spCPU->m_MMU->Read(0x1234));
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x36
    TEST_METHOD(LD_HL_n_Test)
    {
        byte m_Mem[] = { 0x36, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0, (int)spCPU->m_PC);

        spCPU->m_HL = 0x1234;

        spCPU->Step();

        byte result = spCPU->m_MMU->Read(spCPU->m_HL);

        // Verify expectations after
        Assert::AreEqual(0x12, (int)result);
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(2, (int)spCPU->m_PC);

        spCPU.reset();
    }


    // 0x38
    TEST_METHOD(JRCe_Test)
    {
        byte m_Mem[] = { 0x38, 0x04, 0x00, 0x00, 0x00, 0x00, 0x38, 0xFA };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->SetFlag(CarryFlag);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0006, (int)spCPU->m_PC);

        spCPU->ClearFlag(CarryFlag);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(20, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0008, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0x3C
    TEST_METHOD(INCA_Test)
    {
        // Load two INCA instructions
        byte m_Mem[] = { 0x3C, 0x3C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_AF);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0100, (int)spCPU->m_AF);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set A to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_AF = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00A0, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x3D
    TEST_METHOD(DECA_Test)
    {
        byte m_Mem[] = { 0x3D, 0x3D, 0x3D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)(spCPU->m_AF & 0xFF00));
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_AF = 0x0100;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)(spCPU->m_AF & 0xFF00));
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_AF = 0x0000;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)(spCPU->m_AF & 0xFF00));
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0x3E
    TEST_METHOD(LDAn_Test)
    {
        // Load LDAe
        byte m_Mem[] = { 0x3E, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1200, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x76
    TEST_METHOD(HALT_Test)
    {
        byte m_Mem[] = { 0x76 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->m_isHalted);

        spCPU.reset();
    }

    TEST_METHOD(ADDAr_Test)
    {
        // Test for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)(0x80 | reg) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_AF = 0xFF00;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0x0100;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x0001;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0x0100;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x0001;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0x0100;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x0001;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0x0100;
                break;
            }

            spCPU->Step();

            byte result = spCPU->GetHighByte(spCPU->m_AF);

            if (reg == 0x07)
            {
                Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
                Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
                Assert::AreEqual(0x02, (int)result);
            }
            else
            {
                Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
                Assert::AreEqual(0x00, (int)result);
            }

            // Verify expectations after
            Assert::AreEqual(4, (int)spCPU->m_cycles);
            Assert::AreEqual(1, (int)spCPU->m_PC);

            spCPU.reset();
        }
    }

    TEST_METHOD(ADCAr_Test)
    {
        // Test for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)((0x80 | reg) + 8) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_AF = 0xFF00;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0x0100;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x0001;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0x0100;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x0001;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0x0100;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x0001;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0x0100;
                break;
            }

            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(4, (int)spCPU->m_cycles);
            Assert::AreEqual(1, (int)spCPU->m_PC);

            if (reg == 0x07)
            {
                // 0x01 + 0x01 = 0x02
                // result is 0xFE
                // bit 3 is borrowed
                // bit 7 is borrowed
                Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
                Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::AreEqual(0x0200, (int)spCPU->m_AF);
            }
            else
            {
                // 0xFF + 0x01 = 0x00
                // result is 0x00
                // bit 3 is carried
                // bit 7 is carried
                Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
                Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
                Assert::AreEqual(0x00B0, (int)spCPU->m_AF);
            }

            spCPU.reset();
        }
    }

    // 0x86
    TEST_METHOD(ADDA_HL__Test)
    {
        byte m_Mem[] = { 0x86, 0x86 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_HL = 0x1234;

        spCPU->m_MMU->Write(0x1234, 0x01);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        // 0x01 + 0xFF = 0x00
        // result is 0x00
        // bit 3 is carried
        // bit 7 is carried
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0x00B0, (int)spCPU->m_AF);

        spCPU->m_AF = 0xFF00;
        spCPU->m_HL = 0x1234;

        spCPU->m_MMU->Write(0x1234, 0xFF);

        spCPU->Step();

        // 0xFF + 0xFF = 0xFE
        // result is 0xFE
        // bit 3 is borrowed
        // bit 7 is borrowed
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFE30, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x90
    TEST_METHOD(SUBB_Test)
    {
        byte m_Mem[] = { 0x90, 0x90 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_BC = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_BC = 0x0500;

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x91
    TEST_METHOD(SUBC_Test)
    {
        byte m_Mem[] = { 0x91, 0x91 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_BC = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_BC = 0x0005;

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x92
    TEST_METHOD(SUBD_Test)
    {
        byte m_Mem[] = { 0x92, 0x92 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_DE = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_DE = 0x0500;

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x93
    TEST_METHOD(SUBE_Test)
    {
        byte m_Mem[] = { 0x93, 0x93 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_DE = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_DE = 0x0005;

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x94
    TEST_METHOD(SUBH_Test)
    {
        byte m_Mem[] = { 0x94, 0x94 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_HL = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_HL = 0x0500;

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x95
    TEST_METHOD(SUBL_Test)
    {
        byte m_Mem[] = { 0x95, 0x95 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;
        spCPU->m_HL = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;
        spCPU->m_HL = 0x0005;

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x97
    TEST_METHOD(SUBA_Test)
    {
        byte m_Mem[] = { 0x97, 0x97 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        // Subtracting A from A will never set the half carry or carry flag.
        // We don't need to test for it.

        spCPU.reset();
    }

    // 0xA8
    TEST_METHOD(XORB_Test)
    {
        // Load XORB
        byte m_Mem[] = { 0xA8 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1200;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xA9
    TEST_METHOD(XORC_Test)
    {
        // Load XORC
        byte m_Mem[] = { 0xA9 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x0012;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xAA
    TEST_METHOD(XORD_Test)
    {
        // Load XORD
        byte m_Mem[] = { 0xAA };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x1200;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xAB
    TEST_METHOD(XORE_Test)
    {
        // Load XORE
        byte m_Mem[] = { 0xAB };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x0012;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xAC
    TEST_METHOD(XORH_Test)
    {
        // Load XORD
        byte m_Mem[] = { 0xAC };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1200;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xAD
    TEST_METHOD(XORL_Test)
    {
        // Load XORL
        byte m_Mem[] = { 0xAD };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0012;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xAF
    TEST_METHOD(XORA_Test)
    {
        // Load XORA
        byte m_Mem[] = { 0xAF };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0080, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xAE
    TEST_METHOD(XOR_HL_Test)
    {
        // Load XOR (HL)
        byte m_Mem[] = { 0xAE, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0001;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB0
    TEST_METHOD(ORB_Test)
    {
        // Load ORB
        byte m_Mem[] = { 0xB0 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1200;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB1
    TEST_METHOD(ORC_Test)
    {
        // Load ORC
        byte m_Mem[] = { 0xB1 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x0012;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB2
    TEST_METHOD(ORD_Test)
    {
        // Load ORD
        byte m_Mem[] = { 0xB2 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x1200;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB3
    TEST_METHOD(ORE_Test)
    {
        // Load ORE
        byte m_Mem[] = { 0xB3 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x0012;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB4
    TEST_METHOD(ORH_Test)
    {
        // Load ORH
        byte m_Mem[] = { 0xB4 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1200;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB5
    TEST_METHOD(ORL_Test)
    {
        // Load ORL
        byte m_Mem[] = { 0xB5 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0012;
        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB6
    TEST_METHOD(OR_HL_Test)
    {
        // Load ORA
        byte m_Mem[] = { 0xB6 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1400;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x12);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xB7
    TEST_METHOD(ORA_Test)
    {
        // Load ORA
        byte m_Mem[] = { 0xB7 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1200, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xBE
    TEST_METHOD(CP_HL__Test)
    {
        byte m_Mem[] = { 0xBE, 0xBE };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x0200;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x02);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));

        spCPU->m_AF = 0x0100;

        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    // 0xC0 0xC8 0xD0 0xD8
    TEST_METHOD(RETcc_Test)
    {
        for (byte flag = 0x00; flag <= 0x01; flag++)
        {
            for (byte test = 0x00; test <= 0x03; test++)
            {
                byte opCode = 0xC0 | (test << 3);
                // Load RET
                byte m_Mem[] = { opCode };
                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                spCPU->m_SP = 0xFFFE;
                spCPU->PushUShortToSP(0x1234);

                if (flag == 0x00)
                {
                    switch (test)
                    {
                    case 0x00:
                        spCPU->ClearFlag(ZeroFlag);
                        break;
                    case 0x01:
                        spCPU->SetFlag(ZeroFlag);
                        break;
                    case 0x02:
                        spCPU->ClearFlag(CarryFlag);
                        break;
                    case 0x03:
                        spCPU->SetFlag(CarryFlag);
                        break;
                    }
                }
                else
                {
                    switch (test)
                    {
                    case 0x00:
                        spCPU->SetFlag(ZeroFlag);
                        break;
                    case 0x01:
                        spCPU->ClearFlag(ZeroFlag);
                        break;
                    case 0x02:
                        spCPU->SetFlag(CarryFlag);
                        break;
                    case 0x03:
                        spCPU->ClearFlag(CarryFlag);
                        break;
                    }
                }

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);

                // Step the CPU 1 OpCode
                spCPU->Step();

                if (flag == 0x00)
                {
                    // Verify expectations after
                    Assert::AreEqual(20, (int)spCPU->m_cycles);
                    Assert::AreEqual(0x1234, (int)spCPU->m_PC);
                    Assert::AreEqual(0xFFFE, (int)spCPU->m_SP);
                }
                else
                {
                    // Verify expectations after
                    Assert::AreEqual(8, (int)spCPU->m_cycles);
                    Assert::AreEqual(0x0001, (int)spCPU->m_PC);
                    Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
                }

                spCPU.reset();
            }
        }
    }

    // 0xF6
    TEST_METHOD(ORn_Test)
    {
        // Load ORA
        byte m_Mem[] = { 0xF6, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x1600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xC1
    TEST_METHOD(POPBC_Test)
    {
        // Load POP BC
        byte m_Mem[] = { 0xC1 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xFFFE;
        spCPU->PushUShortToSP(0x1234);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)(spCPU->m_BC));
        Assert::AreEqual(0xFFFE, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0xC3
    TEST_METHOD(JPnn_Test)
    {
        byte m_Mem[] = { 0xC3, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)(spCPU->m_PC));

        spCPU.reset();
    }

    // 0xC5
    TEST_METHOD(PUSHBC_Test)
    {
        // Load PUSH BC
        byte m_Mem[] = { 0xC5 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1234;
        spCPU->m_SP = 0xFFFE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
        Assert::AreEqual(0x1234, (int)(spCPU->m_MMU->ReadUShort(0xFFFC)));

        spCPU.reset();
    }

    // 0xC6
    TEST_METHOD(ADDAn_Test)
    {
        byte m_Mem[] = { 0xC6, 0x01, 0xC6, 0xFF };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        // 0x01 + 0xFF = 0x00
        // result is 0x00
        // bit 3 is carried
        // bit 7 is carried
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(2, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0x00B0, (int)spCPU->m_AF);

        spCPU->m_AF = 0xFF00;

        spCPU->m_MMU->Write(0x1234, 0xFF);

        spCPU->Step();

        // 0xFF + 0xFF = 0xFE
        // result is 0xFE
        // bit 3 is borrowed
        // bit 7 is borrowed
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFE30, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xC9
    TEST_METHOD(RET_Test)
    {
        byte m_Mem[] = { 0xC9 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xEEEE;
        spCPU->PushUShortToSP(0x1234);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0xEEEE, (int)spCPU->m_SP);
        Assert::AreEqual(0x1234, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0xCD
    TEST_METHOD(CALLnn_Test)
    {
        // Load CALLnn
        byte m_Mem[] = { 0xCD, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xFFFE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(24, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)spCPU->m_PC);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
        Assert::AreEqual(0x0003, (int)(spCPU->m_MMU->ReadUShort(0xFFFC)));

        spCPU.reset();
    }

    // 0xCE
    TEST_METHOD(ADCAn_Test)
    {
        byte m_Mem[] = { 0xCE, 0x05, 0xCE, 0x05, 0xCE, 0x01 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x0000;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0500, (int)spCPU->m_AF);

        spCPU->SetFlag(CarryFlag);
        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0B00, (int)spCPU->m_AF);

        spCPU->m_AF = 0xFF00;
        spCPU->Step();

        Assert::AreEqual(24, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::AreEqual(0x00B0, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0x8E
    TEST_METHOD(ADCA_HL__Test)
    {
        byte m_Mem[] = { 0x8E, 0x8E, 0x8E };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x0000;
        spCPU->m_HL = 0x1234;
        spCPU->m_MMU->Write(0x1234, 0x05);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0500, (int)spCPU->m_AF);

        spCPU->SetFlag(CarryFlag);
        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0B00, (int)spCPU->m_AF);

        spCPU->m_AF = 0xFF00;
        spCPU->m_MMU->Write(0x1234, 0x01);

        spCPU->Step();

        Assert::AreEqual(24, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::AreEqual(0x00B0, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xC4 0xCC 0xD4 0xDC
    TEST_METHOD(CALLccnn_Test)
    {
        for (byte flag = 0x00; flag <= 0x01; flag++)
        {
            for (byte test = 0x00; test <= 0x03; test++)
            {
                byte opCode = 0xC4 | (test << 3);
                // Load CALL
                byte m_Mem[] = { opCode, 0x34, 0x12 };
                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                spCPU->m_SP = 0xFFFE;

                if (flag == 0x00)
                {
                    switch (test)
                    {
                    case 0x00:
                        spCPU->ClearFlag(ZeroFlag);
                        break;
                    case 0x01:
                        spCPU->SetFlag(ZeroFlag);
                        break;
                    case 0x02:
                        spCPU->ClearFlag(CarryFlag);
                        break;
                    case 0x03:
                        spCPU->SetFlag(CarryFlag);
                        break;
                    }
                }
                else
                {
                    switch (test)
                    {
                    case 0x00:
                        spCPU->SetFlag(ZeroFlag);
                        break;
                    case 0x01:
                        spCPU->ClearFlag(ZeroFlag);
                        break;
                    case 0x02:
                        spCPU->SetFlag(CarryFlag);
                        break;
                    case 0x03:
                        spCPU->ClearFlag(CarryFlag);
                        break;
                    }
                }

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);

                // Step the CPU 1 OpCode
                spCPU->Step();

                if (flag == 0x00)
                {
                    // Verify expectations after
                    Assert::AreEqual(24, (int)spCPU->m_cycles);
                    Assert::AreEqual(0x1234, (int)spCPU->m_PC);
                    Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
                    Assert::AreEqual(0x0003, (int)(spCPU->m_MMU->ReadUShort(0xFFFC)));
                }
                else
                {
                    // Verify expectations after
                    Assert::AreEqual(12, (int)spCPU->m_cycles);
                    Assert::AreEqual(0x0003, (int)spCPU->m_PC);
                    Assert::AreEqual(0xFFFE, (int)spCPU->m_SP);
                }

                spCPU.reset();
            }
        }
    }

    // 0xC2 0xCA 0xD2 0xDA
    TEST_METHOD(JPccnn_Test)
    {
        for (byte flag = 0x00; flag <= 0x01; flag++)
        {
            for (byte test = 0x00; test <= 0x03; test++)
            {
                byte opCode = 0xC2 | (test << 3);
                // Load CALL
                byte m_Mem[] = { opCode, 0x34, 0x12 };
                std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
                spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

                if (flag == 0x00)
                {
                    switch (test)
                    {
                    case 0x00:
                        spCPU->ClearFlag(ZeroFlag);
                        break;
                    case 0x01:
                        spCPU->SetFlag(ZeroFlag);
                        break;
                    case 0x02:
                        spCPU->ClearFlag(CarryFlag);
                        break;
                    case 0x03:
                        spCPU->SetFlag(CarryFlag);
                        break;
                    }
                }
                else
                {
                    switch (test)
                    {
                    case 0x00:
                        spCPU->SetFlag(ZeroFlag);
                        break;
                    case 0x01:
                        spCPU->ClearFlag(ZeroFlag);
                        break;
                    case 0x02:
                        spCPU->SetFlag(CarryFlag);
                        break;
                    case 0x03:
                        spCPU->ClearFlag(CarryFlag);
                        break;
                    }
                }

                // Verify expectations before we run
                Assert::AreEqual(0, (int)spCPU->m_cycles);

                // Step the CPU 1 OpCode
                spCPU->Step();

                if (flag == 0x00)
                {
                    // Verify expectations after
                    Assert::AreEqual(16, (int)spCPU->m_cycles);
                    Assert::AreEqual(0x1234, (int)spCPU->m_PC);
                }
                else
                {
                    // Verify expectations after
                    Assert::AreEqual(12, (int)spCPU->m_cycles);
                    Assert::AreEqual(0x0003, (int)spCPU->m_PC);
                }

                spCPU.reset();
            }
        }
    }

    // 0xD1
    TEST_METHOD(POPDE_Test)
    {
        // Load POP DE
        byte m_Mem[] = { 0xD1 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xFFFE;
        spCPU->PushUShortToSP(0x1234);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)(spCPU->m_DE));
        Assert::AreEqual(0xFFFE, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0xD5
    TEST_METHOD(PUSHDE_Test)
    {
        // Load PUSH DE
        byte m_Mem[] = { 0xD5 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x1234;
        spCPU->m_SP = 0xFFFE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
        Assert::AreEqual(0x1234, (int)(spCPU->m_MMU->ReadUShort(0xFFFC)));

        spCPU.reset();
    }

    // 0xD6
    TEST_METHOD(SUBn_Test)
    {
        byte m_Mem[] = { 0xD6, 0xFF, 0xD6, 0x05 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(2, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::AreEqual(0x00C0, (int)spCPU->m_AF);

        spCPU->m_AF = 0x0400;

        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0xFF70, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xE0
    TEST_METHOD(LD_0xFF00n_A_Test)
    {
        // Load LD_0xFF00n_A
        byte m_Mem[] = { 0xE0, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->Read(0xFF12));

        spCPU.reset();
    }

    // 0xE1
    TEST_METHOD(POPHL_Test)
    {
        // Load POP HL
        byte m_Mem[] = { 0xE1 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xFFFE;
        spCPU->PushUShortToSP(0x1234);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)(spCPU->m_HL));
        Assert::AreEqual(0xFFFE, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0xE2
    TEST_METHOD(LD_0xFF00C_A_Test)
    {
        // Load LD_0xFF00n_A
        byte m_Mem[] = { 0xE2 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;
        spCPU->m_BC = 0x0012;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->Read(0xFF12));

        spCPU.reset();
    }

    // 0xE5
    TEST_METHOD(PUSHHL_Test)
    {
        // Load PUSH HL
        byte m_Mem[] = { 0xE5 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;
        spCPU->m_SP = 0xFFFE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
        Assert::AreEqual(0x1234, (int)(spCPU->m_MMU->ReadUShort(0xFFFC)));

        spCPU.reset();
    }

    // 0xE6
    TEST_METHOD(ANDn_Test)
    {
        // Load AND n
        byte m_Mem[] = { 0xE6, 0xF0, 0xE6, 0x00 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x8F00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x8020, (int)spCPU->m_AF);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);

        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x00A0, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xEE
    TEST_METHOD(XORn_Test)
    {
        byte m_Mem[] = { 0xEE, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1400;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);

        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0600, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xF0
    TEST_METHOD(LDA_0xFF00n__Test)
    {
        byte m_Mem[] = { 0xF0, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_MMU->Write((0xFF00 + 0x12), 0x34);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x34, (int)spCPU->m_MMU->Read(0xFF00 + 0x12));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x3400, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xF1
    TEST_METHOD(POPAF_Test)
    {
        // Load POP AF
        byte m_Mem[] = { 0xF1 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0xFFFE;
        spCPU->PushUShortToSP(0x1234);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1230, (int)(spCPU->m_AF));
        Assert::AreEqual(0xFFFE, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0xF2
    TEST_METHOD(LDA_0xFF00C__Test)
    {
        byte m_Mem[] = { 0xF2 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x0012;

        spCPU->m_MMU->Write((0xFF00 + 0x12), 0x34);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x34, (int)spCPU->m_MMU->Read(0xFF00 + 0x12));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x3400, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xF3
    TEST_METHOD(DI_Test)
    {
        byte m_Mem[] = { 0xF3 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Enable interrupts
        spCPU->m_IME = 0x01;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x00, (int)spCPU->m_IME);

        spCPU.reset();
    }

    // 0xF5
    TEST_METHOD(PUSHAF_Test)
    {
        // Load PUSH AF
        byte m_Mem[] = { 0xF5 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1234;
        spCPU->m_SP = 0xFFFE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFC, (int)spCPU->m_SP);
        Assert::AreEqual(0x1234, (int)(spCPU->m_MMU->ReadUShort(0xFFFC)));

        spCPU.reset();
    }

    // 0xF9
    TEST_METHOD(LDSPHL_Test)
    {
        byte m_Mem[] = { 0xF9 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0x1234;
        spCPU->m_HL = 0xFFFF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0xFFFF, (int)spCPU->m_SP);

        spCPU.reset();
    }

    // 0xFA
    TEST_METHOD(LDA_nn_Test)
    {
        byte m_Mem[] = { 0xFA, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_MMU->Write(0x1234, 0x55);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x5500, (int)spCPU->m_AF);

        spCPU.reset();
    }

    // 0xFB
    TEST_METHOD(EI_Test)
    {
        byte m_Mem[] = { 0xFB };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Disable interrupts
        spCPU->m_IME = 0x00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x01, (int)(spCPU->m_IME));

        spCPU.reset();
    }

    // 0xD9
    TEST_METHOD(RETI_Test)
    {
        byte m_Mem[] = { 0xD9 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Disable interrupts
        spCPU->m_IME = 0x00;

        // Push a PC to stack
        spCPU->PushUShortToSP(0x1234);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)spCPU->m_PC);
        Assert::AreEqual(0x01, (int)(spCPU->m_IME));

        spCPU.reset();
    }

    // 0xEA
    TEST_METHOD(LD_nn_A_Test)
    {
        byte m_Mem[] = { 0xEA, 0x34, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x5500;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x55, (int)(spCPU->m_MMU->Read(0x1234)));

        spCPU.reset();
    }

    // 0xFE
    TEST_METHOD(CPn_Test)
    {
        byte m_Mem[] = { 0xFE, 0x02, 0xFE, 0x02 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x0200;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));

        spCPU->m_AF = 0x0100;

        spCPU->Step();

        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(CPr_Test)
    {
        // Test for each register (Except F, of course)
        for (byte reg = 0x00; reg <= 0x07; reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { (byte)((0xB0 | reg) + 8), (byte)((0xB0 | reg) + 8) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            spCPU->m_AF = 0x0200;

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0, (int)spCPU->m_PC);

            switch (reg)
            {
            case 0x00:  // B
                spCPU->m_BC = 0x0200;
                break;
            case 0x01:  // C
                spCPU->m_BC = 0x0002;
                break;
            case 0x02:  // D
                spCPU->m_DE = 0x0200;
                break;
            case 0x03:  // E
                spCPU->m_DE = 0x0002;
                break;
            case 0x04:  // H
                spCPU->m_HL = 0x0200;
                break;
            case 0x05:  // L
                spCPU->m_HL = 0x0002;
                break;
            case 0x07:  // A
                spCPU->m_AF = 0x0200;
                break;
            }


            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(4, (int)spCPU->m_cycles);
            Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
            Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
            Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

            spCPU->m_AF = 0x0100;

            spCPU->Step();

            if (reg == 0x07)
            {
                Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));
                Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
            }
            else
            {
                Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
                Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));
                Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
            }

            // Verify expectations after
            Assert::AreEqual(8, (int)spCPU->m_cycles);
            Assert::AreEqual(2, (int)spCPU->m_PC);

        spCPU.reset();
    }
    }

    // 0xCB 0x00
    TEST_METHOD(RLCB_Test)
    {
        // Load RLCB
        byte m_Mem[] = { 0xCB, 0x00, 0xCB, 0x00 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3B00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x01
    TEST_METHOD(RLCC_Test)
    {
        // Load RLC C
        byte m_Mem[] = { 0xCB, 0x01, 0xCB, 0x01 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x00CE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009D, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003B, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x02
    TEST_METHOD(RLCD_Test)
    {
        // Load RLC D
        byte m_Mem[] = { 0xCB, 0x02, 0xCB, 0x02 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3B00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x03
    TEST_METHOD(RLCE_Test)
    {
        // Load RLC E
        byte m_Mem[] = { 0xCB, 0x03, 0xCB, 0x03 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x00CE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009D, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003B, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x04
    TEST_METHOD(RLCH_Test)
    {
        // Load RLC H
        byte m_Mem[] = { 0xCB, 0x04, 0xCB, 0x04 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3B00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x05
    TEST_METHOD(RLCL_Test)
    {
        // Load RLC L
        byte m_Mem[] = { 0xCB, 0x05, 0xCB, 0x05 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x00CE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009D, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003B, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x06
    TEST_METHOD(RLC_HL_Test)
    {
        // Load RLC (HL)
        byte m_Mem[] = { 0xCB, 0x06, 0xCB, 0x06, 0xCE };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3B, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x07
    TEST_METHOD(RLCA_Test)
    {
        // Load RLC A
        byte m_Mem[] = { 0xCB, 0x07, 0xCB, 0x07 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_AF & 0xFF00);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3B00, (int)spCPU->m_AF & 0xFF00);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x08
    TEST_METHOD(RRCB_Test)
    {
        // Load RRCB
        byte m_Mem[] = { 0xCB, 0x08, 0xCB, 0x08 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x3B00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xCE00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x09
    TEST_METHOD(RRCC_Test)
    {
        // Load RRC C
        byte m_Mem[] = { 0xCB, 0x09, 0xCB, 0x09 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x003B;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009D, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00CE, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x0A
    TEST_METHOD(RRCD_Test)
    {
        // Load RRC D
        byte m_Mem[] = { 0xCB, 0x0A, 0xCB, 0x0A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x3B00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xCE00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x0B
    TEST_METHOD(RRCE_Test)
    {
        // Load RRC E
        byte m_Mem[] = { 0xCB, 0x0B, 0xCB, 0x0B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x003B;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009D, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00CE, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x0C
    TEST_METHOD(RRCH_Test)
    {
        // Load RRC H
        byte m_Mem[] = { 0xCB, 0x0C, 0xCB, 0x0C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x3B00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xCE00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x0D
    TEST_METHOD(RRCL_Test)
    {
        // Load RRC L
        byte m_Mem[] = { 0xCB, 0x0D, 0xCB, 0x0D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x003B;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009D, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00CE, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x0E
    TEST_METHOD(RRC_HL_Test)
    {
        // Load RRC (HL)
        byte m_Mem[] = { 0xCB, 0x0E, 0xCB, 0x0E, 0x3B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xCE, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x0F
    TEST_METHOD(RRCA_Test)
    {
        // Load RRC A
        byte m_Mem[] = { 0xCB, 0x0F, 0xCB, 0x0F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x3B00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9D10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xCE10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x10
    TEST_METHOD(RLB_Test)
    {
        // Load RLB
        byte m_Mem[] = { 0xCB, 0x10, 0xCB, 0x10 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9C00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3900, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x11
    TEST_METHOD(RLC_Test)
    {
        // Load RLC
        byte m_Mem[] = { 0xCB, 0x11, 0xCB, 0x11 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x00CE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009C, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x0039, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x12
    TEST_METHOD(RLD_Test)
    {
        // Load RLD
        byte m_Mem[] = { 0xCB, 0x12, 0xCB, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9C00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3900, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x13
    TEST_METHOD(RLE_Test)
    {
        // Load RLE
        byte m_Mem[] = { 0xCB, 0x13, 0xCB, 0x13 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x00CE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009C, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x0039, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x14
    TEST_METHOD(RLH_Test)
    {
        // Load RLC
        byte m_Mem[] = { 0xCB, 0x14, 0xCB, 0x14 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9C00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3900, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x15
    TEST_METHOD(RLL_Test)
    {
        // Load RLC
        byte m_Mem[] = { 0xCB, 0x15, 0xCB, 0x15 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x00CE;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009C, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x0039, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x16
    TEST_METHOD(RL_HL_Test)
    {
        // Load RLC
        byte m_Mem[] = { 0xCB, 0x16, 0xCB, 0x16, 0xCE };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x009C, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x0039, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0x17
    TEST_METHOD(RLA_Test)
    {
        // Load RLA
        byte m_Mem[] = { 0x17, 0x17 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x9C10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x3910, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0x1F
    TEST_METHOD(RRA2_Test)
    {
        // Load RRA
        byte m_Mem[] = { 0x1F, 0x1F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x3900;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x1C10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x8E00, (int)spCPU->m_AF);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x17
    TEST_METHOD(RLA2_Test)
    {
        // Load RLA
        byte m_Mem[] = { 0xCB, 0x17, 0xCB, 0x17 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xCE00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x9C10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3910, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x18
    TEST_METHOD(RRB_Test)
    {
        // Load RRB
        byte m_Mem[] = { 0xCB, 0x18, 0xCB, 0x18 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x3900;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1C00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x8E00, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x19
    TEST_METHOD(RRC_Test)
    {
        // Load RRC
        byte m_Mem[] = { 0xCB, 0x19, 0xCB, 0x19 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x0039;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x001C, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x008E, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x1A
    TEST_METHOD(RRD_Test)
    {
        // Load RRD
        byte m_Mem[] = { 0xCB, 0x1A, 0xCB, 0x1A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x3900;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1C00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x8E00, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x1B
    TEST_METHOD(RRE_Test)
    {
        // Load RRE
        byte m_Mem[] = { 0xCB, 0x1B, 0xCB, 0x1B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x0039;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x001C, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x008E, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x1C
    TEST_METHOD(RRH_Test)
    {
        // Load RRC
        byte m_Mem[] = { 0xCB, 0x1C, 0xCB, 0x1C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x3900;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1C00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x8E00, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x1D
    TEST_METHOD(RRL_Test)
    {
        // Load RRC
        byte m_Mem[] = { 0xCB, 0x1D, 0xCB, 0x1D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0039;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x001C, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x008E, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x1E
    TEST_METHOD(RR_HL_Test)
    {
        // Load RRC
        byte m_Mem[] = { 0xCB, 0x1E, 0xCB, 0x1E, 0x39 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x001C, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x008E, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    // 0xCB 0x1F
    TEST_METHOD(RRA_Test)
    {
        // Load RRA
        byte m_Mem[] = { 0xCB, 0x1F, 0xCB, 0x1F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x3900;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1C10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x8E00, (int)spCPU->m_AF);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }


    TEST_METHOD(SWAPr_Test)
    {
        for (byte reg = 0x00;reg < 0x07;reg++)
        {
            if (reg == 0x06) continue;

            byte m_Mem[] = { 0xCB, (byte)(0x30 | reg) };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            switch (reg)
            {
            case 0x00:
                spCPU->m_BC = 0xAB00;
                break;
            case 0x01:
                spCPU->m_BC = 0x00AB;
                break;
            case 0x02:
                spCPU->m_DE = 0xAB00;
                break;
            case 0x03:
                spCPU->m_DE = 0x00AB;
                break;
            case 0x04:
                spCPU->m_HL = 0xAB00;
                break;
            case 0x05:
                spCPU->m_HL = 0x00AB;
                break;
            case 0x07:
                spCPU->m_AF = 0xAB00;
                break;
            }

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0000, (int)spCPU->m_PC);

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(8, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0002, (int)spCPU->m_PC);

            switch (reg)
            {
            case 0x00:
                Assert::AreEqual(0xBA00, (int)spCPU->m_BC);
                break;
            case 0x01:
                Assert::AreEqual(0x00BA, (int)spCPU->m_BC);
                break;
            case 0x02:
                Assert::AreEqual(0xBA00, (int)spCPU->m_DE);
                break;
            case 0x03:
                Assert::AreEqual(0x00BA, (int)spCPU->m_DE);
                break;
            case 0x04:
                Assert::AreEqual(0xBA00, (int)spCPU->m_HL);
                break;
            case 0x05:
                Assert::AreEqual(0x00BA, (int)spCPU->m_HL);
                break;
            case 0x07:
                Assert::AreEqual(0xBA00, (int)spCPU->m_AF);
                break;
            }

            spCPU.reset();
        }
    }

    TEST_METHOD(SWAP_HL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x36, 0xAB };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0002;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);

        Assert::AreEqual(0xBA, (int)spCPU->m_MMU->Read(spCPU->m_HL));

        spCPU.reset();
    }

    TEST_METHOD(SLAB_Test)
    {
        byte m_Mem[] = { 0xCB, 0x20, 0xCB, 0x20 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0xBF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFC00, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLAC_Test)
    {
        byte m_Mem[] = { 0xCB, 0x21, 0xCB, 0x21 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x00BF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FC, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLAD_Test)
    {
        byte m_Mem[] = { 0xCB, 0x22, 0xCB, 0x22 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0xBF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFC00, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLAE_Test)
    {
        byte m_Mem[] = { 0xCB, 0x23, 0xCB, 0x23 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x00BF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FC, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLAH_Test)
    {
        byte m_Mem[] = { 0xCB, 0x24, 0xCB, 0x24 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0xBF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFC00, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLAL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x25, 0xCB, 0x25 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x00BF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FC, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLA_HL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x26, 0xCB, 0x26, 0xBF };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FC, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SLAA_Test)
    {
        byte m_Mem[] = { 0xCB, 0x27, 0xCB, 0x27 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xBF00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFC00, (int)spCPU->m_AF);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAB_Test)
    {
        byte m_Mem[] = { 0xCB, 0x28, 0xCB, 0x28 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAC_Test)
    {
        byte m_Mem[] = { 0xCB, 0x29, 0xCB, 0x29 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x00FD;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAD_Test)
    {
        byte m_Mem[] = { 0xCB, 0x2A, 0xCB, 0x2A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAE_Test)
    {
        byte m_Mem[] = { 0xCB, 0x2B, 0xCB, 0x2B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x00FD;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAH_Test)
    {
        byte m_Mem[] = { 0xCB, 0x2C, 0xCB, 0x2C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x2D, 0xCB, 0x2D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x00FD;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRA_HL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x2E, 0xCB, 0x2E, 0xFD };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FE, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x00FF, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRAA_Test)
    {
        byte m_Mem[] = { 0xCB, 0x2F, 0xCB, 0x2F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0xFE00, (int)spCPU->m_AF & 0xFF00);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0xFF00, (int)spCPU->m_AF & 0xFF00);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLB_Test)
    {
        byte m_Mem[] = { 0xCB, 0x38, 0xCB, 0x38 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E00, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3F00, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLC_Test)
    {
        byte m_Mem[] = { 0xCB, 0x39, 0xCB, 0x39 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x00FD;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003F, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLD_Test)
    {
        byte m_Mem[] = { 0xCB, 0x3A, 0xCB, 0x3A };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E00, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3F00, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLE_Test)
    {
        byte m_Mem[] = { 0xCB, 0x3B, 0xCB, 0x3B };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_DE = 0x00FD;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003F, (int)spCPU->m_DE);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLH_Test)
    {
        byte m_Mem[] = { 0xCB, 0x3C, 0xCB, 0x3C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E00, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3F00, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x3D, 0xCB, 0x3D };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x00FD;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003F, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRL_HL_Test)
    {
        byte m_Mem[] = { 0xCB, 0x3E, 0xCB, 0x3E, 0xFD };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0004;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x007E, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(32, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x003F, (int)spCPU->m_MMU->Read(spCPU->m_HL));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(SRLA_Test)
    {
        byte m_Mem[] = { 0xCB, 0x3F, 0xCB, 0x3F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0xFD00;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x7E10, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::AreEqual(0x3F00, (int)spCPU->m_AF);
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(JP_HL_Test)
    {
        byte m_Mem[] = { 0xE9 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x1234, (int)spCPU->m_PC);

        spCPU.reset();
    }

    TEST_METHOD(SCF_Test)
    {
        byte m_Mem[] = { 0x37 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(CCF_Test)
    {
        byte m_Mem[] = { 0x3F, 0x3F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        spCPU->SetFlag(CarryFlag);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();

        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(CPL_Test)
    {
        byte m_Mem[] = { 0x2F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        spCPU->m_AF = 0x0500;

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::IsTrue(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::AreEqual(0x05 ^ 0xFF, (int)spCPU->GetHighByte(spCPU->m_AF));
    }

    TEST_METHOD(LDHLSPe_Test)
    {
        byte m_Mem[] = { 0xF8, 0xFE };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_SP = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(12, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x1232, (int)spCPU->m_HL);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(SubtractFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }

    TEST_METHOD(RSTn_Test)
    {
        for (byte rst = 0x00; rst <= 0x07; rst++)
        {
            byte opCode = (byte)(0xC7 | (rst << 3));
            byte m_Mem[] = { opCode };
            std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
            spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

            // Verify expectations before we run
            Assert::AreEqual(0, (int)spCPU->m_cycles);
            Assert::AreEqual(0x0000, (int)spCPU->m_PC);

            // Step the CPU 1 OpCode
            spCPU->Step();

            // Verify expectations after
            Assert::AreEqual(16, (int)spCPU->m_cycles);
            Assert::AreEqual(0x08 * rst, (int)spCPU->m_PC);

            spCPU.reset();
        }
    }

    TEST_METHOD(DAA_Test)
    {
        // ADD A, B
        // DAA
        byte m_Mem[] = { 0x80, 0x27 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_BC = 0x1500;
        spCPU->m_AF = 0x2700;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 2 OpCodes
        spCPU->Step();
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x42, (int)spCPU->GetHighByte(spCPU->m_AF));

        spCPU.reset();
    }

    TEST_METHOD(RRCA2_Test)
    {
        // RRCA
        byte m_Mem[] = { 0x0F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x2700;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x93, (int)spCPU->GetHighByte(spCPU->m_AF));
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
    }
};
