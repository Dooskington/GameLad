#include "stdafx.h"

#include <Serial.hpp>
#include <Timer.hpp>

#define SERIAL_DATA 0xFF01
#define SERIAL_CONTROL 0xFF02

class SerialTestCPU : public ICPU
{
public:
    SerialTestCPU() : InterruptCount(0), LastInterrupt(0x00) {}

    bool Initialize() { return true; }
    bool LoadROM(const char*, const char*) { return true; }
    int Step() { return 0; }
    void TriggerInterrupt(byte interrupt)
    {
        InterruptCount++;
        LastInterrupt = interrupt;
    }
    byte* GetCurrentFrame() { return nullptr; }
    void SetInput(byte, byte) {}
    void SetVSyncCallback(void(*)()) {}
    void SetAudioSampleRate(unsigned int) {}
    size_t ConsumeAudioSamples(float*, size_t) { return 0; }
    void ClockAPUFrameSequencer() {}

    int InterruptCount;
    byte LastInterrupt;
};

struct SerialLinkTestContext
{
    int Calls;
    bool Ready;
    bool OutgoingBit;
};

inline bool SerialLinkTestCallback(
    void* context,
    bool outgoingBit,
    bool& incomingBit)
{
    SerialLinkTestContext* link =
        static_cast<SerialLinkTestContext*>(context);
    link->Calls++;
    link->OutgoingBit = outgoingBit;
    if (!link->Ready)
    {
        return false;
    }
    incomingBit = false;
    return true;
}

