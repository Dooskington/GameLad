#include "pch.hpp"
#include "Logger.hpp"

void Logger::Log(const char* message, ...)
{
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
