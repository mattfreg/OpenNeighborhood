#include "pch.h"
#include "ConsoleStore.h"

#include "Render/UI.h"

XBDM::Console ConsoleStore::s_Console;
std::string ConsoleStore::s_CopiedPath;

bool ConsoleStore::CreateConsole(const std::string &ipAddress)
{
    s_Console = XBDM::Console(ipAddress);

    return s_Console.OpenConnection();
}

bool ConsoleStore::Try(const TryCallbackFn &function)
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
