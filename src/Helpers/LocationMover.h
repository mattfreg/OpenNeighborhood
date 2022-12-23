#pragma once

#include <XBDM.h>

class LocationMover
{
public:
    // The location inside OpenNeighborhood and not the path to the current
    // directory in the console.
    // - Root: at the application root, with the Add Xbox button (OpenNeighborhood)
    // - DriveList: after clicking on an Xbox, where the list of all drives is shown (OpenNeighborhood > Jtag)
    // - DriveContents: inside of a drive, where the console location starts (OpenNeighborhood > Jtag > hdd:)
    enum class AppLocation
    {
        Root,
        DriveList,
        DriveContents,
    };

    inline static const XBDM::XboxPath &GetCurrentConsoleLocation() { return s_CurrentConsoleLocation; }

    static void SetCurrentConsoleLocation(const XBDM::XboxPath &location) { s_CurrentConsoleLocation = location; }

    inline static AppLocation GetCurrentAppLocation() { return s_CurrentAppLocation; }

    inline static void SetCurrentAppLocation(AppLocation location) { s_CurrentAppLocation = location; }

    static const XBDM::XboxPath &GoToDirectory(const std::string &directory);
    static const XBDM::XboxPath &GoToParent();

private:
    // The path to the current directory inside the console.
    // If the PathNodes in the PathPanel are like so: OpenNeighborhood > Jtag > hdd: > Games > Game1
    // s_CurrentConsoleLocation is: hdd:\Games\Game1
    static XBDM::XboxPath s_CurrentConsoleLocation;
    static AppLocation s_CurrentAppLocation;
};
