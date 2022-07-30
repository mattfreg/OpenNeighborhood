#include "pch.h"
#include "Helpers/LocationMover.h"

std::string LocationMover::s_CurrentLocation = "\\";
LocationMover::Position LocationMover::s_CurrentPosition = LocationMover::Position::Root;

const std::string &LocationMover::GoToDirectory(const std::string &directory)
{
    // If the directory doesn't already start with '\' and if we are not at the root,
    // append '\' at the end of the current location
    if (directory.front() != '\\' && s_CurrentLocation != "\\")
        s_CurrentLocation += '\\';

    s_CurrentLocation += directory;

    return s_CurrentLocation;
}

std::string LocationMover::GetParent()
{
    size_t lastSeparatorIndex = s_CurrentLocation.find_last_of('\\');

    if (lastSeparatorIndex == 0)
        return "\\";

    // Only keep what was before the last '\', so the parent directory
    return s_CurrentLocation.substr(0, lastSeparatorIndex);
}

const std::string &LocationMover::GoToParent()
{
    s_CurrentLocation = GetParent();

    return s_CurrentLocation;
}
