#include "stdafx.h"
#include "CppUnitTest.h"

#include <Emulator.hpp>
#include <Joypad.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(JoypadTests)
{
public:
    TEST_METHOD(FullInputTest)
    {
        std::unique_ptr<Joypad> spJoypad = std::unique_ptr<Joypad>(new Joypad(nullptr));

        // No input, no items selected, expected all bits set
        spJoypad->SetInput(JOYPAD_NONE, JOYPAD_NONE);
        spJoypad->WriteByte(JoypadAddress, 0x30);
        Assert::AreEqual(0x3F, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons only
        spJoypad->WriteByte(JoypadAddress, 0x10);
        Assert::AreEqual(0x1F, (int)spJoypad->ReadByte(JoypadAddress));

        // Select input only
        spJoypad->WriteByte(JoypadAddress, 0x20);
        Assert::AreEqual(0x2F, (int)spJoypad->ReadByte(JoypadAddress));

        // Press a button, no change
        spJoypad->SetInput(JOYPAD_NONE, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0x2F, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons now, A button shows as LOW (pressed)
        spJoypad->WriteByte(JoypadAddress, 0x10);
        Assert::AreEqual(0x1E, (int)spJoypad->ReadByte(JoypadAddress));

        // Press a direction, no change
        spJoypad->SetInput(JOYPAD_INPUT_UP, JOYPAD_BUTTONS_A);
        Assert::AreEqual(0x1E, (int)spJoypad->ReadByte(JoypadAddress));

        // Select buttons and input now, UP and A button shows as LOW (pressed)
        spJoypad->WriteByte(JoypadAddress, 0x00);
        Assert::AreEqual(0x0A, (int)spJoypad->ReadByte(JoypadAddress));

        spJoypad.reset();
    }
};
