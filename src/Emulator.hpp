#pragma once

#include "CPU.hpp"

class Emulator
{
public:
    Emulator();

    void Start();
    void Stop();
    void Render();

private:
    bool Initialize();

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

private:
    bool m_isRunning;
    std::string m_windowTitle;
    int m_windowWidth;
    int m_windowHeight;

    std::unique_ptr<SDL_Window, SDLWindowDeleter> m_window;
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> m_renderer;
    std::unique_ptr<CPU> m_cpu;
};
