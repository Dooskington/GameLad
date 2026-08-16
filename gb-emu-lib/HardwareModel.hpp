#pragma once

/*
    Explicit hardware model / mode selection.

    The emulator supports two consoles and three operating modes. The console
    is what the user (or the runner configuration) asked for; the mode is what
    the console actually ends up running once the cartridge header has been
    inspected, because a CGB booted with a non-CGB cartridge falls back to a
    DMG compatibility mode rather than behaving like a DMG.

    Nothing in the DMG paths is allowed to depend on cartridge contents: DMG
    stays DMG regardless of what the header says, which keeps the existing DMG
    behaviour bit-identical.
*/

enum class GameBoyModel
{
    // Original monochrome Game Boy (DMG-01).
    DMG,
    // Game Boy Color. Revision C is the reference used by the bundled test
    // corpora (gambatte "cgb04c", mealybug "_cgb_c", AGE "cgbC").
    CGB,
};

enum class GameBoyMode
{
    // DMG console.
    DMG,
    // CGB console running a CGB-aware cartridge: all CGB features enabled.
    CGB,
    // CGB console running a cartridge without the CGB flag. CGB-only
    // registers are locked out and the boot ROM installs a compatibility
    // palette, but the console is still CGB silicon.
    CGBCompatibility,
};

// What the frontend/runner asked for, before the cartridge is known.
enum class ModelPreference
{
    // Pick the console from the cartridge's CGB flag, like inserting the
    // cartridge into whichever console it was made for.
    Auto,
    ForceDMG,
    ForceCGB,
};

inline bool IsCGBHardware(GameBoyMode mode)
{
    return mode == GameBoyMode::CGB || mode == GameBoyMode::CGBCompatibility;
}

// True only when CGB-exclusive registers and rendering features are live.
// CGB compatibility mode deliberately answers false: a DMG cartridge must not
// be able to reach VBK/SVBK/HDMA/colour palettes.
inline bool IsCGBFeatureMode(GameBoyMode mode)
{
    return mode == GameBoyMode::CGB;
}

/*
    Cartridge header 0x0143 (CGB flag):
        0x80 - works on DMG and CGB, CGB features available on a CGB
        0xC0 - CGB only
    Any other value means the byte is part of the old 16-character title and
    the cartridge is DMG-only.
*/
inline bool IsCGBCartridgeFlag(byte flag)
{
    return (flag & 0x80) != 0;
}

inline bool IsCGBOnlyCartridgeFlag(byte flag)
{
    return (flag & 0xC0) == 0xC0;
}

inline GameBoyMode ResolveGameBoyMode(ModelPreference preference, byte cgbFlag)
{
    switch (preference)
    {
    case ModelPreference::ForceDMG:
        return GameBoyMode::DMG;
    case ModelPreference::ForceCGB:
        return IsCGBCartridgeFlag(cgbFlag) ? GameBoyMode::CGB : GameBoyMode::CGBCompatibility;
    case ModelPreference::Auto:
    default:
        return IsCGBCartridgeFlag(cgbFlag) ? GameBoyMode::CGB : GameBoyMode::DMG;
    }
}

inline GameBoyModel ConsoleForMode(GameBoyMode mode)
{
    return mode == GameBoyMode::DMG ? GameBoyModel::DMG : GameBoyModel::CGB;
}

// Anything that needs to know the active mode implements this so the mode can
// be pushed down once, at ROM-load time, instead of being re-derived.
class IModelAware
{
public:
    virtual ~IModelAware() {}
    virtual void SetGameBoyMode(GameBoyMode mode) = 0;
};
