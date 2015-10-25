#include "PCH.hpp"
#include "Emulator.hpp"

const Uint32 TimePerFrame = static_cast<Uint32>((1.0 / 60.0) * 1000.0);

Emulator::Emulator() :
    m_isRunning(true),
    m_windowTitle("Gameboy"),
    m_windowWidth(160),
    m_windowHeight(144)
{
}

void Emulator::Start()
{
    if (Initialize())
    {
        SDL_Event event;

        if (!m_cpu->LoadROM("res/tests/cpu_instrs.gb"))
        {
            Logger::Log("Failed to load the Gameboy ROM");
        }
        else
        {
            Logger::Log("Gameboy is up and running!");

            while (m_isRunning)
            {
                Uint32 frameStartTime = SDL_GetTicks();

                // Poll for window input
                while (SDL_PollEvent(&event) != 0)
                {
                    if (event.type == SDL_QUIT)
                    {
                        m_isRunning = false;
                    }
                }

                if (!m_isRunning)
                {
                    // Exit early if the app is closing
                    continue;
                }

                // Emulate one frame on the CPU (70244 cycles or CyclesPerFrame)
                m_cpu->StepFrame();
                Render();

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
    }

    Stop();
}

void Emulator::Stop()
{
    m_cpu.reset();
    m_renderer.reset();
    m_window.reset();

    SDL_Quit();
}

void Emulator::Render()
{
    // Clear window
    SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(m_renderer.get());

    // Render Game
    // TODO: Get the current frame and render it

    // Update window
    SDL_RenderPresent(m_renderer.get());
}

bool Emulator::Initialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        Logger::LogError("SDL could not initialize! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create window
    m_window = std::unique_ptr<SDL_Window, SDLWindowDeleter>(
        SDL_CreateWindow(
            m_windowTitle.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_windowWidth,
            m_windowHeight,
            SDL_WINDOW_SHOWN));
    if (m_window == nullptr)
    {
        Logger::LogError("Window could not be created! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create renderer
    m_renderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(
        SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED));
    if (m_renderer == nullptr)
    {
        Logger::LogError("Renderer could not be created! SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create CPU
    m_cpu = std::make_unique<CPU>();
    if (m_cpu == nullptr)
    {
        Logger::LogError("CPU could not be created!");
        return false;
    }

    if (!m_cpu->Initialize())
    {
        Logger::LogError("CPU could not be initialized!");
        return false;
    }

    return true;
}
