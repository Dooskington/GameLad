#include "libretro.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace
{
int g_failures = 0;
unsigned int g_videoFrames = 0;
size_t g_audioFrames = 0;
unsigned int g_inputPolls = 0;
bool g_optionsRegistered = false;
bool g_contentOverrideRegistered = false;
bool g_netpacketRegistered = false;
bool g_memoryMapsRegistered = false;
bool g_achievementsEnabled = false;
bool g_rumbleEnabled = false;
unsigned int g_serialPackets = 0;
retro_netpacket_callback g_netpacketCallbacks = {};

std::map<std::string, std::string> g_options =
{
    { "gamelad_model", "auto" },
    { "gamelad_boot_rom", "disabled" },
    { "gamelad_allow_opposing_directions", "disabled" },
    { "gamelad_frameskip", "disabled" },
    { "gamelad_frameskip_threshold", "33" }
};

void Require(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << message << std::endl;
        ++g_failures;
    }
}

bool SetRumble(
    unsigned int,
    enum retro_rumble_effect effect,
    std::uint16_t strength)
{
    if (effect == RETRO_RUMBLE_STRONG)
    {
        g_rumbleEnabled = strength != 0;
    }
    return true;
}

bool Environment(unsigned int command, void* data)
{
    switch (command)
    {
    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
        *static_cast<unsigned int*>(data) = 2;
        return true;
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2:
        g_optionsRegistered = data != nullptr;
        return true;
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_UPDATE_DISPLAY_CALLBACK:
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY:
    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
    case RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK:
    case RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS:
    case RETRO_ENVIRONMENT_SET_MINIMUM_AUDIO_LATENCY:
    case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
    case RETRO_ENVIRONMENT_SET_MESSAGE_EXT:
        return true;
    case RETRO_ENVIRONMENT_SET_CONTENT_INFO_OVERRIDE:
        g_contentOverrideRegistered = data != nullptr;
        return true;
    case RETRO_ENVIRONMENT_SET_NETPACKET_INTERFACE:
        g_netpacketRegistered = data != nullptr;
        if (data != nullptr)
        {
            g_netpacketCallbacks =
                *static_cast<const retro_netpacket_callback*>(data);
        }
        return true;
    case RETRO_ENVIRONMENT_GET_VARIABLE:
    {
        retro_variable* variable = static_cast<retro_variable*>(data);
        std::map<std::string, std::string>::const_iterator value =
            g_options.find(variable->key);
        variable->value =
            value == g_options.end() ? nullptr : value->second.c_str();
        return variable->value != nullptr;
    }
    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
        *static_cast<bool*>(data) = false;
        return true;
    case RETRO_ENVIRONMENT_GET_INPUT_BITMASKS:
        return true;
    case RETRO_ENVIRONMENT_GET_CAN_DUPE:
        *static_cast<bool*>(data) = true;
        return true;
    case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
        static_cast<retro_rumble_interface*>(data)->set_rumble_state = SetRumble;
        return true;
    case RETRO_ENVIRONMENT_GET_VFS_INTERFACE:
    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
        return false;
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    {
        static const char SystemDirectory[] = ".";
        *static_cast<const char**>(data) = SystemDirectory;
        return true;
    }
    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
        return *static_cast<enum retro_pixel_format*>(data) ==
            RETRO_PIXEL_FORMAT_XRGB8888;
    case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
    {
        const retro_memory_map* memoryMap =
            static_cast<const retro_memory_map*>(data);
        g_memoryMapsRegistered =
            memoryMap != nullptr &&
            memoryMap->descriptors != nullptr &&
            memoryMap->num_descriptors >= 6;
        return true;
    }
    case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
        g_achievementsEnabled = *static_cast<const bool*>(data);
        return true;
    case RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE:
        *static_cast<unsigned int*>(data) =
            RETRO_AV_ENABLE_VIDEO |
            RETRO_AV_ENABLE_AUDIO;
        return true;
    case RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION:
        *static_cast<unsigned int*>(data) = 1;
        return true;
    default:
        return false;
    }
}

void Video(
    const void* data,
    unsigned int width,
    unsigned int height,
    size_t pitch)
{
    Require(data != nullptr, "video callback received an unexpected duplicate");
    Require(width == 160 && height == 144, "video geometry is incorrect");
    Require(pitch == 160 * sizeof(std::uint32_t), "video pitch is incorrect");
    ++g_videoFrames;
}

void AudioSample(int16_t, int16_t)
{
}

size_t AudioBatch(const int16_t*, size_t frames)
{
    g_audioFrames += frames;
    return frames;
}

void InputPoll()
{
    ++g_inputPolls;
}

int16_t InputState(
    unsigned int port,
    unsigned int device,
    unsigned int,
    unsigned int id)
{
    if (port != 0 || device != RETRO_DEVICE_JOYPAD)
    {
        return 0;
    }
    if (id == RETRO_DEVICE_ID_JOYPAD_MASK)
    {
        return static_cast<int16_t>(1 << RETRO_DEVICE_ID_JOYPAD_A);
    }
    return id == RETRO_DEVICE_ID_JOYPAD_A ? 1 : 0;
}

void SendNetpacket(
    int,
    const void* buffer,
    size_t length,
    std::uint16_t)
{
    if (buffer == nullptr || length != 4)
    {
        return;
    }

    const std::uint8_t* packet = static_cast<const std::uint8_t*>(buffer);
    if (packet[0] == 0x47 && packet[1] == 0x01)
    {
        ++g_serialPackets;
        const std::uint8_t response[4] = { 0x47, 0x02, packet[2], 1 };
        g_netpacketCallbacks.receive(response, sizeof(response), 0);
    }
}

