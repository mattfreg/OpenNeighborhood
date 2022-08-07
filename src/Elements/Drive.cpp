#include "pch.h"
#include "Elements/Drive.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Events/AppEvent.h"
#include "Elements/File.h"
#include "Render/UI.h"

Drive::Drive(const XBDM::Drive &data)
    : Element(data.FriendlyName, "drive"), m_Data(data)
{
}

void Drive::OnClick()
{
    // Convert the drive name to lower case
    std::for_each(m_Data.Name.begin(), m_Data.Name.end(), [](unsigned char c) { c = static_cast<unsigned char>(std::tolower(c)); });

    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(LocationMover::GetCurrentConsoleLocation() + m_Data.Name + '\\'); });

    if (!success)
        return;

    // Set the current location in the console to be the root of the current drive
    LocationMover::GoToDirectory(m_Data.Name);

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::DriveContents);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}
