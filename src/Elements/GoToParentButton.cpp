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
    std::string currentConsoleLocation = LocationMover::GetCurrentConsoleLocation();
    std::string parentConsoleLocation = LocationMover::GetParent();

    if (parentConsoleLocation == "\\")
    {
        // If currentLocation is not the drive root yet we need to call GoToParent to set it to the current
        // drive root but we don't want to do it the next times we click on the GoToParentButton
        if (currentConsoleLocation != "\\")
            LocationMover::GoToParent();

        std::vector<Ref<Element>> elements;

        LocationMover::AppLocation currentAppLocation = LocationMover::GetCurrentAppLocation();
        switch (currentAppLocation)
        {
        case LocationMover::AppLocation::DriveContents:
            elements = UI::CreateDriveElements();
            break;
        case LocationMover::AppLocation::DriveList:
            elements = UI::CreateRootElements();
            break;
        }

        if (elements.empty())
            return;

        ContentsChangeEvent event(elements);
        m_EventCallback(event);

        return;
    }

    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    // If the parent location ends with ':', then it's a drive and we need to add '\' at the end
    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(parentConsoleLocation.back() == ':' ? parentConsoleLocation + '\\' : parentConsoleLocation); });

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
