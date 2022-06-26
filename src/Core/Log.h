#pragma once

class Log
{
public:
    static void Info(const char *message, ...);

    static void Error(const char *message, ...);

private:
    static void Print(const char *format, va_list argList, std::ostream &outputStream);
};
