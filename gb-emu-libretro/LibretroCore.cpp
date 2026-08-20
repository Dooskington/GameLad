#include "libretro.h"

#include "pch.hpp"
#include "Cartridge.hpp"
#include "Emulator.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
const unsigned int FrameWidth = 160;
const unsigned int FrameHeight = 144;
const unsigned int CyclesPerFrame = 70224;
const double GameBoyClockHz = 4194304.0;
const double FramesPerSecond = GameBoyClockHz / CyclesPerFrame;
const unsigned int AudioSampleRate = 44100;
const size_t AudioChunkFrames = 2048;
const size_t CoreStateHeaderSize = 17;
const unsigned int MaxConsecutiveFrameskips = 4;

retro_environment_t g_environmentCallback = nullptr;
retro_video_refresh_t g_videoCallback = nullptr;
retro_audio_sample_t g_audioSampleCallback = nullptr;
retro_audio_sample_batch_t g_audioBatchCallback = nullptr;
retro_input_poll_t g_inputPollCallback = nullptr;
retro_input_state_t g_inputStateCallback = nullptr;
retro_log_printf_t g_logCallback = nullptr;
const retro_vfs_interface* g_vfs = nullptr;
retro_rumble_interface g_rumbleInterface = {};

std::unique_ptr<Emulator> g_emulator;
std::vector<byte> g_romData;
std::vector<byte> g_bootROMData;
std::string g_gamePath;
unsigned long long g_nextFrameCycle = 0;
size_t g_emulatorStateSize = 0;
bool g_gameLoaded = false;
bool g_controllerEnabled = true;
bool g_supportsInputBitmasks = false;
bool g_canDupe = false;
bool g_turboPhase = false;
bool g_rumbleActive = false;

const std::uint16_t NoNetpacketPeer = 0xFFFF;
retro_netpacket_send_t g_netpacketSend = nullptr;
retro_netpacket_poll_receive_t g_netpacketPollReceive = nullptr;
std::uint16_t g_netpacketPeer = NoNetpacketPeer;
bool g_netpacketActive = false;
bool g_netpacketAwaitingResponse = false;
bool g_netpacketResponseReady = false;
byte g_netpacketSequence = 0;
byte g_netpacketPendingSequence = 0;
bool g_netpacketResponseBit = true;

ModelPreference g_modelPreference = ModelPreference::Auto;
bool g_useBootROM = false;
bool g_allowOpposingDirections = false;

enum class FrameskipMode
{
    Disabled,
    Auto,
    Manual
};

FrameskipMode g_frameskipMode = FrameskipMode::Disabled;
unsigned int g_frameskipThreshold = 33;
unsigned int g_consecutiveFrameskips = 0;
bool g_audioBufferActive = false;
unsigned int g_audioBufferOccupancy = 100;
bool g_audioBufferUnderrunLikely = false;
unsigned int g_audioLatency = 0;

std::array<uint32_t, FrameWidth * FrameHeight> g_videoBuffer;
std::array<float, AudioChunkFrames * 2> g_floatAudioBuffer;
std::array<int16_t, AudioChunkFrames * 2> g_retroAudioBuffer;
std::vector<retro_memory_descriptor> g_memoryDescriptors;

struct CheatSlot
{
    bool Enabled;
    std::string Code;
};

struct GameSharkCode
{
    byte Value;
    ushort Address;
};

std::map<unsigned int, CheatSlot> g_cheatSlots;
std::vector<GameSharkCode> g_gameSharkCodes;

retro_core_option_v2_category CoreOptionCategories[] =
{
    { "system", "System", "Hardware and boot behavior." },
    { "input", "Input", "Game Boy input behavior." },
    { "performance", "Performance", "Frontend performance and frame skipping." },
    { nullptr, nullptr, nullptr }
};

retro_core_option_v2_definition CoreOptionDefinitions[] =
{
    {
        "gamelad_model",
        "Hardware Model",
        "Model",
        "Select the emulated console. Auto uses DMG unless a game requires CGB. "
        "Close and reload content to apply changes.",
        nullptr,
        "system",
        {
            { "auto", "Auto" },
            { "dmg", "Game Boy (DMG)" },
            { "cgb", "Game Boy Color (CGB)" },
            { nullptr, nullptr }
        },
        "auto"
    },
    {
        "gamelad_boot_rom",
        "Use Official Boot ROM",
        "Boot ROM",
        "Run gb_bios.bin or gbc_bios.bin from RetroArch's System directory. "
        "The matching file must be present. Close and reload content to apply changes.",
        nullptr,
        "system",
        {
            { "disabled", "Disabled" },
            { "enabled", "Enabled" },
            { nullptr, nullptr }
        },
        "disabled"
    },
    {
        "gamelad_allow_opposing_directions",
        "Allow Opposing Directions",
        "Opposing Directions",
        "Allow Up+Down and Left+Right to be pressed simultaneously.",
        nullptr,
        "input",
        {
            { "disabled", "Disabled" },
            { "enabled", "Enabled" },
            { nullptr, nullptr }
        },
        "disabled"
    },
    {
        "gamelad_frameskip",
        "Frameskip",
        "Frameskip",
        "Skip video frames when the frontend audio buffer is at risk of underrunning.",
        nullptr,
        "performance",
        {
            { "disabled", "Disabled" },
            { "auto", "Auto" },
            { "manual", "Manual" },
            { nullptr, nullptr }
        },
        "disabled"
    },
    {
        "gamelad_frameskip_threshold",
        "Frameskip Threshold (%)",
        "Threshold (%)",
        "When Manual Frameskip is enabled, skip video while frontend audio-buffer "
        "occupancy is below this percentage.",
        nullptr,
        "performance",
        {
            { "15", nullptr },
            { "18", nullptr },
            { "21", nullptr },
            { "24", nullptr },
            { "27", nullptr },
            { "30", nullptr },
            { "33", nullptr },
            { "36", nullptr },
            { "40", nullptr },
            { "45", nullptr },
            { "50", nullptr },
            { "55", nullptr },
            { "60", nullptr },
            { nullptr, nullptr }
        },
        "33"
    },
    {}
};

retro_core_options_v2 CoreOptions =
{
    CoreOptionCategories,
    CoreOptionDefinitions
};

const retro_variable LegacyOptions[] =
{
    { "gamelad_model", "Hardware Model; auto|dmg|cgb" },
    { "gamelad_boot_rom", "Use Official Boot ROM; disabled|enabled" },
    {
        "gamelad_allow_opposing_directions",
        "Allow Opposing Directions; disabled|enabled"
    },
    { "gamelad_frameskip", "Frameskip; disabled|auto|manual" },
    {
        "gamelad_frameskip_threshold",
        "Frameskip Threshold (%); 33|15|18|21|24|27|30|36|40|45|50|55|60"
    },
    { nullptr, nullptr }
};

