#include "PCH.hpp"
#include <Emulator.hpp>

const Uint32 TimePerFrame = static_cast<Uint32>((1.0 / 60.0) * 1000.0);

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

int main(int argc, char** argv)
{
    bool isRunning = true;
    std::unique_ptr<SDL_Window, SDLWindowDeleter> spWindow;
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> spRenderer;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> spTexture;

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
            160,
            144,
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

    Emulator emulator;

    if (emulator.Initialize("res/tests/cpu_instrs.gb"))
    {
        while (isRunning)
        {
            Uint32 frameStartTime = SDL_GetTicks();

            // Poll for window input
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    isRunning = false;
                }
            }

            if (!isRunning)
            {
                // Exit early if the app is closing
                continue;
            }

            emulator.SetInput(JOYPAD_NONE, JOYPAD_NONE);

            // Emulate one frame on the CPU (70244 cycles or CyclesPerFrame)
            emulator.StepFrame();
            Render(spRenderer.get(), spTexture.get(), emulator);

            // If we haven't used up our time, we need to delay the rest of the frame time
            Uint32 frameElapsedTime = SDL_GetTicks() - frameStartTime;
            if (frameElapsedTime < TimePerFrame)
            {
                Uint32 delay = TimePerFrame - frameElapsedTime;

                // Sleep for (16ms - elapsed frame time)
                SDL_Delay(delay);
            }
            else
            {
                Logger::Log("Frame time was too long: %dms  (Expect less than %dms)", frameElapsedTime, TimePerFrame);
            }
        }
    }

    emulator.Stop();

    spTexture.reset();
    spRenderer.reset();
    spWindow.reset();
    SDL_Quit();

    return 0;
}
