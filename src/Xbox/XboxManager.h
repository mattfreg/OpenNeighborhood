#pragma once

#include <XBDM.h>

class XboxManager
{
public:
    enum class Position
    {
        Root,
        DriveList,
        DriveContents,
    };

    using TryCallbackFn = std::function<void()>;

    static bool CreateConsole(const std::string &ipAddress, std::string &consoleName, bool keepConnectionOpen = false);

    inline static XBDM::Console &GetConsole() { return s_Console; }

    inline static const std::string &GetCurrentLocation() { return s_CurrentLocation; }

    inline static void SetCurrentLocation(const std::string &location) { s_CurrentLocation = location; }

    static const std::string &GoToDirectory(const std::string &directory);
    static std::string GetParent();
    static const std::string &GoToParent();

    inline static Position GetCurrentPosition() { return s_CurrentPosition; }

    inline static void SetCurrentPosition(Position position) { s_CurrentPosition = position; }

    static bool Try(const TryCallbackFn &function);

private:
    static XBDM::Console s_Console;
    static std::string s_CurrentLocation;
    static Position s_CurrentPosition;
};