const retro_input_descriptor InputDescriptors[] =
{
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "D-Pad Up" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "D-Pad Down" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "D-Pad Left" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "A" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "B" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Turbo A" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Turbo B" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
    { 0, 0, 0, 0, nullptr }
};

const retro_controller_description ControllerDescriptions[] =
{
    { "Game Boy", RETRO_DEVICE_JOYPAD }
};

const retro_controller_info ControllerInfo[] =
{
    { ControllerDescriptions, 1 },
    { nullptr, 0 }
};

const retro_system_content_info_override ContentOverrides[] =
{
    { "gb|gbc", false, false },
    { nullptr, false, false }
};

void Log(enum retro_log_level level, const char* message)
{
    if (g_logCallback != nullptr)
    {
        g_logCallback(level, "GameLad: %s\n", message);
    }
}

void ShowMessage(const char* message, enum retro_log_level level)
{
    if (g_environmentCallback == nullptr || message == nullptr)
    {
        return;
    }

    unsigned int version = 0;
    if (g_environmentCallback(
            RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION,
            &version) &&
        version >= 1)
    {
        retro_message_ext notification = {};
        notification.msg = message;
        notification.duration = 4000;
        notification.priority = level >= RETRO_LOG_ERROR ? 3 : 1;
        notification.level = level;
        notification.target = RETRO_MESSAGE_TARGET_OSD;
        notification.type = RETRO_MESSAGE_TYPE_NOTIFICATION;
        notification.progress = -1;
        g_environmentCallback(RETRO_ENVIRONMENT_SET_MESSAGE_EXT, &notification);
    }
    else
    {
        retro_message notification = { message, 240 };
        g_environmentCallback(RETRO_ENVIRONMENT_SET_MESSAGE, &notification);
    }
}

const char* GetOption(const char* key)
{
    if (g_environmentCallback == nullptr)
    {
        return nullptr;
    }

    retro_variable option = { key, nullptr };
    return g_environmentCallback(RETRO_ENVIRONMENT_GET_VARIABLE, &option)
        ? option.value
        : nullptr;
}

bool OptionEquals(const char* key, const char* value)
{
    const char* selected = GetOption(key);
    return selected != nullptr && std::strcmp(selected, value) == 0;
}

void ReadStartupOptions()
{
    if (OptionEquals("gamelad_model", "dmg"))
    {
        g_modelPreference = ModelPreference::ForceDMG;
    }
    else if (OptionEquals("gamelad_model", "cgb"))
    {
        g_modelPreference = ModelPreference::ForceCGB;
    }
    else
    {
        g_modelPreference = ModelPreference::Auto;
    }

    g_useBootROM = OptionEquals("gamelad_boot_rom", "enabled");
}

void ReadRuntimeOptions()
{
    g_allowOpposingDirections =
        OptionEquals("gamelad_allow_opposing_directions", "enabled");

    if (OptionEquals("gamelad_frameskip", "auto"))
    {
        g_frameskipMode = FrameskipMode::Auto;
    }
    else if (OptionEquals("gamelad_frameskip", "manual"))
    {
        g_frameskipMode = FrameskipMode::Manual;
    }
    else
    {
        g_frameskipMode = FrameskipMode::Disabled;
    }

    const char* threshold = GetOption("gamelad_frameskip_threshold");
    if (threshold != nullptr)
    {
        const int parsed = std::atoi(threshold);
        if (parsed >= 0 && parsed <= 100)
        {
            g_frameskipThreshold = static_cast<unsigned int>(parsed);
        }
    }
}

bool UpdateOptionVisibility()
{
    if (g_environmentCallback == nullptr)
    {
        return false;
    }

    retro_core_option_display display = {};
    display.key = "gamelad_frameskip_threshold";
    display.visible = OptionEquals("gamelad_frameskip", "manual");
    return g_environmentCallback(
        RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY,
        &display);
}

void RegisterCoreOptions()
{
    if (g_environmentCallback == nullptr)
    {
        return;
    }

    unsigned int version = 0;
    if (g_environmentCallback(
            RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION,
            &version) &&
        version >= 2)
    {
        g_environmentCallback(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2, &CoreOptions);
    }
    else
    {
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_VARIABLES,
            const_cast<retro_variable*>(LegacyOptions));
    }

    retro_core_options_update_display_callback visibility = {};
    visibility.callback = UpdateOptionVisibility;
    g_environmentCallback(
        RETRO_ENVIRONMENT_SET_CORE_OPTIONS_UPDATE_DISPLAY_CALLBACK,
        &visibility);
}

std::string JoinPath(const char* directory, const char* filename)
{
    if (directory == nullptr || directory[0] == '\0')
    {
        return "";
    }

    std::string path(directory);
    std::replace(path.begin(), path.end(), '\\', '/');
    if (path[path.size() - 1] != '/' && path[path.size() - 1] != '\\')
    {
        path += '/';
    }
    path += filename;
    return path;
}

bool ReadFile(const std::string& path, std::vector<byte>& data)
{
    data.clear();
    if (path.empty())
    {
        return false;
    }

    if (g_vfs != nullptr)
    {
        retro_vfs_file_handle* file = g_vfs->open(
            path.c_str(),
            RETRO_VFS_FILE_ACCESS_READ,
            RETRO_VFS_FILE_ACCESS_HINT_SEQUENTIAL_BULK);
        if (file != nullptr)
        {
            const int64_t size = g_vfs->size(file);
            if (size > 0 && size <= 64 * 1024 * 1024)
            {
                data.resize(static_cast<size_t>(size));
                const int64_t read = g_vfs->read(file, data.data(), data.size());
                g_vfs->close(file);
                if (read == size)
                {
                    return true;
                }
                data.clear();
                return false;
            }
            g_vfs->close(file);
        }
    }

    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    const std::streampos streamSize = file.tellg();
    if (streamSize <= 0 ||
        streamSize > static_cast<std::streampos>(64 * 1024 * 1024))
    {
        return false;
    }

    data.resize(static_cast<size_t>(streamSize));
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(data.data()), data.size()))
    {
        data.clear();
        return false;
    }
    return true;
}

