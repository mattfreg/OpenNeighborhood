#pragma once

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

    inline static const std::string &GetCurrentConsoleLocation() { return s_CurrentConsoleLocation; }

    inline static void SetCurrentConsoleLocation(const std::string &location) { s_CurrentConsoleLocation = location; }

    static const std::string &GoToDirectory(const std::string &directory);
    static std::string GetParent();
    static const std::string &GoToParent();

    inline static AppLocation GetCurrentAppLocation() { return s_CurrentAppLocation; }

    inline static void SetCurrentAppLocation(AppLocation location) { s_CurrentAppLocation = location; }

private:
    // The path to the current directory inside the console.
    // If the PathNodes in the PathPanel are like so: OpenNeighborhood > Jtag > hdd: > Games > Game1
    // s_CurrentConsoleLocation is: \hdd:\Games\Game1
    static std::string s_CurrentConsoleLocation;
    static AppLocation s_CurrentAppLocation;
};
