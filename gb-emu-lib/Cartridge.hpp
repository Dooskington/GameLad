#pragma once

#define CartridgeTypeAddress 0x0147
#define ROMSizeAddress 0x0148
#define RAMSizeAddress 0x0149

/*
0x0143 - CGB flag
0x80 - the cartridge uses CGB features but still boots on a DMG
0xC0 - the cartridge is CGB only
Any other value is the last character of the legacy 16-byte title field, so the
cartridge predates the CGB and is monochrome only. Only bit 7 is decoded on
real hardware; bit 6 distinguishes "CGB enhanced" from "CGB only".
*/
#define CGBFlagAddress 0x0143

/*
0x0147 - Cartridge Type
Specifies which Memory Bank Controller (if any) is used in the cartridge, and if further external hardware exists in the cartridge.
00h  ROM ONLY                 13h  MBC3+RAM+BATTERY
01h  MBC1                     15h  MBC4
02h  MBC1+RAM                 16h  MBC4+RAM
03h  MBC1+RAM+BATTERY         17h  MBC4+RAM+BATTERY
05h  MBC2                     19h  MBC5
06h  MBC2+BATTERY             1Ah  MBC5+RAM
08h  ROM+RAM                  1Bh  MBC5+RAM+BATTERY
09h  ROM+RAM+BATTERY          1Ch  MBC5+RUMBLE
0Bh  MMM01                    1Dh  MBC5+RUMBLE+RAM
0Ch  MMM01+RAM                1Eh  MBC5+RUMBLE+RAM+BATTERY
0Dh  MMM01+RAM+BATTERY        FCh  POCKET CAMERA
0Fh  MBC3+TIMER+BATTERY       FDh  BANDAI TAMA5
10h  MBC3+TIMER+RAM+BATTERY   FEh  HuC3
11h  MBC3                     FFh  HuC1+RAM+BATTERY
12h  MBC3+RAM
*/


/*
0x0148 - ROM Size
Specifies the ROM Size of the cartridge. Typically calculated as "32KB shl N".
00h -  32KByte (no ROM banking)
01h -  64KByte (4 banks)
02h - 128KByte (8 banks)
03h - 256KByte (16 banks)
04h - 512KByte (32 banks)
05h -   1MByte (64 banks)  - only 63 banks used by MBC1
06h -   2MByte (128 banks) - only 125 banks used by MBC1
07h -   4MByte (256 banks)
52h - 1.1MByte (72 banks)
53h - 1.2MByte (80 banks)
54h - 1.5MByte (96 banks)
*/
#define ROM_32KB        0x00
#define ROM_64KB        0x01
#define ROM_128KB       0x02
#define ROM_256KB       0x03
#define ROM_512KB       0x04
#define ROM_1MB         0x05
#define ROM_2MB         0x06
#define ROM_4MB         0x07
#define ROM_8MB         0x08
#define ROM_1_1MB       0x52
#define ROM_1_2MB       0x53
#define ROM_1_5MB       0x54

/*
0x0149 - RAM Size
Specifies the size of the external RAM in the cartridge (if any).
00h - None
01h - 2 KBytes
02h - 8 Kbytes
03h - 32 KBytes (4 banks of 8KBytes each)
When using a MBC2 chip 00h must be specified in this entry, even though the MBC2 includes a built-in
RAM of 512 x 4 bits.
*/
#define RAM_None        0x00
#define RAM_2KB         0x01
#define RAM_8KB         0x02
#define RAM_32KB        0x03
#define RAM_128KB       0x04
#define RAM_64KB        0x05

#include <vector>

class Cartridge : public IMemoryUnit
{
public:
    Cartridge();
    ~Cartridge();

    bool LoadROM(const char* path);
    bool LoadROM(
        const byte* data,
        size_t size,
        const char* persistencePath = nullptr,
        bool managePersistentData = false);
    void Step(unsigned long cycles);
    void Serialize(StateSerializer& state);

    // Raw cartridge CGB flag (0x0143). Returns 0x00 when no ROM is loaded so
    // model resolution falls back to DMG rather than to an undefined value.
    byte GetCGBFlag() const;
    bool IsCGBCartridge() const;
    bool IsCGBOnlyCartridge() const;
    byte* GetSaveRAM() { return m_RAM.get(); }
    const byte* GetSaveRAM() const { return m_RAM.get(); }
    size_t GetSaveRAMSize() const { return m_RAMSize; }
    unsigned long long GetROMHash() const;
    const byte* GetROM() const { return m_ROM.get(); }
    size_t GetROMSize() const { return m_ROMSize; }
    byte* GetRTCData();
    size_t GetRTCDataSize() const;
    bool IsRumbleEnabled() const;
    bool HasBattery() const { return m_HasBattery; }
    void ClearROMPatches();
    void ApplyROMPatch(byte value, ushort address, int compareValue);

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    struct ROMPatch
    {
        byte Value;
        ushort Address;
        int CompareValue;
    };

    bool LoadMBC(unsigned int actualSize);
    bool IsMBC1Multicart() const;
    void LoadPersistentData();
    void SavePersistentData();

private:
    std::string m_Path;
    byte m_MBCType;
    unsigned int m_ROMSize;
    unsigned int m_RAMSize;
    bool m_HasBattery;
    bool m_HasRTC;
    bool m_ManagePersistentData;
    unsigned long long m_ROMHash;
    std::unique_ptr<byte[]> m_ROM;
    std::unique_ptr<byte[]> m_RAM;
    std::unique_ptr<IMemoryUnit> m_MBC;
    std::vector<ROMPatch> m_ROMPatches;
};