bool PrepareBootROM()
{
    g_bootROMData.clear();
    if (!g_useBootROM)
    {
        return true;
    }

    if (g_romData.size() <= CGBFlagAddress)
    {
        return false;
    }

    const GameBoyMode mode =
        ResolveGameBoyMode(g_modelPreference, g_romData[CGBFlagAddress]);
    const bool cgb = IsCGBHardware(mode);
    const char* filename = cgb ? "gbc_bios.bin" : "gb_bios.bin";
    const size_t expectedSize = cgb ? 0x0900 : 0x0100;

    const char* systemDirectory = nullptr;
    if (g_environmentCallback == nullptr ||
        !g_environmentCallback(
            RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY,
            &systemDirectory))
    {
        ShowMessage("GameLad could not query RetroArch's System directory.", RETRO_LOG_ERROR);
        return false;
    }

    const std::string path = JoinPath(systemDirectory, filename);
    if (!ReadFile(path, g_bootROMData) || g_bootROMData.size() != expectedSize)
    {
        const char* message = cgb
            ? "GameLad requires a 2304-byte gbc_bios.bin in the System directory."
            : "GameLad requires a 256-byte gb_bios.bin in the System directory.";
        ShowMessage(message, RETRO_LOG_ERROR);
        Log(RETRO_LOG_ERROR, message);
        g_bootROMData.clear();
        return false;
    }

    ShowMessage(cgb ? "GameLad loaded the CGB boot ROM." : "GameLad loaded the DMG boot ROM.",
        RETRO_LOG_INFO);
    return true;
}

void SetRumble(bool enabled)
{
    if (g_rumbleActive == enabled)
    {
        return;
    }

    g_rumbleActive = enabled;
    if (g_rumbleInterface.set_rumble_state != nullptr)
    {
        g_rumbleInterface.set_rumble_state(
            0,
            RETRO_RUMBLE_STRONG,
            enabled ? 0xFFFF : 0);
    }
}

bool ExchangeSerialBit(void*, bool outgoingBit, bool& incomingBit)
{
    if (!g_netpacketActive ||
        g_netpacketSend == nullptr ||
        g_netpacketPeer == NoNetpacketPeer)
    {
        incomingBit = true;
        return true;
    }

    if (g_netpacketAwaitingResponse &&
        g_netpacketResponseReady)
    {
        incomingBit = g_netpacketResponseBit;
        g_netpacketAwaitingResponse = false;
        g_netpacketResponseReady = false;
        return true;
    }

    if (!g_netpacketAwaitingResponse)
    {
        const byte packet[4] =
        {
            0x47,
            0x01,
            g_netpacketSequence++,
            outgoingBit ? static_cast<byte>(1) : static_cast<byte>(0)
        };
        g_netpacketPendingSequence = packet[2];
        g_netpacketAwaitingResponse = true;
        g_netpacketSend(
            RETRO_NETPACKET_RELIABLE | RETRO_NETPACKET_FLUSH_HINT,
            packet,
            sizeof(packet),
            g_netpacketPeer);
    }

    if (g_netpacketPollReceive != nullptr)
    {
        g_netpacketPollReceive();
    }
    if (g_netpacketResponseReady)
    {
        incomingBit = g_netpacketResponseBit;
        g_netpacketAwaitingResponse = false;
        g_netpacketResponseReady = false;
        return true;
    }
    return false;
}

void NetpacketStart(
    std::uint16_t clientId,
    retro_netpacket_send_t send,
    retro_netpacket_poll_receive_t pollReceive)
{
    g_netpacketSend = send;
    g_netpacketPollReceive = pollReceive;
    g_netpacketPeer = clientId == 0 ? NoNetpacketPeer : 0;
    g_netpacketActive = true;
    g_netpacketAwaitingResponse = false;
    g_netpacketResponseReady = false;
    if (g_emulator != nullptr)
    {
        g_emulator->SetSerialLinkCallback(ExchangeSerialBit, nullptr);
    }
}

void NetpacketReceive(
    const void* buffer,
    size_t length,
    std::uint16_t clientId)
{
    if (!g_netpacketActive ||
        buffer == nullptr ||
        length != 4 ||
        g_netpacketSend == nullptr)
    {
        return;
    }

    const byte* packet = static_cast<const byte*>(buffer);
    if (packet[0] != 0x47 || packet[3] > 1)
    {
        return;
    }

    if (packet[1] == 0x01)
    {
        bool outgoingBit = true;
        if (g_emulator != nullptr)
        {
            g_emulator->ClockExternalSerialBit(
                packet[3] != 0,
                outgoingBit);
        }

        const byte response[4] =
        {
            0x47,
            0x02,
            packet[2],
            outgoingBit ? static_cast<byte>(1) : static_cast<byte>(0)
        };
        g_netpacketSend(
            RETRO_NETPACKET_RELIABLE | RETRO_NETPACKET_FLUSH_HINT,
            response,
            sizeof(response),
            clientId);
    }
    else if (packet[1] == 0x02 &&
             g_netpacketAwaitingResponse &&
             packet[2] == g_netpacketPendingSequence &&
             (g_netpacketPeer == NoNetpacketPeer ||
              clientId == g_netpacketPeer))
    {
        g_netpacketResponseBit = packet[3] != 0;
        g_netpacketResponseReady = true;
    }
}

void NetpacketStop()
{
    if (g_emulator != nullptr)
    {
        g_emulator->SetSerialLinkCallback(nullptr, nullptr);
    }
    g_netpacketSend = nullptr;
    g_netpacketPollReceive = nullptr;
    g_netpacketPeer = NoNetpacketPeer;
    g_netpacketActive = false;
    g_netpacketAwaitingResponse = false;
    g_netpacketResponseReady = false;
}

bool NetpacketConnected(std::uint16_t clientId)
{
    if (g_netpacketPeer != NoNetpacketPeer)
    {
        return false;
    }
    g_netpacketPeer = clientId;
    return true;
}

void NetpacketDisconnected(std::uint16_t clientId)
{
    if (clientId == g_netpacketPeer)
    {
        g_netpacketPeer = NoNetpacketPeer;
        g_netpacketAwaitingResponse = false;
        g_netpacketResponseReady = false;
    }
}

void ResetNetpacketTransaction()
{
    g_netpacketAwaitingResponse = false;
    g_netpacketResponseReady = false;
    ++g_netpacketSequence;
}

const retro_netpacket_callback NetpacketCallbacks =
{
    NetpacketStart,
    NetpacketReceive,
    NetpacketStop,
    nullptr,
    NetpacketConnected,
    NetpacketDisconnected,
    "1"
};

void AddMemoryDescriptor(
    uint64_t flags,
    void* pointer,
    size_t offset,
    size_t start,
    size_t length,
    const char* addressSpace = nullptr)
{
    retro_memory_descriptor descriptor = {};
    descriptor.flags = flags;
    descriptor.ptr = pointer;
    descriptor.offset = offset;
    descriptor.start = start;
    descriptor.len = length;
    descriptor.addrspace = addressSpace;
    g_memoryDescriptors.push_back(descriptor);
}

