#pragma once

#include <XBDM.h>

class ConsoleHolder
{
public:
    using TryCallbackFn = std::function<void()>;

    static bool CreateConsole(const std::string &ipAddress);

    inline static XBDM::Console &GetConsole() { return s_Console; }

    static bool Try(const TryCallbackFn &function);

private:
    static XBDM::Console s_Console;
};
