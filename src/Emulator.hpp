#pragma once

class Emulator
{
    public:
        Emulator();

        void Start();
        void Stop();
        void Update();
        void Render();

    private:
        bool m_isRunning;
        std::string m_windowTitle;
        int m_windowWidth;
        int m_windowHeight;
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
};
