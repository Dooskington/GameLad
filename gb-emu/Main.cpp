#include "PCH.hpp"
#include <Emulator.hpp>

const double TimePerFrame = 1.0 / 60.0;

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

    //SDL_CreateRGBSurface(0, 160, 144, 8, )
    byte* pPixels;
    int pitch = 0;
    SDL_LockTexture(pTexture, nullptr, (void**)&pPixels, &pitch);

    // Render Game
    byte* pData = emulator.GetCurrentFrame();
    for (int y = 0; y < 144; y++)
    {
        for (int x = 0; x < 160; x++)
        {
            int index = ((y * 160) + x) * 4;
            pPixels[index + 3] = *pData;    // R
            pPixels[index + 2] = *pData;    // G
            pPixels[index + 1] = *pData;    // B
            pPixels[index + 0] = 0xFF;      // A
            pData++;
        }
    }

    SDL_UnlockTexture(pTexture);

    SDL_RenderCopy(pRenderer, pTexture, nullptr, nullptr);

    // Update window
    SDL_RenderPresent(pRenderer);
}

// TODO: refactor this
std::unique_ptr<SDL_Renderer, SDLRendererDeleter> spRenderer;
std::unique_ptr<SDL_Texture, SDLTextureDeleter> spTexture;
Emulator emulator;

int renderFrames = 0;
double renderElapsedInSec = 0;
double renderTimeInSec = 0;
Uint64 renderStart = SDL_GetPerformanceCounter();

// The emulator will call this whenever we hit VBlank
void VSyncCallback()
{
    Render(spRenderer.get(), spTexture.get(), emulator);

}

void ProcessInput(Emulator& emulator)
{
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

    //std::string romPath = "res/tests/cpu_instrs.gb";            // PASSED
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
        //std::string romPath = "res/tests/01-read_timing.gb";            // FAILED
        //std::string romPath = "res/tests/02-write_timing.gb";            // FAILED
        //std::string romPath = "res/tests/03-modify_timing.gb";            // FAILED

    //std::string romPath = "res/tests/oam_bug.gb";            // FAILED

    //std::string romPath = "res/games/Pokemon - Blue Version.gb";
    //std::string romPath = "res/games/Tetris (World).gb";
    //std::string romPath = "res/games/Super Mario Land (World).gb";
    //std::string romPath = "res/games/Tamagotchi.gb";
    //std::string romPath = "res/games/Battletoads.gb";
    //std::string romPath = "res/games/Tetris.gb";
    
    std::string romPath = "res/games/Mario.gbc";

    //std::string romPath = "res/games/Lemmings.gbc";   // Requires MBC5 - NYI
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
            "Gameboy",
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

    if (emulator.Initialize(romPath.data()))
    {
        emulator.SetVSyncCallback(&VSyncCallback);

        renderStart = SDL_GetPerformanceCounter();
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

            // Emulate one frame on the CPU (70244 cycles or CyclesPerFrame)
            emulator.StepFrame();

            renderFrames++;

            Uint64 renderEnd = SDL_GetPerformanceCounter();

            // Loop until we use up the rest of our frame time
            while (true)
            {
                renderEnd = SDL_GetPerformanceCounter();
                renderElapsedInSec = (double)(renderEnd - renderStart) / SDL_GetPerformanceFrequency();

                // Break out once we use up our time per frame
                if (renderElapsedInSec >= TimePerFrame)
                {
                    break;
                }
            }

            // Print Render FPS every 5 seconds
            renderTimeInSec += renderElapsedInSec;
            if (renderTimeInSec > 0.5)
            {
                // Uncomment to display render FPS
                //Logger::Log("RFPS: %f", renderFrames / renderTimeInSec);
                renderFrames = 0;
                renderTimeInSec = 0;
            }

            renderStart = renderEnd;
        }
    }

    emulator.Stop();

    spTexture.reset();
    spRenderer.reset();
    spWindow.reset();
    SDL_Quit();

    return 0;
}