void RegisterMemoryMaps()
{
    g_memoryDescriptors.clear();
    if (g_emulator == nullptr || g_environmentCallback == nullptr)
    {
        return;
    }

    const byte* rom = g_emulator->GetROM();
    const size_t romSize = g_emulator->GetROMSize();
    if (rom != nullptr && romSize >= 0x4000)
    {
        AddMemoryDescriptor(
            RETRO_MEMDESC_CONST,
            const_cast<byte*>(rom),
            0,
            0x0000,
            0x4000);
        if (romSize >= 0x8000)
        {
            AddMemoryDescriptor(
                RETRO_MEMDESC_CONST,
                const_cast<byte*>(rom),
                0x4000,
                0x4000,
                0x4000);
        }
    }

    byte* videoRAM = g_emulator->GetVideoRAM();
    if (videoRAM != nullptr)
    {
        AddMemoryDescriptor(
            RETRO_MEMDESC_VIDEO_RAM,
            videoRAM,
            0,
            0x8000,
            0x2000);
        if (IsCGBHardware(g_emulator->GetGameBoyMode()) &&
            g_emulator->GetVideoRAMSize() > 0x2000)
        {
            AddMemoryDescriptor(
                RETRO_MEMDESC_VIDEO_RAM,
                videoRAM,
                0x2000,
                0x36000,
                0x2000);
        }
    }

    byte* saveRAM = g_emulator->GetSaveRAM();
    const size_t saveRAMSize = g_emulator->GetSaveRAMSize();
    if (saveRAM != nullptr && saveRAMSize != 0)
    {
        AddMemoryDescriptor(
            RETRO_MEMDESC_SAVE_RAM,
            saveRAM,
            0,
            0xA000,
            (std::min)(saveRAMSize, static_cast<size_t>(0x2000)));
        for (size_t offset = 0x2000; offset < saveRAMSize; offset += 0x2000)
        {
            AddMemoryDescriptor(
                RETRO_MEMDESC_SAVE_RAM,
                saveRAM,
                offset,
                0x16000 + offset - 0x2000,
                (std::min)(static_cast<size_t>(0x2000), saveRAMSize - offset));
        }
    }

    byte* workRAM = g_emulator->GetWorkRAM();
    if (workRAM != nullptr)
    {
        AddMemoryDescriptor(
            RETRO_MEMDESC_SYSTEM_RAM,
            workRAM,
            0,
            0xC000,
            0x1000);
        AddMemoryDescriptor(
            RETRO_MEMDESC_SYSTEM_RAM,
            workRAM,
            0x1000,
            0xD000,
            0x1000);
        if (IsCGBHardware(g_emulator->GetGameBoyMode()))
        {
            for (size_t bank = 2; bank < 8; ++bank)
            {
                AddMemoryDescriptor(
                    RETRO_MEMDESC_SYSTEM_RAM,
                    workRAM,
                    bank * 0x1000,
                    0x10000 + ((bank - 2) * 0x1000),
                    0x1000);
            }
        }
    }

    byte* oam = g_emulator->GetOAM();
    if (oam != nullptr)
    {
        AddMemoryDescriptor(0, oam, 0, 0xFE00, 0x80);
        AddMemoryDescriptor(0, oam, 0x80, 0xFE80, 0x20);
    }

    byte* highRAM = g_emulator->GetHighRAM();
    if (highRAM != nullptr)
    {
        const size_t lengths[] = { 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
        size_t offset = 0;
        for (size_t index = 0; index < ARRAYSIZE(lengths); ++index)
        {
            AddMemoryDescriptor(
                RETRO_MEMDESC_SYSTEM_RAM,
                highRAM,
                offset,
                0xFF80 + offset,
                lengths[index]);
            offset += lengths[index];
        }
    }

    retro_memory_map memoryMap = {};
    memoryMap.descriptors = g_memoryDescriptors.data();
    memoryMap.num_descriptors =
        static_cast<unsigned int>(g_memoryDescriptors.size());
    g_environmentCallback(RETRO_ENVIRONMENT_SET_MEMORY_MAPS, &memoryMap);

    bool achievements = true;
    g_environmentCallback(
        RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS,
        &achievements);
}

int HexDigit(char value)
{
    if (value >= '0' && value <= '9')
    {
        return value - '0';
    }
    value = static_cast<char>(std::toupper(static_cast<unsigned char>(value)));
    return value >= 'A' && value <= 'F' ? value - 'A' + 10 : -1;
}

bool DecodeGameGenie(const std::string& code, byte& value, ushort& address, int& compare)
{
    if (code.size() != 7 && code.size() != 11)
    {
        return false;
    }

    const int digits[] =
    {
        HexDigit(code[0]), HexDigit(code[1]), HexDigit(code[2]),
        HexDigit(code[4]), HexDigit(code[5]), HexDigit(code[6])
    };
    for (size_t index = 0; index < ARRAYSIZE(digits); ++index)
    {
        if (digits[index] < 0)
        {
            return false;
        }
    }
    if (code[3] != '-' || (code.size() == 11 && code[7] != '-'))
    {
        return false;
    }

    value = static_cast<byte>((digits[0] << 4) | digits[1]);
    address = static_cast<ushort>(
        (digits[2] << 8) |
        (digits[3] << 4) |
        digits[4] |
        ((digits[5] ^ 0x0F) << 12));
    address &= 0x7FFF;
    compare = -1;

    if (code.size() == 11)
    {
        const int high = HexDigit(code[8]);
        const int low = HexDigit(code[10]);
        if (high < 0 || low < 0 || HexDigit(code[9]) < 0)
        {
            return false;
        }
        unsigned int decoded = static_cast<unsigned int>(((high << 4) | low) ^ 0xFF);
        decoded = ((decoded >> 2) | (decoded << 6)) ^ 0x45;
        compare = static_cast<int>(decoded & 0xFF);
    }
    return true;
}

bool DecodeGameShark(const std::string& code, GameSharkCode& decoded)
{
    if (code.size() != 8)
    {
        return false;
    }

    int digits[8] = {};
    for (size_t index = 0; index < ARRAYSIZE(digits); ++index)
    {
        digits[index] = HexDigit(code[index]);
        if (digits[index] < 0)
        {
            return false;
        }
    }

    const byte type = static_cast<byte>((digits[0] << 4) | digits[1]);
    if (type != 0x01)
    {
        return false;
    }

    decoded.Value = static_cast<byte>((digits[2] << 4) | digits[3]);
    decoded.Address = static_cast<ushort>(
        (digits[4] << 4) |
        digits[5] |
        (digits[6] << 12) |
        (digits[7] << 8));
    return true;
}

std::string NormalizeCheatCode(const std::string& code)
{
    std::string normalized;
    for (size_t index = 0; index < code.size(); ++index)
    {
        const unsigned char value = static_cast<unsigned char>(code[index]);
        if (!std::isspace(value))
        {
            normalized += static_cast<char>(std::toupper(value));
        }
    }
    return normalized;
}

void RebuildCheats()
{
    g_gameSharkCodes.clear();
    if (g_emulator == nullptr)
    {
        return;
    }

    g_emulator->ClearROMPatches();
    for (std::map<unsigned int, CheatSlot>::const_iterator slot = g_cheatSlots.begin();
         slot != g_cheatSlots.end();
         ++slot)
    {
        if (!slot->second.Enabled)
        {
            continue;
        }

        const std::string& allCodes = slot->second.Code;
        size_t position = 0;
        while (position <= allCodes.size())
        {
            const size_t separator = allCodes.find_first_of("+;", position);
            const std::string code = NormalizeCheatCode(allCodes.substr(
                position,
                separator == std::string::npos
                    ? std::string::npos
                    : separator - position));
            if (!code.empty())
            {
                if (code.find('-') != std::string::npos)
                {
                    byte value = 0;
                    ushort address = 0;
                    int compare = -1;
                    if (DecodeGameGenie(code, value, address, compare))
                    {
                        g_emulator->ApplyROMPatch(value, address, compare);
                    }
                    else
                    {
                        Log(RETRO_LOG_WARN, "ignored an invalid Game Genie code");
                    }
                }
                else
                {
                    GameSharkCode decoded = {};
                    if (DecodeGameShark(code, decoded))
                    {
                        g_gameSharkCodes.push_back(decoded);
                    }
                    else
                    {
                        Log(RETRO_LOG_WARN, "ignored an invalid GameShark code");
                    }
                }
            }

            if (separator == std::string::npos)
            {
                break;
            }
            position = separator + 1;
        }
    }
}

void ApplyGameSharkCodes()
{
    if (g_emulator == nullptr)
    {
        return;
    }

    for (size_t index = 0; index < g_gameSharkCodes.size(); ++index)
    {
        g_emulator->WriteMemory(
            g_gameSharkCodes[index].Address,
            g_gameSharkCodes[index].Value);
    }
}

bool InitializeEmulator()
{
    std::unique_ptr<Emulator> emulator(new Emulator());
    if (!emulator->Initialize(
            g_bootROMData.empty() ? nullptr : g_bootROMData.data(),
            g_bootROMData.size(),
            g_romData.data(),
            g_romData.size(),
            g_modelPreference))
    {
        Log(RETRO_LOG_ERROR, "failed to initialize the emulator");
        return false;
    }

    emulator->SetAudioSampleRate(AudioSampleRate);
    std::vector<byte> state;
    if (!emulator->Serialize(state))
    {
        Log(RETRO_LOG_ERROR, "failed to determine the save-state size");
        return false;
    }

    SetRumble(false);
    g_emulator = std::move(emulator);
    if (g_netpacketActive)
    {
        g_emulator->SetSerialLinkCallback(ExchangeSerialBit, nullptr);
    }
    g_emulatorStateSize = state.size();
    g_nextFrameCycle = g_emulator->GetBaseClockCycles();
    g_consecutiveFrameskips = 0;
    RebuildCheats();
    RegisterMemoryMaps();
    return true;
}

bool ResetEmulatorInPlace()
{
    if (g_emulator == nullptr)
    {
        return false;
    }

    std::vector<byte> saveRAM;
    std::vector<byte> rtc;
    if (g_emulator->GetSaveRAM() != nullptr)
    {
        saveRAM.assign(
            g_emulator->GetSaveRAM(),
            g_emulator->GetSaveRAM() + g_emulator->GetSaveRAMSize());
    }
    if (g_emulator->GetRTCData() != nullptr)
    {
        rtc.assign(
            g_emulator->GetRTCData(),
            g_emulator->GetRTCData() + g_emulator->GetRTCDataSize());
    }

    Emulator initial;
    if (!initial.Initialize(
            g_bootROMData.empty() ? nullptr : g_bootROMData.data(),
            g_bootROMData.size(),
            g_romData.data(),
            g_romData.size(),
            g_modelPreference))
    {
        return false;
    }
    initial.SetAudioSampleRate(AudioSampleRate);

    std::vector<byte> initialState;
    if (!initial.Serialize(initialState) ||
        initialState.size() != g_emulatorStateSize ||
        !g_emulator->Deserialize(initialState.data(), initialState.size()))
    {
        return false;
    }

    if (!saveRAM.empty() && g_emulator->GetSaveRAM() != nullptr)
    {
        std::memcpy(
            g_emulator->GetSaveRAM(),
            saveRAM.data(),
            (std::min)(saveRAM.size(), g_emulator->GetSaveRAMSize()));
    }
    if (!rtc.empty() && g_emulator->GetRTCData() != nullptr)
    {
        std::memcpy(
            g_emulator->GetRTCData(),
            rtc.data(),
            (std::min)(rtc.size(), g_emulator->GetRTCDataSize()));
    }

    g_nextFrameCycle = g_emulator->GetBaseClockCycles();
    g_turboPhase = false;
    g_consecutiveFrameskips = 0;
    ResetNetpacketTransaction();
    SetRumble(false);
    RebuildCheats();
    return true;
}

void StopEmulator()
{
    SetRumble(false);
    if (g_emulator != nullptr)
    {
        g_emulator->Stop();
        g_emulator.reset();
    }

    g_nextFrameCycle = 0;
    g_emulatorStateSize = 0;
    g_memoryDescriptors.clear();
    g_gameSharkCodes.clear();
}

int16_t GetInputMask()
{
    if (!g_controllerEnabled || g_inputStateCallback == nullptr)
    {
        return 0;
    }

    if (g_supportsInputBitmasks)
    {
        return g_inputStateCallback(
            0,
            RETRO_DEVICE_JOYPAD,
            0,
            RETRO_DEVICE_ID_JOYPAD_MASK);
    }

    int16_t mask = 0;
    const unsigned int buttons[] =
    {
        RETRO_DEVICE_ID_JOYPAD_B,
        RETRO_DEVICE_ID_JOYPAD_Y,
        RETRO_DEVICE_ID_JOYPAD_SELECT,
        RETRO_DEVICE_ID_JOYPAD_START,
        RETRO_DEVICE_ID_JOYPAD_UP,
        RETRO_DEVICE_ID_JOYPAD_DOWN,
        RETRO_DEVICE_ID_JOYPAD_LEFT,
        RETRO_DEVICE_ID_JOYPAD_RIGHT,
        RETRO_DEVICE_ID_JOYPAD_A,
        RETRO_DEVICE_ID_JOYPAD_X
    };
    for (size_t index = 0; index < ARRAYSIZE(buttons); ++index)
    {
        if (g_inputStateCallback(
                0,
                RETRO_DEVICE_JOYPAD,
                0,
                buttons[index]) != 0)
        {
            mask |= static_cast<int16_t>(1 << buttons[index]);
        }
    }
    return mask;
}

bool MaskPressed(int16_t mask, unsigned int id)
{
    return (mask & static_cast<int16_t>(1 << id)) != 0;
}

void UpdateInput()
{
    if (g_inputPollCallback != nullptr)
    {
        g_inputPollCallback();
    }

    const int16_t mask = GetInputMask();
    bool up = MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_UP);
    bool down = MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_DOWN);
    bool left = MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_LEFT);
    bool right = MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_RIGHT);
    if (!g_allowOpposingDirections)
    {
        if (up && down)
        {
            up = false;
            down = false;
        }
        if (left && right)
        {
            left = false;
            right = false;
        }
    }

    byte directions = JOYPAD_NONE;
    byte buttons = JOYPAD_NONE;
    if (up) { directions |= JOYPAD_INPUT_UP; }
    if (down) { directions |= JOYPAD_INPUT_DOWN; }
    if (left) { directions |= JOYPAD_INPUT_LEFT; }
    if (right) { directions |= JOYPAD_INPUT_RIGHT; }

    const bool turboA =
        MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_X) && g_turboPhase;
    const bool turboB =
        MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_Y) && g_turboPhase;
    if (MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_A) || turboA)
    {
        buttons |= JOYPAD_BUTTONS_A;
    }
    if (MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_B) || turboB)
    {
        buttons |= JOYPAD_BUTTONS_B;
    }
    if (MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_START))
    {
        buttons |= JOYPAD_BUTTONS_START;
    }
    if (MaskPressed(mask, RETRO_DEVICE_ID_JOYPAD_SELECT))
    {
        buttons |= JOYPAD_BUTTONS_SELECT;
    }

    g_emulator->SetInput(directions, buttons);
}

