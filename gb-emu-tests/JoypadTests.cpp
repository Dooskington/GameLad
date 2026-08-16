#include "stdafx.h"

#include <Emulator.hpp>
#include <Joypad.hpp>

#define INPUT_ONLY      0x20
#define BUTTONS_ONLY    0x10
#define NO_INPUT        0x30
#define BOTH_INPUT      0x00

class JoypadTestCPU : public ICPU
{
public:
    JoypadTestCPU() : InterruptCount(0), LastInterrupt(0x00) {}

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

TEST_CLASS(JoypadTests)
{
public:
    TEST_METHOD(FullInputTest)
    {
        std::unique_ptr<Joypad> spJoypad = std::unique_ptr<Joypad>(new Joypad(nullptr));

        // No input, no items selected, expected all bits set
        spJoypad->SetInput(JOYPAD_NONE, JOYPAD_NONE);
        spJoypad->WriteByte(JoypadAddress, NO_INPUT);
        Assert::AreEqual(0xFF, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons only
        spJoypad->WriteByte(JoypadAddress, BUTTONS_ONLY);
        Assert::AreEqual(0xDF, (int)spJoypad->ReadByte(JoypadAddress));

        // Select input only
        spJoypad->WriteByte(JoypadAddress, INPUT_ONLY);
        Assert::AreEqual(0xEF, (int)spJoypad->ReadByte(JoypadAddress));

        // Press a button, no change
        spJoypad->SetInput(JOYPAD_NONE, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0xEF, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons now, A button shows as LOW (pressed)
        spJoypad->WriteByte(JoypadAddress, BUTTONS_ONLY);
        Assert::AreEqual(0xDE, (int)spJoypad->ReadByte(JoypadAddress));

        // Press a direction, no change
        spJoypad->SetInput(JOYPAD_INPUT_UP, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0xDE, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons and input now, UP and A button shows as LOW (pressed)
        spJoypad->WriteByte(JoypadAddress, BOTH_INPUT);
        Assert::AreEqual(0xCA, (int)spJoypad->ReadByte(JoypadAddress));

        spJoypad.reset();
    }

    TEST_METHOD(RegisterMaskTest)
    {
        Joypad joypad(nullptr);

        Assert::AreEqual(0xCF, (int)joypad.ReadByte(JoypadAddress));
        joypad.WriteByte(JoypadAddress, 0xCF);
        Assert::AreEqual(0xCF, (int)joypad.ReadByte(JoypadAddress));
        joypad.WriteByte(JoypadAddress, 0xFF);
        Assert::AreEqual(0xFF, (int)joypad.ReadByte(JoypadAddress));
    }

    TEST_METHOD(FallingEdgeInterruptTest)
    {
        JoypadTestCPU cpu;
        Joypad joypad(&cpu);

        // Unselected key presses do not change the input lines.
        joypad.WriteByte(JoypadAddress, NO_INPUT);
        joypad.SetInput(JOYPAD_NONE, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0, cpu.InterruptCount);

        // Selecting a row with a held key creates a high-to-low transition.
        joypad.WriteByte(JoypadAddress, BUTTONS_ONLY);
        Assert::AreEqual(1, cpu.InterruptCount);
        Assert::AreEqual((int)INT60, (int)cpu.LastInterrupt);

        // Stable input and key releases do not request another interrupt.
        joypad.SetInput(JOYPAD_NONE, JOYPAD_BUTTONS_A);
        joypad.SetInput(JOYPAD_NONE, JOYPAD_NONE);
        Assert::AreEqual(1, cpu.InterruptCount);

        joypad.SetInput(JOYPAD_NONE, JOYPAD_BUTTONS_B);
        Assert::AreEqual(2, cpu.InterruptCount);

        // Deselecting the active row only creates low-to-high transitions.
        joypad.WriteByte(JoypadAddress, NO_INPUT);
        Assert::AreEqual(2, cpu.InterruptCount);
    }

    TEST_METHOD(CombinedRowsUseLineEdgesTest)
    {
        JoypadTestCPU cpu;
        Joypad joypad(&cpu);

        joypad.WriteByte(JoypadAddress, NO_INPUT);
        joypad.SetInput(JOYPAD_INPUT_RIGHT, JOYPAD_BUTTONS_A);
        joypad.WriteByte(JoypadAddress, INPUT_ONLY);
        Assert::AreEqual(1, cpu.InterruptCount);

        // Selecting the second row keeps P10 low, so there is no new line edge.
        joypad.WriteByte(JoypadAddress, BOTH_INPUT);
        Assert::AreEqual(1, cpu.InterruptCount);
        Assert::AreEqual(0xCE, (int)joypad.ReadByte(JoypadAddress));
    }
};
