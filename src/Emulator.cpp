#include "PCH.hpp"
#include "Emulator.hpp"

Emulator::Emulator() :
    m_isRunning(true),
    m_windowTitle("Gameboy"),
    m_windowWidth(160),
    m_windowHeight(144),
    m_window(nullptr),
    m_renderer(nullptr),
    m_cpu(nullptr),
    m_timePerFrame((1.0 / 60.0) * 1000.0)
{
}

void Emulator::Start()
{
    if (Initialize())
    {
        // Set the current time to the start time
        Uint32 currentTime = SDL_GetTicks();

        Logger::Log("Gameboy is up and running!");
        SDL_Event event;
        while (m_isRunning)
        {
            Uint32 frameStartTime = SDL_GetTicks();
            Uint32 frameTime = frameStartTime - currentTime;
            currentTime = frameStartTime;

            // Poll for window input
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    m_isRunning = false;
                }
            }

            // Emulate one frame on the CPU (70244 cycles)
            m_cpu->StepFrame();

            // The frame is over when we have spent 16ms here
            if(m_timePerFrame - frameTime > 0)
            {
                // Sleep for (int)(m_frameTime - frameTime)
                //SDL_Delay((int)(m_timePerFrame - frameTime));
                std::cout << "Need to sleep for: " << (int)(m_timePerFrame - frameTime) << std::endl;
                // int cast needs to be there or else we get underflow
                // Also seem to be getting a negative number ( like -18 ) every few seconds...
                // SDL Delay seems to just lock up the window indefinetly
            }

            Render();
        }
    }

    Stop();
}

void Emulator::Stop()
{
    // Free SDL resources
    if (m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_window != nullptr)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    Mix_Quit();
    SDL_Quit();
}

void Emulator::Render()
{
    // Clear window
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    // Render Game

    // Update window
    SDL_RenderPresent(m_renderer);
}

bool Emulator::Initialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        Logger::LogError("SDL could not initialize! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        Logger::LogError("SDL_mixer could not be initialized! SDL_mixer error: '%s'", Mix_GetError());
        return false;
    }

    // Create window
    m_window = SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN);
    if (m_window == nullptr)
    {
        Logger::LogError("Window could not be created! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr)
    {
        Logger::LogError("Renderer could not be created! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create CPU
    m_cpu = std::make_unique<CPU>();

    return true;
}