void ConvertVideoFrame()
{
    const byte* source = g_emulator->GetCurrentFrame();
    for (size_t pixel = 0; pixel < g_videoBuffer.size(); ++pixel)
    {
        const size_t sourceIndex = pixel * 4;
        const uint32_t red = source[sourceIndex + 3];
        const uint32_t green = source[sourceIndex + 2];
        const uint32_t blue = source[sourceIndex + 1];
        g_videoBuffer[pixel] = (red << 16) | (green << 8) | blue;
    }
}

int16_t ConvertAudioSample(float sample)
{
    const float clamped = (std::max)(-1.0f, (std::min)(1.0f, sample));
    const float scale = clamped < 0.0f ? 32768.0f : 32767.0f;
    return static_cast<int16_t>(clamped * scale);
}

void SubmitAudioBatch(const int16_t* samples, size_t frames)
{
    if (g_audioBatchCallback != nullptr)
    {
        size_t submitted = 0;
        while (submitted < frames)
        {
            const size_t accepted =
                g_audioBatchCallback(samples + (submitted * 2), frames - submitted);
            if (accepted == 0)
            {
                break;
            }
            submitted += (std::min)(accepted, frames - submitted);
        }
    }
    else if (g_audioSampleCallback != nullptr)
    {
        for (size_t frame = 0; frame < frames; ++frame)
        {
            g_audioSampleCallback(samples[frame * 2], samples[(frame * 2) + 1]);
        }
    }
}

