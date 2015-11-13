#pragma once

class Logger
{
private:
    Logger() { }

public:
    static void Disable();
    static void Enable();
    static void Log(const char* message, ...);
    static void LogError(const char* message, ...);

private:
    static bool m_IsEnabled;
};
