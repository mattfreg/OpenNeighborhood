#include "pch.h"
#include "Elements/Drive.h"

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Elements/File.h"
#include "Render/UI.h"

Drive::Drive(const XBDM::Drive &data)
    : m_Data(data), Element(data.FriendlyName, "drive")
{
}

void Drive::OnClick()
{
    // Convert the drive name to lower case
    std::transform(m_Data.Name.begin(), m_Data.Name.end(), m_Data.Name.begin(), [](unsigned char c) { return std::tolower(c); });

    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;

    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(XboxManager::GetCurrentLocation() + m_Data.Name + '\\'); });

    if (!success)
        return;

    // Set the current location in the console to be the root of the current drive
    XboxManager::GoToDirectory(m_Data.Name);

    XboxManager::SetCurrentPosition(XboxManager::Position::DriveContents);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}
