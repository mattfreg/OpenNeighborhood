#include "pch.h"
#include "Core/Log.h"

void Log::Info(const char *message, ...)
{
    va_list argList;
    va_start(argList, message);

    std::string fullMessage = "[Info]: ";
    fullMessage += message;
    Print(fullMessage.c_str(), argList, std::cout);

    va_end(argList);
}

void Log::Error(const char *message, ...)
{
    va_list argList;
    va_start(argList, message);

    std::string fullMessage = "[Error]: ";
    fullMessage += message;
    Print(fullMessage.c_str(), argList, std::cerr);

    va_end(argList);
}

void Log::Print(const char *format, va_list argList, std::ostream &outputStream)
{
    const size_t MAX_SIZE = 2048;

    char buffer[MAX_SIZE] = { 0 };
    vsnprintf(buffer, MAX_SIZE, format, argList);

    outputStream << buffer << '\n';
}
