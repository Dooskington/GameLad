#pragma once

class Logger
{
private:
    Logger() { }

public:
    static void Log(const char* message, ...);
    static void LogError(const char* message, ...);
};
