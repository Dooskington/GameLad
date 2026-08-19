#include "PCH.hpp"
#include <Emulator.hpp>
#include <vector>

// The number of base-clock cycles in one Game Boy frame
const unsigned int CyclesPerFrame = 70224;

// The Game Boy's real frame rate is 4194304 / 70224 = 59.7275Hz, not 60Hz.
// Pacing to a flat 1/60s runs the emulator ~0.5% fast, which makes it produce
// audio slightly faster than the audio device consumes it and steadily grows
// output latency.
const double GameBoyClockHz = 4194304.0;
const double TimePerFrame = CyclesPerFrame / GameBoyClockHz;

struct SDLWindowDeleter
{
    void operator()(SDL_Window* window)
    {
        if (window != nullptr)
        {
            SDL_DestroyWindow(window);
        }
    }
};

struct SDLRendererDeleter
{
    void operator()(SDL_Renderer* renderer)
    {
        if (renderer != nullptr)
        {
            SDL_DestroyRenderer(renderer);
        }
    }
};

struct SDLTextureDeleter
{
    void operator()(SDL_Texture* texture)
    {
        if (texture != nullptr)
        {
            SDL_DestroyTexture(texture);
        }
    }
};

void Render(SDL_Renderer* pRenderer, SDL_Texture* pTexture, Emulator& emulator)
{
    // Clear window
    SDL_SetRenderDrawColor(pRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(pRenderer);

    byte* pPixels;
    int pitch = 0;
    SDL_LockTexture(pTexture, nullptr, (void**)&pPixels, &pitch);

    // Render Game
    byte* pData = emulator.GetCurrentFrame();
    memcpy(pPixels, pData, 160 * 144 * 4);

    SDL_UnlockTexture(pTexture);

    SDL_RenderCopy(pRenderer, pTexture, nullptr, nullptr);

    // Update window
    SDL_RenderPresent(pRenderer);
}

// TODO: refactor this
std::unique_ptr<SDL_Renderer, SDLRendererDeleter> spRenderer;
std::unique_ptr<SDL_Texture, SDLTextureDeleter> spTexture;
Emulator emulator;

// Reused scratch buffer for pulling pending samples out of the APU's own
// sample buffer each frame and pushing them to the SDL audio device. The
// emulated audio hardware state is never touched from SDL's audio thread -
// this push (SDL_QueueAudio) happens from the main thread, once per video
// frame, entirely on the producer side.
const size_t AudioScratchFrames = 8192;
std::vector<float> audioScratchBuffer(AudioScratchFrames * 2);
SDL_AudioDeviceID audioDeviceId = 0;

// ConsumeAudioSamples() always writes interleaved stereo floats, so a frame
// is fixed at 2 floats regardless of what the device reports.
const Uint32 AudioBytesPerFrame = 2 * sizeof(float);
Uint32 maxQueuedAudioBytes = 0;

void PumpAudio()
{
    if (audioDeviceId == 0)
    {
        return;
    }

    for (;;)
    {
        size_t framesConsumed = emulator.ConsumeAudioSamples(audioScratchBuffer.data(), AudioScratchFrames);
        if (framesConsumed == 0)
        {
            break;
        }

        // The emulator is paced by SDL_GetPerformanceCounter while the device
        // drains at its own clock. Those never match exactly, so cap how much
        // can sit in the queue - otherwise the drift accumulates into
        // ever-growing audio latency.
        if (SDL_GetQueuedAudioSize(audioDeviceId) < maxQueuedAudioBytes)
        {
            SDL_QueueAudio(audioDeviceId, audioScratchBuffer.data(), (Uint32)(framesConsumed * AudioBytesPerFrame));
        }

        if (framesConsumed < AudioScratchFrames)
        {
            break;
        }
    }
}

// The emulator will call this whenever we hit VBlank
void VSyncCallback()
{
    Render(spRenderer.get(), spTexture.get(), emulator);
}

void ProcessInput(Emulator& emulator)
{
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    byte input = JOYPAD_NONE;
    byte buttons = JOYPAD_NONE;

    if(keys[SDL_SCANCODE_W])
    {
        input |= JOYPAD_INPUT_UP;
    }

    if(keys[SDL_SCANCODE_A])
    {
        input |= JOYPAD_INPUT_LEFT;
    }

    if(keys[SDL_SCANCODE_S])
    {
        input |= JOYPAD_INPUT_DOWN;
    }

    if(keys[SDL_SCANCODE_D])
    {
        input |= JOYPAD_INPUT_RIGHT;
    }

    if(keys[SDL_SCANCODE_K])
    {
        buttons |= JOYPAD_BUTTONS_A;
    }

    if(keys[SDL_SCANCODE_L])
    {
        buttons |= JOYPAD_BUTTONS_B;
    }

    if(keys[SDL_SCANCODE_N])
    {
        buttons |= JOYPAD_BUTTONS_START;
    }

    if(keys[SDL_SCANCODE_M])
    {
        buttons |= JOYPAD_BUTTONS_SELECT;
    }

    emulator.SetInput(input, buttons);
}

int main(int argc, char** argv)
{
    int windowWidth = 160;
    int windowHeight = 144;
    int windowScale = 2;
    if(argc > 1)
    {
        windowScale = atoi(argv[1]);
    }

    std::string bootROM;
    //std::string bootROM = "res/games/dmg_bios.bin";
    //std::string bootROM = "res/games/gbc_bios.bin";

    std::string romPath = "res/tests/cpu_instrs.gb";            // PASSED
        //std::string romPath = "res/tests/01-special.gb";            // PASSED
        //std::string romPath = "res/tests/02-interrupts.gb";         // PASSED
        //std::string romPath = "res/tests/03-op sp,hl.gb";           // PASSED
        //std::string romPath = "res/tests/04-op r,imm.gb";           // PASSED
        //std::string romPath = "res/tests/05-op rp.gb";              // PASSED
        //std::string romPath = "res/tests/06-ld r,r.gb";             // PASSED
        //std::string romPath = "res/tests/07-jr,jp,call,ret,rst.gb"; // PASSED
        //std::string romPath = "res/tests/08-misc instrs.gb";        // PASSED
        //std::string romPath = "res/tests/09-op r,r.gb";             // PASSED
        //std::string romPath = "res/tests/10-bit ops.gb";            // PASSED
        //std::string romPath = "res/tests/11-op a,(hl).gb";          // PASSED

    //std::string romPath = "res/tests/instr_timing.gb";            // PASSED

    //std::string romPath = "res/tests/mem_timing.gb";            // FAILED
        //std::string romPath = "res/tests/01-read_timing.gb";        // FAILED
        //std::string romPath = "res/tests/02-write_timing.gb";       // FAILED
        //std::string romPath = "res/tests/03-modify_timing.gb";      // FAILED

    //std::string romPath = "res/tests/oam_bug.gb";            // FAILED

    //std::string romPath = "res/games/Pokemon - Blue Version.gb";
    //std::string romPath = "res/games/Tetris (World).gb";
    //std::string romPath = "res/games/Super Mario Land (World).gb";
    //std::string romPath = "res/games/Tamagotchi.gb";
    //std::string romPath = "res/games/Battletoads.gb";
    //std::string romPath = "res/games/Tetris.gb";
    //std::string romPath = "res/games/Zelda.gb";
    //std::string romPath = "res/games/plantboy.gb";
    //std::string romPath = "res/games/Metroid.gb";
    //std::string romPath = "res/games/Castlevania.gb";

    // CGB Only
    //std::string romPath = "res/games/Lemmings.gbc";   // Requires MBC5
    //std::string romPath = "res/games/Mario2.gbc";   // Requires MBC5
    if(argc > 2)
    {
        romPath = argv[2];
    }

    bool isRunning = true;
    std::unique_ptr<SDL_Window, SDLWindowDeleter> spWindow;

    SDL_Event event;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        Logger::LogError("SDL could not initialize! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create window
    spWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>(
        SDL_CreateWindow(
            "GameLad",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            windowWidth * windowScale, // Original = 160
            windowHeight * windowScale, // Original = 144
            SDL_WINDOW_SHOWN));
    if (spWindow == nullptr)
    {
        Logger::LogError("Window could not be created! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create renderer
    spRenderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(
        SDL_CreateRenderer(spWindow.get(), -1, SDL_RENDERER_ACCELERATED));
    if (spRenderer == nullptr)
    {
        Logger::LogError("Renderer could not be created! SDL error: '%s'", SDL_GetError());
        return false;
    }

    spTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(
        SDL_CreateTexture(spRenderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144));

    // Open the audio device as a playback-only queue (no callback thread):
    // we push already-generated samples from the main thread in PumpAudio(),
    // keeping the emulated APU state fully independent of SDL's own audio
    // thread/timing.
    SDL_AudioSpec desiredSpec = {};
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_F32SYS;
    desiredSpec.channels = 2;
    desiredSpec.samples = 2048;

    SDL_AudioSpec obtainedSpec = {};
    audioDeviceId = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, 0);
    if (audioDeviceId == 0)
    {
        Logger::LogError("Audio device could not be opened! SDL error: '%s'", SDL_GetError());
    }
    else
    {
        // Roughly four frames of audio: enough to ride out frame-time jitter
        // without an audible delay between action and sound.
        maxQueuedAudioBytes = (Uint32)(obtainedSpec.freq * AudioBytesPerFrame * 4.0 * TimePerFrame);
        SDL_PauseAudioDevice(audioDeviceId, 0);
    }

    if (emulator.Initialize(bootROM.empty() ? nullptr : bootROM.data(), romPath.data()))
    {
        if (audioDeviceId != 0)
        {
            emulator.SetAudioSampleRate(obtainedSpec.freq);
        }

        emulator.SetVSyncCallback(&VSyncCallback);

        unsigned long long nextFrameBaseCycle = emulator.GetBaseClockCycles();
        Uint64 frameStart = SDL_GetPerformanceCounter();
        while (isRunning)
        {
            // Poll for window input
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    isRunning = false;
                    emulator.SetVSyncCallback(nullptr);
                }
            }

            if (!isRunning)
            {
                // Exit early if the app is closing
                continue;
            }

            ProcessInput(emulator);

            // Advance a frame's worth of *base-clock* cycles. Step() returns
            // raw CPU cycles, which double in CGB double-speed mode, so
            // counting those would emulate only half a frame per real frame.
            nextFrameBaseCycle += CyclesPerFrame;
            while (emulator.GetBaseClockCycles() < nextFrameBaseCycle)
            {
                unsigned long long before = emulator.GetBaseClockCycles();
                emulator.Step();
                if (emulator.GetBaseClockCycles() == before)
                {
                    // STOP halts the system clock, so no emulated time passes
                    // and there is nothing left to run this frame. Resync the
                    // target rather than accumulating a deficit the CPU would
                    // have to sprint through after the joypad wakes it.
                    nextFrameBaseCycle = before;
                    break;
                }
            }

            PumpAudio();

            Uint64 frameEnd = SDL_GetPerformanceCounter();
            // Loop until we use up the rest of our frame time
            while (true)
            {
                frameEnd = SDL_GetPerformanceCounter();
                double frameElapsedInSec = (double)(frameEnd - frameStart) / SDL_GetPerformanceFrequency();

                // Break out once we use up our time per frame
                if (frameElapsedInSec >= TimePerFrame)
                {
                    break;
                }
            }

            frameStart = frameEnd;
        }
    }

    emulator.Stop();

    if (audioDeviceId != 0)
    {
        SDL_CloseAudioDevice(audioDeviceId);
        audioDeviceId = 0;
    }

    spTexture.reset();
    spRenderer.reset();
    spWindow.reset();
    SDL_Quit();

    return 0;
}
