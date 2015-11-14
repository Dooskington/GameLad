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
            ushort val = ReadByte(address + 1);
            val = val << 8;
            val |= ReadByte(address);
            return val;
        }

        byte ReadByte(const ushort& address)
        {
            return m_data[address];
        }

        bool WriteByte(const ushort& address, const byte val)
        {
            m_data[address] = val;
            return true;
        }

    private:
        byte m_data[0xFFFF + 1];
    };

public:
    // Misc tests
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
            ushort* reg = spCPU->GetUShortRegister(flags);

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

    // LD r, R
    TEST_METHOD(LDrR_Test)
    {
        // Test LD r, R for each register (Except F, of course)
        for (byte left = 0x00; left <= 0x07; left++)
        {
            for (byte right = 0x00; right <= 0x07; right++)
            {
                if (left == 0x06 || right == 0x06) continue;

                byte m_Mem[] = { (0x40 | (left << 3) | right) };
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set B to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_BC = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set C to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_BC = 0x00FF;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set B to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_DE = 0x00FF;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set B to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_DE = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_DE);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x9C90, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU->Step();
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x3990, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(CarryFlag));

        spCPU.reset();
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set H to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_HL = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set L to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_HL = 0x00FF;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_HL);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->ReadByte(0x1234));

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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        // Set A to 0xFF to cause Zero and HalfCarry to be set and execute
        spCPU->m_AF = 0xFF00;
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x00A0, (int)spCPU->m_AF);
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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

    // 0x77
    TEST_METHOD(LD_HL_A_Test)
    {
        // Load LD_HL_A
        byte m_Mem[] = { 0x77 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;
        spCPU->m_HL = 0x1234;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->ReadByte(0x1234));

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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

        Assert::AreEqual(0x0080, (int)spCPU->m_AF);

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
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->ReadByte(0xFF12));

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
        Assert::AreEqual(0x12, (int)spCPU->m_MMU->ReadByte(0xFF12));

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

    // 0xCB 0x7C
    TEST_METHOD(BIT7h_Test)
    {
        // Load BIT7h
        byte m_Mem[] = { 0xCB, 0x7C, 0xCB, 0x7C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_HL = 0x0000;

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
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));

        spCPU->m_HL = 0x8000;

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(16, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0004, (int)spCPU->m_PC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsTrue(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));

        spCPU.reset();
    }
};
