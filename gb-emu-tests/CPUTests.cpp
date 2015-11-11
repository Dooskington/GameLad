#include "stdafx.h"
#include "CppUnitTest.h"

#include <CPU.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
                memcpy_s(m_data, ARRAYSIZE(m_data), memory, size);
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

    // 0x06
    TEST_METHOD(LDBe_Test)
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
    TEST_METHOD(LDCe_Test)
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
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x1234, (int)spCPU->m_DE);

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
        Assert::AreEqual(0x12, (int)spCPU->GetHighByte(spCPU->m_AF));

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
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0003, (int)spCPU->m_PC);
        Assert::AreEqual(0x1234, (int)spCPU->m_HL);

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
        Assert::AreEqual(8, (int)spCPU->m_cycles);
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

    // 0x3E
    TEST_METHOD(LDAe_Test)
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
        Assert::AreEqual(0x12, (int)spCPU->GetHighByte(spCPU->m_AF));

        spCPU.reset();
    }

    // 0x4F
    TEST_METHOD(LDCA_Test)
    {
        // Load LDCA
        byte m_Mem[] = { 0x4F };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new CPUTestsMMU(m_Mem, ARRAYSIZE(m_Mem)), true);

        spCPU->m_AF = 0x1200;
        spCPU->m_BC = 0x00FF;

        // Verify expectations before we run
        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);

        // Step the CPU 1 OpCode
        spCPU->Step();

        // Verify expectations after
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0012, (int)spCPU->m_BC);

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
        Assert::AreEqual(0x00, (int)spCPU->GetHighByte(spCPU->m_AF));
        
        Assert::IsTrue(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));
        Assert::IsFalse(spCPU->IsFlagSet(CarryFlag));

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
