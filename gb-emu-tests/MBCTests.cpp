#include "stdafx.h"

#include <MBC.hpp>

TEST_CLASS(MBCTests)
{
public:
    TEST_METHOD(ROMOnlyTest)
    {
        byte rom[0x8000];
        memset(rom, 0x00, ARRAYSIZE(rom));
        rom[0x5432] = 0xDE;

        byte ram[0x2000];
        memset(ram, 0x00, ARRAYSIZE(ram));
        ram[0x0BCD] = 0xAB;

        std::unique_ptr<ROMOnly_MBC> spMBC = std::unique_ptr<ROMOnly_MBC>(new ROMOnly_MBC(rom, ram));

        // Test ROM and RAM read
        Assert::AreEqual(0xDE, (int)spMBC->ReadByte(0x5432));
        Assert::AreEqual(0xAB, (int)spMBC->ReadByte(0xABCD));

        // Test ROM and RAM write
        ::Logger::Disable();
        Assert::IsFalse(spMBC->WriteByte(0x5432, 0xEF));
        ::Logger::Enable();
        Assert::IsTrue(spMBC->WriteByte(0xABCD, 0xEF));
        Assert::AreEqual(0xEF, (int)spMBC->ReadByte(0xABCD));
        Assert::AreEqual((int)ram[0x0BCD], (int)spMBC->ReadByte(0xABCD));

        spMBC.reset();
    }

    TEST_METHOD(MBC1Test)
    {
        byte rom[0x10000];   //64 KB, 4 banks
        memset(rom, 0x00, ARRAYSIZE(rom));
        rom[0x1234] = 0xDE; // Bank 0
        rom[0x4321] = 0xAF; // Bank 1
        rom[0x8321] = 0xBB; // Bank 2

        byte ram[0x8000];
        memset(ram, 0x00, ARRAYSIZE(ram));
        ram[0x0BCD] = 0xAB; // Bank 0
        ram[0x2BCD] = 0xCC; // Bank 1

        std::unique_ptr<MBC1_MBC> spMBC = std::unique_ptr<MBC1_MBC>(new MBC1_MBC(rom, ram));

        // Test ROM and RAM read
        Assert::AreEqual(0xDE, (int)spMBC->ReadByte(0x1234));
        Assert::AreEqual(0xAF, (int)spMBC->ReadByte(0x4321));   // Default bank test
        ::Logger::Disable();
        Assert::AreEqual(0xFF, (int)spMBC->ReadByte(0xABCD));   // RAM is not enabled
        ::Logger::Enable();

        // Change ROM to bank 2
        Assert::IsTrue(spMBC->WriteByte(0x2000, 0x02));
        Assert::AreEqual(0xBB, (int)spMBC->ReadByte(0x4321));   // Bank 2

        // Change ROM to bank 0, but should select 1
        Assert::IsTrue(spMBC->WriteByte(0x2000, 0x00));
        Assert::AreEqual(0xAF, (int)spMBC->ReadByte(0x4321));   // Default bank test

        // Enable RAM
        Assert::IsTrue(spMBC->WriteByte(0x0000, 0x0A));
        Assert::AreEqual(0xAB, (int)spMBC->ReadByte(0xABCD));   // RAM is enabled

        // Change RAM to bank 1
        Assert::IsTrue(spMBC->WriteByte(0x4000, 0x01));
        Assert::AreEqual(0xAB, (int)spMBC->ReadByte(0xABCD));   // Bank 0, because RAMBankMode is not set

        // Enable RAMBankMode
        Assert::IsTrue(spMBC->WriteByte(0x6000, 0x01));
        Assert::AreEqual(0xCC, (int)spMBC->ReadByte(0xABCD));   // Bank 1

        spMBC.reset();
    }

    TEST_METHOD(MBC2Test)
    {
        byte rom[0x10000];   //64 KB, 4 banks
        memset(rom, 0x00, ARRAYSIZE(rom));
        rom[0x1234] = 0xDE; // Bank 0
        rom[0x4321] = 0xAF; // Bank 1
        rom[0x8321] = 0xBB; // Bank 2

        std::unique_ptr<MBC2_MBC> spMBC = std::unique_ptr<MBC2_MBC>(new MBC2_MBC(rom));

        // Test ROM and RAM read
        Assert::AreEqual(0xDE, (int)spMBC->ReadByte(0x1234));
        Assert::AreEqual(0xAF, (int)spMBC->ReadByte(0x4321));   // Default bank test

        // Change ROM to bank 2
        Assert::IsTrue(spMBC->WriteByte(0x2000, 0x02));
        Assert::AreEqual(0xBB, (int)spMBC->ReadByte(0x4321));   // Bank 2

        // Enable RAM
        Assert::IsTrue(spMBC->WriteByte(0x0000, 0x0A));
        Assert::IsTrue(spMBC->WriteByte(0xA123, 0xFF));
        Assert::AreEqual(0x0F, (int)spMBC->ReadByte(0xA123));
        Assert::IsTrue(spMBC->WriteByte(0xA123, 0xAD));
        Assert::AreEqual(0x0D, (int)spMBC->ReadByte(0xA123));   // Bank 1

        // Disable RAM
        Assert::IsTrue(spMBC->WriteByte(0x0000, 0x00));
        ::Logger::Disable();
        Assert::AreEqual(0xFF, (int)spMBC->ReadByte(0xA123));   // RAM is not enabled
        ::Logger::Enable();

        spMBC.reset();
    }

    TEST_METHOD(MBC3Test)
    {
        byte rom[0x10000];   //64 KB, 4 banks
        memset(rom, 0x00, ARRAYSIZE(rom));
        rom[0x1234] = 0xDE; // Bank 0
        rom[0x4321] = 0xAF; // Bank 1
        rom[0x8321] = 0xBB; // Bank 2

        byte ram[0x8000];
        memset(ram, 0x00, ARRAYSIZE(ram));
        ram[0x0BCD] = 0xAB; // Bank 0
        ram[0x2BCD] = 0xCC; // Bank 1

        std::unique_ptr<MBC3_MBC> spMBC = std::unique_ptr<MBC3_MBC>(new MBC3_MBC(rom, ram));

        // Test ROM and RAM read
        Assert::AreEqual(0xDE, (int)spMBC->ReadByte(0x1234));
        Assert::AreEqual(0xAF, (int)spMBC->ReadByte(0x4321));   // Default bank test
        ::Logger::Disable();
        Assert::AreEqual(0xFF, (int)spMBC->ReadByte(0xABCD));   // RAM is not enabled
        ::Logger::Enable();

        // Change ROM to bank 2
        Assert::IsTrue(spMBC->WriteByte(0x2000, 0x02));
        Assert::AreEqual(0xBB, (int)spMBC->ReadByte(0x4321));   // Bank 2

        // Change ROM to bank 0, but should select 1
        Assert::IsTrue(spMBC->WriteByte(0x2000, 0x00));
        Assert::AreEqual(0xAF, (int)spMBC->ReadByte(0x4321));   // Default bank test

        // Enable RAM
        Assert::IsTrue(spMBC->WriteByte(0x0000, 0x0A));
        Assert::AreEqual(0xAB, (int)spMBC->ReadByte(0xABCD));   // RAM is enabled

        // Change RAM to bank 1
        Assert::IsTrue(spMBC->WriteByte(0x4000, 0x01));
        Assert::AreEqual(0xCC, (int)spMBC->ReadByte(0xABCD));   // Bank 1

        spMBC.reset();
    }
};
