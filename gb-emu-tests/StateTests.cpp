#include "stdafx.h"

#include <Cartridge.hpp>
#include <Emulator.hpp>
#include <MBC.hpp>
#include <MMU.hpp>

#include <vector>

TEST_CLASS(StateTests)
{
private:
    static std::vector<byte> CreateROM(byte cartridgeType, byte ramSize, byte cgbFlag = 0x00)
    {
        std::vector<byte> rom(0x8000, 0x00);
        rom[0x0100] = 0x00; // NOP
        rom[0x0101] = 0x18; // JR -3
        rom[0x0102] = 0xFD;
        rom[CGBFlagAddress] = cgbFlag;
        rom[CartridgeTypeAddress] = cartridgeType;
        rom[ROMSizeAddress] = ROM_32KB;
        rom[RAMSizeAddress] = ramSize;
        return rom;
    }

    static void Step(Emulator& emulator, int count)
    {
        for (int index = 0; index < count; ++index)
        {
            emulator.Step();
        }
    }

public:
    TEST_METHOD(DeterministicRoundTripTest)
    {
        std::vector<byte> rom = CreateROM(ROMOnly, RAM_None);
        Emulator emulator;
        Assert::IsTrue(emulator.Initialize(nullptr, 0, rom.data(), rom.size()));
        emulator.SetAudioSampleRate(44100);
        Assert::IsTrue(emulator.WriteMemory(0xC123, 0x5A));
        Step(emulator, 2000);

        std::vector<byte> saved;
        Assert::IsTrue(emulator.Serialize(saved));
        const unsigned long long savedCycles = emulator.GetBaseClockCycles();

        Step(emulator, 750);
        std::vector<byte> expected;
        Assert::IsTrue(emulator.Serialize(expected));

        const bool restored = emulator.Deserialize(saved.data(), saved.size());
        if (!restored)
        {
            std::cout << "\nState restore failed.";
        }
        Assert::IsTrue(restored);
        if (emulator.GetBaseClockCycles() != savedCycles)
        {
            std::cout << "\nCycle count was not restored.";
        }
        Assert::IsTrue(emulator.GetBaseClockCycles() == savedCycles);
        Assert::AreEqual(0x5A, emulator.ReadMemory(0xC123));
        Step(emulator, 750);

        std::vector<byte> actual;
        Assert::IsTrue(emulator.Serialize(actual));
        Assert::IsTrue(actual.size() == expected.size());
        if (actual.size() == expected.size() &&
            std::memcmp(actual.data(), expected.data(), expected.size()) != 0)
        {
            size_t firstDifference = 0;
            while (firstDifference < actual.size() &&
                actual[firstDifference] == expected[firstDifference])
            {
                ++firstDifference;
            }
            std::cout << "\nFirst deterministic state difference at byte "
                      << firstDifference << ".";
        }
        Assert::IsTrue(
            actual.size() == expected.size() &&
            std::memcmp(actual.data(), expected.data(), expected.size()) == 0);
    }

    TEST_METHOD(RejectedStateIsTransactionalTest)
    {
        std::vector<byte> rom = CreateROM(ROMOnly, RAM_None);
        Emulator emulator;
        Assert::IsTrue(emulator.Initialize(nullptr, 0, rom.data(), rom.size()));
        Step(emulator, 300);

        std::vector<byte> before;
        Assert::IsTrue(emulator.Serialize(before));
        std::vector<byte> corrupt = before;
        corrupt[0] ^= 0xFF;
        Assert::IsFalse(emulator.Deserialize(corrupt.data(), corrupt.size()));

        std::vector<byte> after;
        Assert::IsTrue(emulator.Serialize(after));
        Assert::IsTrue(after.size() == before.size());
        Assert::IsTrue(std::memcmp(after.data(), before.data(), before.size()) == 0);

        std::vector<byte> otherROM = rom;
        otherROM[0x0200] = 0x42;
        Emulator other;
        Assert::IsTrue(other.Initialize(nullptr, 0, otherROM.data(), otherROM.size()));
        std::vector<byte> otherBefore;
        Assert::IsTrue(other.Serialize(otherBefore));
        Assert::IsFalse(other.Deserialize(before.data(), before.size()));
        std::vector<byte> otherAfter;
        Assert::IsTrue(other.Serialize(otherAfter));
        Assert::IsTrue(otherAfter.size() == otherBefore.size());
        Assert::IsTrue(std::memcmp(
            otherAfter.data(),
            otherBefore.data(),
            otherBefore.size()) == 0);
    }

    TEST_METHOD(PreviousStateVersionIsRejectedTest)
    {
        std::vector<byte> rom = CreateROM(ROMOnly, RAM_None);
        Emulator emulator;
        Assert::IsTrue(emulator.Initialize(nullptr, 0, rom.data(), rom.size()));
        Step(emulator, 300);

        std::vector<byte> before;
        Assert::IsTrue(emulator.Serialize(before));
        std::vector<byte> previousVersion = before;
        previousVersion[8] = 2;
        previousVersion[9] = 0;
        previousVersion[10] = 0;
        previousVersion[11] = 0;
        Assert::IsFalse(emulator.Deserialize(
            previousVersion.data(),
            previousVersion.size()));

        std::vector<byte> after;
        Assert::IsTrue(emulator.Serialize(after));
        Assert::IsTrue(after.size() == before.size());
        Assert::IsTrue(std::memcmp(after.data(), before.data(), before.size()) == 0);
    }

    TEST_METHOD(MBCAndRTCStateRoundTripTest)
    {
        std::vector<byte> rom = CreateROM(MBC3TimerRAMBattery, RAM_8KB);
        Emulator emulator;
        Assert::IsTrue(emulator.Initialize(nullptr, 0, rom.data(), rom.size()));
        Assert::IsTrue(emulator.WriteMemory(0x0000, 0x0A));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x00));
        Assert::IsTrue(emulator.WriteMemory(0xA123, 0x77));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x08));
        Assert::IsTrue(emulator.WriteMemory(0xA000, 0x2A));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0xFF));

        std::vector<byte> saved;
        Assert::IsTrue(emulator.Serialize(saved));
        std::vector<byte> rtcBefore(
            emulator.GetRTCData(),
            emulator.GetRTCData() + emulator.GetRTCDataSize());
        std::vector<byte> invalid = saved;
        invalid[0] ^= 0xFF;
        Assert::IsFalse(emulator.Deserialize(invalid.data(), invalid.size()));
        Assert::IsTrue(std::memcmp(
            rtcBefore.data(),
            emulator.GetRTCData(),
            rtcBefore.size()) == 0);

        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x00));
        Assert::IsTrue(emulator.WriteMemory(0xA123, 0x11));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x08));
        Assert::IsTrue(emulator.WriteMemory(0xA000, 0x01));

        Assert::IsTrue(emulator.Deserialize(saved.data(), saved.size()));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x00));
        Assert::AreEqual(0x77, emulator.ReadMemory(0xA123));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x08));
        Assert::AreEqual(0x2A, emulator.ReadMemory(0xA000));
        Assert::IsTrue(emulator.GetSaveRAM() != nullptr);
        Assert::AreEqual(0x2000, static_cast<int>(emulator.GetSaveRAMSize()));
        Assert::IsTrue(emulator.GetRTCData() != nullptr);
        Assert::AreEqual(13, static_cast<int>(emulator.GetRTCDataSize()));
    }

    TEST_METHOD(RumbleStateRoundTripTest)
    {
        std::vector<byte> rom = CreateROM(MBC5RumbleRAMBattery, RAM_8KB);
        Emulator emulator;
        Assert::IsTrue(emulator.Initialize(nullptr, 0, rom.data(), rom.size()));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x08));
        Assert::IsTrue(emulator.IsRumbleEnabled());

        std::vector<byte> saved;
        Assert::IsTrue(emulator.Serialize(saved));
        Assert::IsTrue(emulator.WriteMemory(0x4000, 0x00));
        Assert::IsFalse(emulator.IsRumbleEnabled());
        Assert::IsTrue(emulator.Deserialize(saved.data(), saved.size()));
        Assert::IsTrue(emulator.IsRumbleEnabled());
    }

    TEST_METHOD(CGBBootROMMappingTest)
    {
        std::vector<byte> bootROM(0x0900, 0xCC);
        bootROM[0x0000] = 0x11;
        bootROM[0x0200] = 0x22;
        bootROM[0x08FF] = 0x33;

        MMU mmu;
        Assert::IsTrue(mmu.LoadBootROM(bootROM.data(), bootROM.size()));
        Assert::AreEqual(0x11, mmu.Read(0x0000));
        Assert::AreEqual(0x00, mmu.Read(0x0100));
        Assert::AreEqual(0x22, mmu.Read(0x0200));
        Assert::AreEqual(0x33, mmu.Read(0x08FF));
        Assert::IsTrue(mmu.Write(0xFF50, 0x01));
        Assert::AreEqual(0x00, mmu.Read(0x0000));
    }

    TEST_METHOD(GameGenieReadPatchTest)
    {
        std::vector<byte> rom = CreateROM(ROMOnly, RAM_None);
        Cartridge cartridge;
        Assert::IsTrue(cartridge.LoadROM(rom.data(), rom.size()));
        Assert::AreEqual(0x00, cartridge.ReadByte(0x0100));

        cartridge.ApplyROMPatch(0x76, 0x0100, -1);
        Assert::AreEqual(0x76, cartridge.ReadByte(0x0100));
        cartridge.ClearROMPatches();
        Assert::AreEqual(0x00, cartridge.ReadByte(0x0100));

        cartridge.ApplyROMPatch(0x77, 0x0100, 0x01);
        Assert::AreEqual(0x00, cartridge.ReadByte(0x0100));
        cartridge.ClearROMPatches();
        cartridge.ApplyROMPatch(0x77, 0x0100, 0x00);
        Assert::AreEqual(0x77, cartridge.ReadByte(0x0100));
    }
};
