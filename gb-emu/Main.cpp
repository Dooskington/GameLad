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
            Logger::Log("SDL_Window destroyed.");
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
            Logger::Log("SDL_Renderer destroyed.");
        }
    }
};

void Render(SDL_Renderer* pRenderer)
{
    // Clear window
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);

    // Render Game
    // TODO: Get the current frame and render it

    // Update window
    SDL_RenderPresent(pRenderer);
}

int main(int argc, char** argv)
{
    bool isRunning = true;
    std::unique_ptr<SDL_Window, SDLWindowDeleter> spWindow;
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> spRenderer;
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

    Emulator emulator;

    if (emulator.Initialize())
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

            // Emulate one frame on the CPU (70244 cycles or CyclesPerFrame)
            emulator.StepFrame();
            Render(spRenderer.get());

            // If we haven't used up our time, we need to delay the rest of the frame time
            Uint32 frameElapsedTime = SDL_GetTicks() - frameStartTime;
            if (frameElapsedTime < TimePerFrame)
            {
                Uint32 delay = TimePerFrame - frameElapsedTime;

                // Sleep for (16ms - elapsed frame time)
                SDL_Delay(delay);
            }
        }
    }

    emulator.Stop();

    spRenderer.reset();
    spWindow.reset();
    SDL_Quit();

    return 0;
}