void FlushAudio(bool submit)
{
    for (;;)
    {
        const size_t frames = g_emulator->ConsumeAudioSamples(
            g_floatAudioBuffer.data(),
            AudioChunkFrames);
        if (frames == 0)
        {
            break;
        }

        if (submit)
        {
            for (size_t sample = 0; sample < frames * 2; ++sample)
            {
                g_retroAudioBuffer[sample] =
                    ConvertAudioSample(g_floatAudioBuffer[sample]);
            }
            SubmitAudioBatch(g_retroAudioBuffer.data(), frames);
        }

        if (frames < AudioChunkFrames)
        {
            break;
        }
    }
}

void AudioBufferStatus(
    bool active,
    unsigned int occupancy,
    bool underrunLikely)
{
    g_audioBufferActive = active;
    g_audioBufferOccupancy = occupancy;
    g_audioBufferUnderrunLikely = underrunLikely;
}

bool ShouldSkipVideo()
{
    if (!g_canDupe ||
        g_frameskipMode == FrameskipMode::Disabled ||
        !g_audioBufferActive)
    {
        g_consecutiveFrameskips = 0;
        return false;
    }

    const bool requested =
        (g_frameskipMode == FrameskipMode::Auto && g_audioBufferUnderrunLikely) ||
        (g_frameskipMode == FrameskipMode::Manual &&
         g_audioBufferOccupancy < g_frameskipThreshold);
    if (!requested || g_consecutiveFrameskips >= MaxConsecutiveFrameskips)
    {
        g_consecutiveFrameskips = 0;
        return false;
    }

    ++g_consecutiveFrameskips;
    return true;
}

void UpdateAudioLatency()
{
    const unsigned int requested =
        g_frameskipMode == FrameskipMode::Disabled ? 0 : 128;
    if (requested != g_audioLatency && g_environmentCallback != nullptr)
    {
        unsigned int latency = requested;
        if (g_environmentCallback(
                RETRO_ENVIRONMENT_SET_MINIMUM_AUDIO_LATENCY,
                &latency))
        {
            g_audioLatency = requested;
        }
    }
}

void CheckUpdatedOptions()
{
    if (g_environmentCallback == nullptr)
    {
        return;
    }

    bool updated = false;
    if (g_environmentCallback(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) &&
        updated)
    {
        ReadRuntimeOptions();
        UpdateOptionVisibility();
    }
    UpdateAudioLatency();
}

void WriteUInt32(byte* output, std::uint32_t value)
{
    for (size_t index = 0; index < 4; ++index)
    {
        output[index] = static_cast<byte>((value >> (index * 8)) & 0xFF);
    }
}

std::uint32_t ReadUInt32(const byte* input)
{
    std::uint32_t value = 0;
    for (size_t index = 0; index < 4; ++index)
    {
        value |= static_cast<std::uint32_t>(input[index]) << (index * 8);
    }
    return value;
}
}

extern "C"
{
RETRO_API void retro_set_environment(retro_environment_t callback)
{
    g_environmentCallback = callback;
    RegisterCoreOptions();
    if (g_environmentCallback != nullptr)
    {
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS,
            const_cast<retro_input_descriptor*>(InputDescriptors));
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_CONTROLLER_INFO,
            const_cast<retro_controller_info*>(ControllerInfo));
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_CONTENT_INFO_OVERRIDE,
            const_cast<retro_system_content_info_override*>(ContentOverrides));
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_NETPACKET_INTERFACE,
            const_cast<retro_netpacket_callback*>(&NetpacketCallbacks));
    }
}

RETRO_API void retro_set_video_refresh(retro_video_refresh_t callback)
{
    g_videoCallback = callback;
}

RETRO_API void retro_set_audio_sample(retro_audio_sample_t callback)
{
    g_audioSampleCallback = callback;
}

RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t callback)
{
    g_audioBatchCallback = callback;
}

RETRO_API void retro_set_input_poll(retro_input_poll_t callback)
{
    g_inputPollCallback = callback;
}

RETRO_API void retro_set_input_state(retro_input_state_t callback)
{
    g_inputStateCallback = callback;
}

RETRO_API void retro_init(void)
{
    if (g_environmentCallback == nullptr)
    {
        return;
    }

    retro_log_callback logging = {};
    if (g_environmentCallback(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
    {
        g_logCallback = logging.log;
    }

    retro_vfs_interface_info vfs = {};
    vfs.required_interface_version = 1;
    if (g_environmentCallback(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs))
    {
        g_vfs = vfs.iface;
    }

    g_supportsInputBitmasks =
        g_environmentCallback(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, nullptr);
    g_environmentCallback(RETRO_ENVIRONMENT_GET_CAN_DUPE, &g_canDupe);
    g_environmentCallback(
        RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE,
        &g_rumbleInterface);

    retro_audio_buffer_status_callback audioStatus = {};
    audioStatus.callback = AudioBufferStatus;
    g_environmentCallback(
        RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK,
        &audioStatus);

    std::uint64_t serializationQuirks =
        RETRO_SERIALIZATION_QUIRK_PLATFORM_DEPENDENT;
    g_environmentCallback(
        RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS,
        &serializationQuirks);

    ReadStartupOptions();
    ReadRuntimeOptions();
    UpdateOptionVisibility();
}

RETRO_API void retro_deinit(void)
{
    NetpacketStop();
    StopEmulator();
    if (g_audioLatency != 0 && g_environmentCallback != nullptr)
    {
        unsigned int latency = 0;
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_MINIMUM_AUDIO_LATENCY,
            &latency);
    }
    if (g_environmentCallback != nullptr)
    {
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK,
            nullptr);
        g_environmentCallback(
            RETRO_ENVIRONMENT_SET_CORE_OPTIONS_UPDATE_DISPLAY_CALLBACK,
            nullptr);
    }

    g_gameLoaded = false;
    std::vector<byte>().swap(g_romData);
    std::vector<byte>().swap(g_bootROMData);
    std::string().swap(g_gamePath);
    std::vector<retro_memory_descriptor>().swap(g_memoryDescriptors);
    std::vector<GameSharkCode>().swap(g_gameSharkCodes);
    g_cheatSlots.clear();

    g_nextFrameCycle = 0;
    g_emulatorStateSize = 0;
    g_controllerEnabled = true;
    g_supportsInputBitmasks = false;
    g_canDupe = false;
    g_turboPhase = false;
    g_rumbleActive = false;
    g_modelPreference = ModelPreference::Auto;
    g_useBootROM = false;
    g_allowOpposingDirections = false;
    g_frameskipMode = FrameskipMode::Disabled;
    g_frameskipThreshold = 33;
    g_consecutiveFrameskips = 0;
    g_audioBufferActive = false;
    g_audioBufferOccupancy = 100;
    g_audioBufferUnderrunLikely = false;
    g_audioLatency = 0;
    g_netpacketSequence = 0;
    g_netpacketPendingSequence = 0;
    g_netpacketResponseBit = true;

    g_vfs = nullptr;
    g_rumbleInterface = {};
    g_logCallback = nullptr;
    g_videoCallback = nullptr;
    g_audioSampleCallback = nullptr;
    g_audioBatchCallback = nullptr;
    g_inputPollCallback = nullptr;
    g_inputStateCallback = nullptr;
    g_environmentCallback = nullptr;
}

RETRO_API unsigned int retro_api_version(void)
{
    return RETRO_API_VERSION;
}

RETRO_API void retro_get_system_info(struct retro_system_info* info)
{
    info->library_name = "GameLad";
    info->library_version = "0.2.0";
    info->valid_extensions = "gb|gbc";
    info->need_fullpath = false;
    info->block_extract = false;
}

RETRO_API void retro_get_system_av_info(struct retro_system_av_info* info)
{
    info->geometry.base_width = FrameWidth;
    info->geometry.base_height = FrameHeight;
    info->geometry.max_width = FrameWidth;
    info->geometry.max_height = FrameHeight;
    info->geometry.aspect_ratio =
        static_cast<float>(FrameWidth) / static_cast<float>(FrameHeight);
    info->timing.fps = FramesPerSecond;
    info->timing.sample_rate = AudioSampleRate;
}

RETRO_API void retro_set_controller_port_device(unsigned int port, unsigned int device)
{
    if (port == 0)
    {
        const unsigned int baseDevice = device & RETRO_DEVICE_MASK;
        g_controllerEnabled =
            baseDevice == RETRO_DEVICE_JOYPAD ||
            baseDevice == RETRO_DEVICE_ANALOG;
    }
}

RETRO_API void retro_reset(void)
{
    if (!g_gameLoaded)
    {
        return;
    }

    ReadRuntimeOptions();
    if (!ResetEmulatorInPlace())
    {
        ShowMessage("GameLad could not reset the current content.", RETRO_LOG_ERROR);
    }
}

RETRO_API void retro_run(void)
{
    if (g_emulator == nullptr)
    {
        return;
    }

    CheckUpdatedOptions();
    UpdateInput();
    ApplyGameSharkCodes();

    g_nextFrameCycle += CyclesPerFrame;
    while (g_emulator->GetBaseClockCycles() < g_nextFrameCycle)
    {
        const unsigned long long before = g_emulator->GetBaseClockCycles();
        g_emulator->Step();
        if (g_emulator->GetBaseClockCycles() == before)
        {
            g_nextFrameCycle = before;
            break;
        }
    }

    unsigned int avEnable =
        RETRO_AV_ENABLE_VIDEO |
        RETRO_AV_ENABLE_AUDIO;
    if (g_environmentCallback != nullptr)
    {
        g_environmentCallback(
            RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE,
            &avEnable);
    }

    const bool videoEnabled = (avEnable & RETRO_AV_ENABLE_VIDEO) != 0;
    const bool skipVideo = videoEnabled && ShouldSkipVideo();
    if (videoEnabled && g_videoCallback != nullptr)
    {
        if (skipVideo)
        {
            g_videoCallback(nullptr, FrameWidth, FrameHeight, 0);
        }
        else
        {
            ConvertVideoFrame();
            g_videoCallback(
                g_videoBuffer.data(),
                FrameWidth,
                FrameHeight,
                FrameWidth * sizeof(g_videoBuffer[0]));
        }
    }

    FlushAudio((avEnable & RETRO_AV_ENABLE_AUDIO) != 0);
    SetRumble(g_emulator->IsRumbleEnabled());
    g_turboPhase = !g_turboPhase;
}

