#include "pch.h"
#include "Xbox/XboxManager.h"

#include "Render/UI.h"

XBDM::Console XboxManager::s_Console;
std::string XboxManager::s_CurrentLocation = "\\";
XboxManager::Position XboxManager::s_CurrentPosition = XboxManager::Position::Root;

bool XboxManager::CreateConsole(const std::string &ipAddress, bool keepConnectionOpen)
{
    s_Console = XBDM::Console(ipAddress);

    if (!s_Console.OpenConnection())
        return false;

    if (!keepConnectionOpen)
        s_Console.CloseConnection();

    return true;
}

const std::string &XboxManager::GoToDirectory(const std::string &directory)
{
    if (directory.front() != '\\' && s_CurrentLocation != "\\")
        s_CurrentLocation += '\\';

    s_CurrentLocation += directory;

    return s_CurrentLocation;
}

std::string XboxManager::GetParent()
{
    size_t lastSeparatorIndex = s_CurrentLocation.find_last_of('\\');

    if (lastSeparatorIndex == 0)
        return "\\";

    return s_CurrentLocation.substr(0, lastSeparatorIndex);
}

const std::string &XboxManager::GoToParent()
{
    s_CurrentLocation = GetParent();

    return s_CurrentLocation;
}

bool XboxManager::Try(const TryCallbackFn &function)
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
