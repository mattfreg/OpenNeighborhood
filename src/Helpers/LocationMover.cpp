#include "pch.h"
#include "Helpers/LocationMover.h"

#include "Helpers/Utils.h"

std::string LocationMover::s_CurrentConsoleLocation = "\\";
LocationMover::AppLocation LocationMover::s_CurrentAppLocation = LocationMover::AppLocation::Root;

std::string LocationMover::GetCurrentConsoleLocation()
{
    // XBDM expects paths to have the following format:  <drive>:\Path\To\File
    // but s_CurrentConsoleLocation is like so:         \<drive>:\Path\To\File
    // so we need to remove the first backslash
    return s_CurrentConsoleLocation.substr(1);
}

void LocationMover::SetCurrentConsoleLocation(const std::string &location)
{
    if (location.front() != '\\')
        s_CurrentConsoleLocation = "\\" + location;
    else
        s_CurrentConsoleLocation = location;
}

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
    return Utils::DirName(s_CurrentConsoleLocation);
}

const std::string &LocationMover::GoToParent()
{
    s_CurrentConsoleLocation = GetParent();

    return s_CurrentConsoleLocation;
}