void PollNetpacket()
{
}

std::vector<std::uint8_t> CreateROM(
    std::uint8_t cartridgeType,
    std::uint8_t ramSize)
{
    std::vector<std::uint8_t> rom(0x8000, 0x00);
    rom[0x0100] = 0x00;
    rom[0x0101] = 0x18;
    rom[0x0102] = 0xFD;
    rom[0x0147] = cartridgeType;
    rom[0x0148] = 0x00;
    rom[0x0149] = ramSize;
    return rom;
}

bool LoadROM(std::vector<std::uint8_t>& rom)
{
    retro_game_info game = {};
    game.path = "memory.gb";
    game.data = rom.data();
    game.size = rom.size();
    return retro_load_game(&game);
}
}

int main()
{
    retro_set_environment(Environment);
    retro_set_video_refresh(Video);
    retro_set_audio_sample(AudioSample);
    retro_set_audio_sample_batch(AudioBatch);
    retro_set_input_poll(InputPoll);
    retro_set_input_state(InputState);
    retro_init();

    retro_system_info systemInfo = {};
    retro_get_system_info(&systemInfo);
    Require(retro_api_version() == RETRO_API_VERSION, "API version is incorrect");
    Require(
        systemInfo.need_fullpath == false,
        "core should accept frontend-provided ROM data");
    Require(g_optionsRegistered, "core options were not registered");
    Require(g_contentOverrideRegistered, "content override was not registered");
    Require(g_netpacketRegistered, "link-cable netpacket support was not registered");

    std::vector<std::uint8_t> mbc3ROM = CreateROM(0x10, 0x02);
    Require(LoadROM(mbc3ROM), "MBC3 ROM did not load");
    Require(g_memoryMapsRegistered, "memory maps were not registered");
    Require(g_achievementsEnabled, "achievements were not enabled");
    Require(
        retro_get_memory_size(RETRO_MEMORY_SAVE_RAM) == 0x2000,
        "save RAM size is incorrect");
    Require(
        retro_get_memory_size(RETRO_MEMORY_RTC) == 13,
        "RTC persistence size is incorrect");
    Require(
        retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM) == 0x2000,
        "DMG system RAM size is incorrect");

    retro_run();
    Require(g_videoFrames == 1, "video callback was not invoked");
    Require(g_audioFrames != 0, "audio callback was not invoked");
    Require(g_inputPolls == 1, "input was not polled");

    std::uint8_t* systemRAM = static_cast<std::uint8_t*>(
        retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
    Require(systemRAM != nullptr, "system RAM pointer is missing");
    systemRAM[0x123] = 0x5A;

    const size_t stateSize = retro_serialize_size();
    std::vector<std::uint8_t> state(stateSize);
    Require(stateSize != 0, "save-state size is zero");
    Require(retro_serialize(state.data(), state.size()), "serialization failed");
    systemRAM[0x123] = 0x11;
    Require(
        retro_unserialize(state.data(), state.size()),
        "unserialization failed");
    Require(systemRAM[0x123] == 0x5A, "system RAM was not restored");

    retro_cheat_set(0, true, "761-00F");
    retro_cheat_reset();

    retro_cheat_set(0, true, "014223C1");
    retro_run();
    Require(systemRAM[0x123] == 0x42, "GameShark code was not applied");
    retro_cheat_reset();

    std::uint8_t* saveRAM = static_cast<std::uint8_t*>(
        retro_get_memory_data(RETRO_MEMORY_SAVE_RAM));
    std::uint8_t* rtcData = static_cast<std::uint8_t*>(
        retro_get_memory_data(RETRO_MEMORY_RTC));
    std::uint8_t* systemRAMBeforeReset = systemRAM;
    saveRAM[0] = 0x66;
    retro_reset();
    std::uint8_t* saveRAMAfterReset = static_cast<std::uint8_t*>(
        retro_get_memory_data(RETRO_MEMORY_SAVE_RAM));
    Require(
        saveRAMAfterReset == saveRAM,
        "reset changed the frontend save-RAM pointer");
    Require(
        retro_get_memory_data(RETRO_MEMORY_RTC) == rtcData,
        "reset changed the frontend RTC pointer");
    Require(
        retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM) == systemRAMBeforeReset,
        "reset changed the frontend system-RAM pointer");
    Require(
        saveRAMAfterReset[0] == 0x66,
        "reset did not preserve cartridge RAM");
    retro_unload_game();

    std::vector<std::uint8_t> rumbleROM = CreateROM(0x1E, 0x02);
    Require(LoadROM(rumbleROM), "MBC5 rumble ROM did not load");
    retro_cheat_set(0, true, "01080040");
    retro_run();
    Require(g_rumbleEnabled, "cartridge rumble was not forwarded");

    Require(
        g_netpacketCallbacks.start != nullptr &&
        g_netpacketCallbacks.receive != nullptr &&
        g_netpacketCallbacks.stop != nullptr,
        "netpacket callbacks are incomplete");
    g_netpacketCallbacks.start(1, SendNetpacket, PollNetpacket);
    retro_cheat_set(1, true, "018001FF");
    retro_cheat_set(2, true, "018102FF");
    retro_run();
    Require(g_serialPackets != 0, "link-cable serial packets were not sent");
    g_netpacketCallbacks.stop();
    retro_cheat_reset();

    retro_unload_game();
    Require(!g_rumbleEnabled, "rumble was not stopped on unload");

    retro_deinit();
    if (g_failures != 0)
    {
        std::cerr << g_failures << " libretro integration test(s) failed."
                  << std::endl;
        return 1;
    }

    std::cout << "All libretro integration tests passed." << std::endl;
    return 0;
}
