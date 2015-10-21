#include "PCH.hpp"
#include "Logger.hpp"

void Logger::Log(const std::string message, ...)
{
    size_t size = message.length() * 2;
    std::unique_ptr<char[]> buf(new char[size]);
    va_list argPointer;
    va_start(argPointer, message);
    vsnprintf_s(&buf.get()[0], size, size, message.c_str(), argPointer);
    va_end(argPointer);

    std::cout << buf.get() << std::endl;
}

void Logger::LogError(const std::string message, ...)
{
    size_t size = message.length() * 2;
    std::unique_ptr<char[]> buf(new char[size]);
    va_list argPointer;
    va_start(argPointer, message);
    vsnprintf_s(&buf.get()[0], size, size, message.c_str(), argPointer);
    va_end(argPointer);
    
    std::cerr << buf.get() << std::endl;
}
