#include "PCH.hpp"
#include <Emulator.hpp>

// 60 FPS or 16.67ms
const double TimePerFrame = 1.0 / 60.0;

// The number of CPU cycles per frame
const unsigned int CyclesPerFrame = 70224;

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

    if (emulator.Initialize(bootROM.empty() ? nullptr : bootROM.data(), romPath.data()))
    {
        emulator.SetVSyncCallback(&VSyncCallback);

        unsigned int cycles = 0;
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
            while (cycles < CyclesPerFrame)
            {
                cycles += emulator.Step();
            }

            cycles -= CyclesPerFrame;

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

    spTexture.reset();
    spRenderer.reset();
    spWindow.reset();
    SDL_Quit();

    return 0;
}
