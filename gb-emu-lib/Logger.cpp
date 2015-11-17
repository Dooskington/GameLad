#include "pch.hpp"
#include "Logger.hpp"

bool Logger::m_IsEnabled = true;

void Logger::Disable()
{
    m_IsEnabled = false;
}

void Logger::Enable()
{
    m_IsEnabled = true;
}

void Logger::Log(const char* message, ...)
{
    if (!m_IsEnabled)
    {
        return;
    }

    va_list argPointer;
    va_start(argPointer, message);

    char buffer[1024];
    std::vsnprintf(buffer, ARRAYSIZE(buffer), message, argPointer);

    va_end(argPointer);
    std::cout << buffer << std::endl;
}

void Logger::LogError(const char* message, ...)
{
    va_list argPointer;
    va_start(argPointer, message);

    char buffer[1024];
    std::vsnprintf(buffer, ARRAYSIZE(buffer), message, argPointer);

    va_end(argPointer);
    std::cerr << buffer << std::endl;
}

void Logger::LogCharacter(char character)
{
    std::cout << character << std::flush;
}
