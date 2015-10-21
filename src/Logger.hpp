#pragma once

class Logger
{
private:
    Logger() { }

public:
    static void Log(const std::string message, ...);
    static void LogError(const std::string message, ...);
};
