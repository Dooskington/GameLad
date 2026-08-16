#include "stdafx.h"

#include <MMU.hpp>

TEST_CLASS(MMUTests)
{
public:
    TEST_METHOD(InterruptRegisterMaskTest)
    {
        MMU mmu;

        Assert::AreEqual(0xE0, (int)mmu.Read(0xFF0F));
        mmu.Write(0xFF0F, 0x08);
        Assert::AreEqual(0xE8, (int)mmu.Read(0xFF0F));
        mmu.Write(0xFF0F, 0xE0);
        Assert::AreEqual(0xE0, (int)mmu.Read(0xFF0F));
        mmu.Write(0xFF0F, 0xFF);
        Assert::AreEqual(0xFF, (int)mmu.Read(0xFF0F));

        // IE occupies the full byte even though only bits 0-4 select interrupts.
        Assert::AreEqual(0x00, (int)mmu.Read(0xFFFF));
        mmu.Write(0xFFFF, 0xA8);
        Assert::AreEqual(0xA8, (int)mmu.Read(0xFFFF));
    }
};