RETRO_API size_t retro_serialize_size(void)
{
    return g_emulator == nullptr
        ? 0
        : CoreStateHeaderSize + g_emulatorStateSize;
}

RETRO_API bool retro_serialize(void* data, size_t size)
{
    if (g_emulator == nullptr ||
        data == nullptr ||
        size != retro_serialize_size())
    {
        return false;
    }

    std::vector<byte> emulatorState;
    if (!g_emulator->Serialize(emulatorState) ||
        emulatorState.size() != g_emulatorStateSize)
    {
        return false;
    }

    const unsigned long long cycles = g_emulator->GetBaseClockCycles();
    if (cycles < g_nextFrameCycle ||
        cycles - g_nextFrameCycle >
            static_cast<unsigned long long>((std::numeric_limits<std::uint32_t>::max)()))
    {
        return false;
    }

    byte* output = static_cast<byte*>(data);
    const byte magic[8] = { 'G', 'L', 'R', 'E', 'T', 'R', 'O', 0 };
    std::memcpy(output, magic, sizeof(magic));
    WriteUInt32(output + 8, 1);
    WriteUInt32(output + 12, static_cast<std::uint32_t>(cycles - g_nextFrameCycle));
    output[16] = g_turboPhase ? 1 : 0;
    std::memcpy(
        output + CoreStateHeaderSize,
        emulatorState.data(),
        emulatorState.size());
    return true;
}

RETRO_API bool retro_unserialize(const void* data, size_t size)
{
    if (g_emulator == nullptr ||
        data == nullptr ||
        size != retro_serialize_size())
    {
        return false;
    }

    const byte* input = static_cast<const byte*>(data);
    const byte magic[8] = { 'G', 'L', 'R', 'E', 'T', 'R', 'O', 0 };
    if (std::memcmp(input, magic, sizeof(magic)) != 0 ||
        ReadUInt32(input + 8) != 1 ||
        input[16] > 1)
    {
        return false;
    }

    const std::uint32_t cycleOffset = ReadUInt32(input + 12);
    if (cycleOffset > 4096)
    {
        return false;
    }

    std::vector<byte> backup;
    if (!g_emulator->Serialize(backup) ||
        !g_emulator->Deserialize(
            input + CoreStateHeaderSize,
            size - CoreStateHeaderSize))
    {
        return false;
    }

    const unsigned long long cycles = g_emulator->GetBaseClockCycles();
    if (cycles < cycleOffset)
    {
        g_emulator->Deserialize(backup.data(), backup.size());
        return false;
    }
    g_nextFrameCycle = cycles - cycleOffset;
    g_turboPhase = input[16] != 0;
    ResetNetpacketTransaction();
    SetRumble(g_emulator->IsRumbleEnabled());
    return true;
}

RETRO_API void retro_cheat_reset(void)
{
    g_cheatSlots.clear();
    RebuildCheats();
}

RETRO_API void retro_cheat_set(unsigned int index, bool enabled, const char* code)
{
    CheatSlot slot = {};
    slot.Enabled = enabled;
    slot.Code = code == nullptr ? "" : code;
    g_cheatSlots[index] = slot;
    RebuildCheats();
}

RETRO_API bool retro_load_game(const struct retro_game_info* game)
{
    if (game == nullptr)
    {
        return false;
    }

    enum retro_pixel_format pixelFormat = RETRO_PIXEL_FORMAT_XRGB8888;
    if (g_environmentCallback == nullptr ||
        !g_environmentCallback(
            RETRO_ENVIRONMENT_SET_PIXEL_FORMAT,
            &pixelFormat))
    {
        Log(RETRO_LOG_ERROR, "the frontend does not support XRGB8888 video");
        return false;
    }

    StopEmulator();
    g_gameLoaded = false;
    g_cheatSlots.clear();
    g_romData.clear();
    g_bootROMData.clear();
    g_gamePath.clear();
    if (game->data != nullptr && game->size != 0)
    {
        const byte* data = static_cast<const byte*>(game->data);
        g_romData.assign(data, data + game->size);
    }
    else if (game->path != nullptr)
    {
        ReadFile(game->path, g_romData);
    }

    if (g_romData.size() < 0x0150)
    {
        Log(RETRO_LOG_ERROR, "the frontend did not provide a valid ROM image");
        g_romData.clear();
        return false;
    }

    g_gamePath = game->path == nullptr ? "" : game->path;
    ReadStartupOptions();
    ReadRuntimeOptions();
    if (!PrepareBootROM() || !InitializeEmulator())
    {
        StopEmulator();
        g_romData.clear();
        g_bootROMData.clear();
        return false;
    }

    unsigned int performanceLevel = 1;
    g_environmentCallback(
        RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL,
        &performanceLevel);
    g_gameLoaded = true;
    return true;
}

RETRO_API bool retro_load_game_special(
    unsigned int,
    const struct retro_game_info*,
    size_t)
{
    return false;
}

RETRO_API void retro_unload_game(void)
{
    StopEmulator();
    g_gameLoaded = false;
    g_romData.clear();
    g_bootROMData.clear();
    g_gamePath.clear();
    g_cheatSlots.clear();
}

RETRO_API unsigned int retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

RETRO_API void* retro_get_memory_data(unsigned int id)
{
    if (g_emulator == nullptr)
    {
        return nullptr;
    }

    switch (id)
    {
    case RETRO_MEMORY_SAVE_RAM:
        return g_emulator->HasBattery() ? g_emulator->GetSaveRAM() : nullptr;
    case RETRO_MEMORY_RTC:
        return g_emulator->GetRTCData();
    case RETRO_MEMORY_SYSTEM_RAM:
        return g_emulator->GetWorkRAM();
    case RETRO_MEMORY_VIDEO_RAM:
        return g_emulator->GetVideoRAM();
    case RETRO_MEMORY_ROM:
        return const_cast<byte*>(g_emulator->GetROM());
    default:
        return nullptr;
    }
}

RETRO_API size_t retro_get_memory_size(unsigned int id)
{
    if (g_emulator == nullptr)
    {
        return 0;
    }

    switch (id)
    {
    case RETRO_MEMORY_SAVE_RAM:
        return g_emulator->HasBattery() ? g_emulator->GetSaveRAMSize() : 0;
    case RETRO_MEMORY_RTC:
        return g_emulator->GetRTCDataSize();
    case RETRO_MEMORY_SYSTEM_RAM:
        return IsCGBHardware(g_emulator->GetGameBoyMode()) ? 0x8000 : 0x2000;
    case RETRO_MEMORY_VIDEO_RAM:
        return IsCGBHardware(g_emulator->GetGameBoyMode()) ? 0x4000 : 0x2000;
    case RETRO_MEMORY_ROM:
        return g_emulator->GetROMSize();
    default:
        return 0;
    }
}
}
