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
    const XBDM::XboxPath &currentConsoleLocation = LocationMover::GetCurrentConsoleLocation();
    XBDM::XboxPath parentConsoleLocation = currentConsoleLocation.Parent();

    // If parentConsoleLocation is the same as currentConsoleLocation, it means we are already at the drive root
    // and can't go further
    if (parentConsoleLocation == currentConsoleLocation)
    {
        std::vector<Ref<Element>> elements;

        LocationMover::AppLocation currentAppLocation = LocationMover::GetCurrentAppLocation();
        if (currentAppLocation == LocationMover::AppLocation::DriveContents)
            elements = UI::CreateDriveElements();
        else if (currentAppLocation == LocationMover::AppLocation::DriveList)
            elements = UI::CreateRootElements();

        if (elements.empty())
            return;

        ContentsChangeEvent event(elements);
        m_EventCallback(event);

        return;
    }

    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(parentConsoleLocation); });

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
