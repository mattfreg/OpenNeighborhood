#pragma once

#include <XBDM.h>

class ConsoleStore
{
public:
    using TryCallbackFn = std::function<void()>;

    static bool CreateConsole(const std::string &ipAddress);

    inline static XBDM::Console &GetConsole() { return s_Console; }

    static bool Try(const TryCallbackFn &function);

    inline static const XBDM::XboxPath &GetCopiedPath() { return s_CopiedPath; }

    static void SetCopiedPath(const XBDM::XboxPath &path) { s_CopiedPath = path; }

private:
    static XBDM::Console s_Console;
    static XBDM::XboxPath s_CopiedPath;
};
