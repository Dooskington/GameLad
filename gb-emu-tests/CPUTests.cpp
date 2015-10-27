#include "stdafx.h"
#include "CppUnitTest.h"

#include <CPU.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CPUTests)
{
private:
    class TestMMU : public IMMU
    {
    public:
        TestMMU(byte* memory, int size)
        {
            memset(m_data, 0x00, ARRAYSIZE(m_data));
            memcpy_s(m_data, ARRAYSIZE(m_data), memory, size);
        }

        ~TestMMU()
        {
        }

        bool Initialize()
        {
            return true;
        }

        void RegisterMemoryUnit(const ushort& startRange, const ushort& endRange, IMemoryUnit* pUnit)
        {
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
        byte m_Mem[] = { 0x00 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new TestMMU(m_Mem, ARRAYSIZE(m_Mem)));

        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0, (int)spCPU->m_PC);
        spCPU->Step();
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(1, (int)spCPU->m_PC);

        spCPU.reset();
    }

    // 0x0C
    TEST_METHOD(INCC_Test)
    {
        byte m_Mem[] = { 0x0C, 0x0C };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new TestMMU(m_Mem, ARRAYSIZE(m_Mem)));

        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        Assert::AreEqual(0x0000, (int)spCPU->m_BC);
        spCPU->Step();
        Assert::AreEqual(4, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0001, (int)spCPU->m_PC);
        Assert::AreEqual(0x0001, (int)spCPU->m_BC);
        Assert::IsFalse(spCPU->IsFlagSet(ZeroFlag));
        Assert::IsFalse(spCPU->IsFlagSet(AddFlag));
        Assert::IsFalse(spCPU->IsFlagSet(HalfCarryFlag));

        spCPU->m_BC = 0x00FF;
        spCPU->Step();
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
        byte m_Mem[] = { 0x0E, 0x12 };
        std::unique_ptr<CPU> spCPU = std::make_unique<CPU>();
        spCPU->Initialize(new TestMMU(m_Mem, ARRAYSIZE(m_Mem)));

        Assert::AreEqual(0, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0000, (int)spCPU->m_PC);
        spCPU->Step();
        Assert::AreEqual(8, (int)spCPU->m_cycles);
        Assert::AreEqual(0x0002, (int)spCPU->m_PC);
        Assert::AreEqual(0x0012, (int)spCPU->m_BC);

        spCPU.reset();
    }
};
