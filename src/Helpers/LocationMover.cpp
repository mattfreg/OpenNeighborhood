#include "pch.h"
#include "Helpers/LocationMover.h"

#include "Helpers/Utils.h"

XBDM::XboxPath LocationMover::s_CurrentConsoleLocation;
LocationMover::AppLocation LocationMover::s_CurrentAppLocation = LocationMover::AppLocation::Root;

const XBDM::XboxPath &LocationMover::GoToDirectory(const std::string &directory)
{
    s_CurrentConsoleLocation /= directory;

    return s_CurrentConsoleLocation;
}

const XBDM::XboxPath &LocationMover::GoToParent()
{
    s_CurrentConsoleLocation = s_CurrentConsoleLocation.Parent();

    return s_CurrentConsoleLocation;
}
