#include "PCH.hpp"
#include "Emulator.hpp"

Emulator::Emulator() :
    m_isRunning(true),
    m_windowTitle("Gameboy"),
    m_windowWidth(160),
    m_windowHeight(144),
    m_window(nullptr),
    m_renderer(nullptr)
{
}

void Emulator::Start()
{
    if (Initialize())
    {
        Logger::Log("Gameboy is up and running!");
        // Game loop
        SDL_Event event;
        while (m_isRunning)
        {
            // Poll for window input
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    m_isRunning = false;
                }
            }

            Update();
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

void Emulator::Update()
{

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
        Logger::LogError("Window could not be created! SDL error: %s", SDL_GetError());
        return false;
    }

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_renderer == nullptr)
    {
        Logger::LogError("Renderer could not be created! SDL error: %s", SDL_GetError());
        return false;
    }

    return true;
}
