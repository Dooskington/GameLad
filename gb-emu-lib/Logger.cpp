#include "PCH.hpp"
#include "Logger.hpp"

void Logger::Log(const std::string message, ...)
{
    va_list argPointer;
    va_start(argPointer, message);

    // Get the size required so we can allocate the correct space
    size_t size = std::vsnprintf(nullptr, 0, message.c_str(), argPointer) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    std::vsnprintf(&buf.get()[0], size, message.c_str(), argPointer);

    va_end(argPointer);
    std::cout << buf.get() << std::endl;
}

void Logger::LogError(const std::string message, ...)
{
    va_list argPointer;
    va_start(argPointer, message);

    // Get the size required so we can allocate the correct space
    size_t size = std::vsnprintf(nullptr, 0, message.c_str(), argPointer) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    std::vsnprintf(&buf.get()[0], size, message.c_str(), argPointer);

    va_end(argPointer);
    std::cerr << buf.get() << std::endl;
}
