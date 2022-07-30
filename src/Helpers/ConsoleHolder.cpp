#include "pch.h"
#include "ConsoleHolder.h"

#include "Render/UI.h"

XBDM::Console ConsoleHolder::s_Console;

bool ConsoleHolder::CreateConsole(const std::string &ipAddress)
{
    s_Console = XBDM::Console(ipAddress);

    return s_Console.OpenConnection();
}

bool ConsoleHolder::Try(const TryCallbackFn &function)
{
    try
    {
        function();
    }
    catch (const std::exception &exception)
    {
        UI::SetErrorMessage(exception.what());
        UI::SetSuccess(false);
    }

    return UI::IsGood();
}
