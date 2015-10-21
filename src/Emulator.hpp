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

private:
    bool m_isRunning;
    std::string m_windowTitle;
    int m_windowWidth;
    int m_windowHeight;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    std::unique_ptr<CPU> m_cpu;
    const Uint32 m_timePerFrame;
};
