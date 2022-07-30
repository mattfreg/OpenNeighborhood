#pragma once

class LocationMover
{
public:
    enum class Position
    {
        Root,
        DriveList,
        DriveContents,
    };

    inline static const std::string &GetCurrentLocation() { return s_CurrentLocation; }

    inline static void SetCurrentLocation(const std::string &location) { s_CurrentLocation = location; }

    static const std::string &GoToDirectory(const std::string &directory);
    static std::string GetParent();
    static const std::string &GoToParent();

    inline static Position GetCurrentPosition() { return s_CurrentPosition; }

    inline static void SetCurrentPosition(Position position) { s_CurrentPosition = position; }

private:
    static std::string s_CurrentLocation;
    static Position s_CurrentPosition;
};
