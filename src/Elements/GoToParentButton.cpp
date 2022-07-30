#include "pch.h"
#include "Elements/GoToParentButton.h"

#include "Render/TextureMap.h"
#include "Events/AppEvent.h"
#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/ConfigManager.h"
#include "Elements/File.h"
#include "Elements/Drive.h"
#include "Elements/AddXboxButton.h"
#include "Elements/Xbox.h"
#include "Render/UI.h"

GoToParentButton::GoToParentButton()
    : Element("", "leftArrow")
{
    auto texture = TextureMap::GetTexture(m_TextureName);
    m_Width = texture->GetWidth();
    m_Height = texture->GetHeight();
}

void GoToParentButton::OnRender()
{
    auto texture = TextureMap::GetTexture(m_TextureName);

    if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>(texture->GetTextureID())), ImVec2(static_cast<float>(m_Width), static_cast<float>(m_Height))))
        OnClick();
}

void GoToParentButton::OnClick()
{
    const std::string &currentLocation = LocationMover::GetCurrentConsoleLocation();
    std::string parentLocation = LocationMover::GetParent();

    if (parentLocation == "\\")
    {
        // If currentLocation is not the drive root yet we need to call GoToParent to set it to the current
        // drive root but we don't want to do it the next times we click on the GoToParentButton
        if (currentLocation != "\\")
            LocationMover::GoToParent();

        LocationMover::AppLocation currentPosition = LocationMover::GetCurrentAppLocation();
        switch (currentPosition)
        {
        case LocationMover::AppLocation::DriveContents:
            GoToDrives();
            break;
        case LocationMover::AppLocation::DriveList:
            GoToRoot();
            break;
        }

        return;
    }

    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    // If the parent location ends with ':', then it's a drive and we need to add '\' at the end
    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(parentLocation.back() == ':' ? parentLocation + '\\' : parentLocation); });

    if (!success)
        return;

    LocationMover::GoToParent();

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void GoToParentButton::GoToDrives()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::vector<XBDM::Drive> drives;

    bool success = ConsoleStore::Try([&]() { drives = console.GetDrives(); });

    if (!success)
        return;

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::DriveList);

    auto driveElements = std::vector<Ref<Element>>();
    driveElements.reserve(drives.size());

    for (auto &drive : drives)
        driveElements.emplace_back(CreateRef<Drive>(drive));

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}

void GoToParentButton::GoToRoot()
{
    auto elements = std::vector<Ref<Element>>();

    elements.emplace_back(CreateRef<AddXboxButton>());

    ConfigManager::Config config = ConfigManager::GetConfig();

    for (auto &[xboxName, _] : config)
        if (config.get(xboxName).has("ip_address"))
            elements.emplace_back(CreateRef<Xbox>(xboxName, config.get(xboxName).get("ip_address")));

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::Root);

    ContentsChangeEvent event(elements);
    m_EventCallback(event);
}