TEST_CLASS(SerialTests)
{
public:
    TEST_METHOD(RegisterMaskTest)
    {
        Serial serial;

        Assert::AreEqual(0x00, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0x7E, (int)serial.ReadByte(SERIAL_CONTROL));

        serial.WriteByte(SERIAL_DATA, 0xA5);
        serial.WriteByte(SERIAL_CONTROL, 0xFF);
        Assert::AreEqual(0xA5, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_CONTROL));

        serial.WriteByte(SERIAL_CONTROL, 0x00);
        Assert::AreEqual(0x7E, (int)serial.ReadByte(SERIAL_CONTROL));
    }

    TEST_METHOD(InternalClockTimingTest)
    {
        SerialTestCPU cpu;
        Serial serial(&cpu);

        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);

        serial.Step(511);
        Assert::AreEqual(0x00, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_CONTROL));

        serial.Step(1);
        Assert::AreEqual(0x01, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0, cpu.InterruptCount);

        serial.Step(3583);
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_CONTROL));
        Assert::AreEqual(0, cpu.InterruptCount);

        serial.Step(1);
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0x7F, (int)serial.ReadByte(SERIAL_CONTROL));
        Assert::AreEqual(1, cpu.InterruptCount);
        Assert::AreEqual((int)INT58, (int)cpu.LastInterrupt);
    }

    TEST_METHOD(ExternalClockTransferTest)
    {
        SerialTestCPU cpu;
        Serial serial(&cpu);
        bool outgoingBit = false;
        const byte incomingData = 0x3C;
        const byte outgoingData = 0xA5;

        serial.WriteByte(SERIAL_DATA, outgoingData);
        serial.WriteByte(SERIAL_CONTROL, 0x80);
        Assert::IsTrue(serial.IsWaitingForExternalClock());

        // CPU cycles cannot advance a transfer owned by an external clock.
        serial.Step(8192);
        Assert::AreEqual((int)outgoingData, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0, cpu.InterruptCount);

        for (int bit = 7; bit >= 0; bit--)
        {
            bool incomingBit = ISBITSET(incomingData, bit);
            Assert::IsTrue(serial.ClockExternalBit(incomingBit, outgoingBit));
            Assert::AreEqual(
                ISBITSET(outgoingData, bit) ? 1 : 0,
                outgoingBit ? 1 : 0);
        }

        Assert::AreEqual((int)incomingData, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0x7E, (int)serial.ReadByte(SERIAL_CONTROL));
        Assert::IsFalse(serial.IsWaitingForExternalClock());
        Assert::AreEqual(1, cpu.InterruptCount);
        Assert::AreEqual((int)INT58, (int)cpu.LastInterrupt);
        Assert::IsFalse(serial.ClockExternalBit(true, outgoingBit));
    }

    TEST_METHOD(LinkCallbackCanDelayMasterClockTest)
    {
        SerialTestCPU cpu;
        Serial serial(&cpu);
        SerialLinkTestContext link = { 0, false, false };
        serial.SetLinkCallback(SerialLinkTestCallback, &link);
        serial.WriteByte(SERIAL_DATA, 0x80);
        serial.WriteByte(SERIAL_CONTROL, 0x81);

        serial.Step(512);
        Assert::AreEqual(1, link.Calls);
        Assert::IsTrue(link.OutgoingBit);
        Assert::AreEqual(0x80, (int)serial.ReadByte(SERIAL_DATA));

        link.Ready = true;
        serial.Step(512);
        Assert::AreEqual(2, link.Calls);
        Assert::AreEqual(0x00, (int)serial.ReadByte(SERIAL_DATA));
    }

    TEST_METHOD(InternalClockPhaseAlignmentTest)
    {
        SerialTestCPU cpu;
        Serial serial(&cpu);

        serial.Step(100);
        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);

        serial.Step(411);
        Assert::AreEqual(0x00, (int)serial.ReadByte(SERIAL_DATA));
        serial.Step(1);
        Assert::AreEqual(0x01, (int)serial.ReadByte(SERIAL_DATA));

        serial.Step(3583);
        Assert::AreEqual(0, cpu.InterruptCount);
        serial.Step(1);
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(1, cpu.InterruptCount);
    }

    TEST_METHOD(InternalClockUsesFullDividerPhaseTest)
    {
        Serial serial;

        serial.Step(300);
        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);

        serial.Step(467);
        Assert::AreEqual(0x00, (int)serial.ReadByte(SERIAL_DATA));
        serial.Step(1);
        Assert::AreEqual(0x01, (int)serial.ReadByte(SERIAL_DATA));
    }

    TEST_METHOD(PreBootClockPhaseTest)
    {
        Serial serial;

        serial.PreBoot();
        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);

        serial.Step(308);
        Assert::AreEqual(0x00, (int)serial.ReadByte(SERIAL_DATA));
        serial.Step(1);
        Assert::AreEqual(0x01, (int)serial.ReadByte(SERIAL_DATA));
    }

    TEST_METHOD(DividerWriteClocksAndResetsSerialPhaseTest)
    {
        Serial serial;

        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);
        serial.Step(400);
        serial.ResetDivider();
        Assert::AreEqual(0x01, (int)serial.ReadByte(SERIAL_DATA));

        serial.Step(511);
        Assert::AreEqual(0x01, (int)serial.ReadByte(SERIAL_DATA));
        serial.Step(1);
        Assert::AreEqual(0x03, (int)serial.ReadByte(SERIAL_DATA));
    }

    TEST_METHOD(CGBFastClockTest)
    {
        Serial serial;
        serial.SetGameBoyMode(GameBoyMode::CGB);

        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x83);

        serial.Step(127);
        Assert::AreEqual(0x7F, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_CONTROL));
        serial.Step(1);
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_DATA));
        Assert::AreEqual(0x7F, (int)serial.ReadByte(SERIAL_CONTROL));
    }

    TEST_METHOD(InterruptAcknowledgeWindowTest)
    {
        SerialTestCPU dmgCPU;
        Serial dmg(&dmgCPU);
        dmg.WriteByte(SERIAL_CONTROL, 0x81);
        dmg.Step(4093);
        dmg.AcknowledgeInterrupt();
        dmg.Step(3);
        Assert::AreEqual(0, dmgCPU.InterruptCount);

        SerialTestCPU lateDMGCPU;
        Serial lateDMG(&lateDMGCPU);
        lateDMG.WriteByte(SERIAL_CONTROL, 0x81);
        lateDMG.Step(4092);
        lateDMG.AcknowledgeInterrupt();
        lateDMG.Step(4);
        Assert::AreEqual(1, lateDMGCPU.InterruptCount);

        SerialTestCPU cgbCPU;
        Serial cgb(&cgbCPU);
        cgb.SetGameBoyMode(GameBoyMode::CGB);
        cgb.WriteByte(SERIAL_CONTROL, 0x81);
        cgb.Step(4091);
        cgb.AcknowledgeInterrupt();
        cgb.Step(5);
        Assert::AreEqual(0, cgbCPU.InterruptCount);

        SerialTestCPU doubleSpeedCPU;
        Serial doubleSpeed(&doubleSpeedCPU);
        doubleSpeed.SetGameBoyMode(GameBoyMode::CGB);
        doubleSpeed.WriteByte(SERIAL_CONTROL, 0x81);
        doubleSpeed.Step(4088);
        doubleSpeed.AcknowledgeInterrupt(true);
        doubleSpeed.Step(8);
        Assert::AreEqual(0, doubleSpeedCPU.InterruptCount);

        SerialTestCPU lateDoubleSpeedCPU;
        Serial lateDoubleSpeed(&lateDoubleSpeedCPU);
        lateDoubleSpeed.SetGameBoyMode(GameBoyMode::CGB);
        lateDoubleSpeed.WriteByte(SERIAL_CONTROL, 0x81);
        lateDoubleSpeed.Step(4087);
        lateDoubleSpeed.AcknowledgeInterrupt(true);
        lateDoubleSpeed.Step(9);
        Assert::AreEqual(1, lateDoubleSpeedCPU.InterruptCount);
    }

    TEST_METHOD(ControlWriteRestartsTransferTest)
    {
        SerialTestCPU cpu;
        Serial serial(&cpu);

        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);
        serial.Step(3584);
        Assert::AreEqual(0, cpu.InterruptCount);

        serial.WriteByte(SERIAL_DATA, 0x00);
        serial.WriteByte(SERIAL_CONTROL, 0x81);
        serial.Step(4095);
        Assert::AreEqual(0, cpu.InterruptCount);
        serial.Step(1);
        Assert::AreEqual(1, cpu.InterruptCount);
        Assert::AreEqual(0xFF, (int)serial.ReadByte(SERIAL_DATA));
    }
};
