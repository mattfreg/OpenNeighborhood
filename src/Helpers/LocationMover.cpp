#include "pch.h"
#include "Helpers/LocationMover.h"

std::string LocationMover::s_CurrentConsoleLocation = "\\";
LocationMover::AppLocation LocationMover::s_CurrentAppLocation = LocationMover::AppLocation::Root;

const std::string &LocationMover::GoToDirectory(const std::string &directory)
{
    // If the directory doesn't already start with '\' and if we are not at the root,
    // append '\' at the end of the current location
    if (directory.front() != '\\' && s_CurrentConsoleLocation != "\\")
        s_CurrentConsoleLocation += '\\';

    s_CurrentConsoleLocation += directory;

    return s_CurrentConsoleLocation;
}

std::string LocationMover::GetParent()
{
    size_t lastSeparatorIndex = s_CurrentConsoleLocation.find_last_of('\\');

    if (lastSeparatorIndex == 0)
        return "\\";

    // Only keep what was before the last '\', so the parent directory
    return s_CurrentConsoleLocation.substr(0, lastSeparatorIndex);
}

const std::string &LocationMover::GoToParent()
{
    s_CurrentConsoleLocation = GetParent();

    return s_CurrentConsoleLocation;
}
