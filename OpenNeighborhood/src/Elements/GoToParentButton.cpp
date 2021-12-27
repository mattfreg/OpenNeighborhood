#include "pch.h"
#include "Elements/GoToParentButton.h"

#include "Render/TextureManager.h"
#include "Events/AppEvent.h"
#include "Xbox/XboxManager.h"
#include "Elements/File.h"
#include "Render/UI.h"


GoToParentButton::GoToParentButton()
    : Element("", "leftArrow")
{
    auto texture = TextureManager::GetTexture(m_TextureName);
    m_Width = texture->GetWidth();
    m_Height = texture->GetHeight();
}

void GoToParentButton::OnRender()
{
    auto texture = TextureManager::GetTexture(m_TextureName);

    if (ImGui::ImageButton((void *)(intptr_t)texture->GetTextureID(), ImVec2((float)m_Width, (float)m_Height)))
        OnClick();
}

void GoToParentButton::OnClick()
{
    std::string currentLocation = XboxManager::GetCurrentLocation();
    std::string parentLocation = XboxManager::GetParent();

    if (currentLocation == parentLocation)
        return;

    if (parentLocation[parentLocation.length() - 1] == ':')
        parentLocation += '\\';

    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;

    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(parentLocation); });

    if (!success)
        return;

    XboxManager::GoToParent();

    auto fileElements = CreateRef<std::vector<Ref<Element>>>();

    for (auto &file : files)
        fileElements->emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}
