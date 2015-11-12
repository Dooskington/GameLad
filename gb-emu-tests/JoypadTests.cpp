#include "stdafx.h"

#include <Emulator.hpp>
#include <Joypad.hpp>

#define INPUT_ONLY      0x20
#define BUTTONS_ONLY    0x10
#define NO_INPUT        0x30
#define BOTH_INPUT      0x00

TEST_CLASS(JoypadTests)
{
public:
    TEST_METHOD(FullInputTest)
    {
        std::unique_ptr<Joypad> spJoypad = std::unique_ptr<Joypad>(new Joypad(nullptr));

        // No input, no items selected, expected all bits set
        spJoypad->SetInput(JOYPAD_NONE, JOYPAD_NONE);
        spJoypad->WriteByte(JoypadAddress, NO_INPUT);
        Assert::AreEqual(0x3F, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons only
        spJoypad->WriteByte(JoypadAddress, BUTTONS_ONLY);
        Assert::AreEqual(0x1F, (int)spJoypad->ReadByte(JoypadAddress));

        // Select input only
        spJoypad->WriteByte(JoypadAddress, INPUT_ONLY);
        Assert::AreEqual(0x2F, (int)spJoypad->ReadByte(JoypadAddress));

        // Press a button, no change
        spJoypad->SetInput(JOYPAD_NONE, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0x2F, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons now, A button shows as LOW (pressed)
        spJoypad->WriteByte(JoypadAddress, BUTTONS_ONLY);
        Assert::AreEqual(0x1E, (int)spJoypad->ReadByte(JoypadAddress));

        // Press a direction, no change
        spJoypad->SetInput(JOYPAD_INPUT_UP, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0x1E, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons and input now, UP and A button shows as LOW (pressed)
        spJoypad->WriteByte(JoypadAddress, BOTH_INPUT);
        Assert::AreEqual(0x0A, (int)spJoypad->ReadByte(JoypadAddress));

        spJoypad.reset();
    }
};
